// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ValueInteger.hpp"


#include "LiteralTables.hpp"
#include "Number.hpp"

#include "../SaturationMath.hpp"
#include "../YieldMacros.hpp"

#include <utility>


namespace erbsland::conf::impl::lexer {


/// Scan for a time or quantity unit that may follow the decimal integer.
///
auto scanDecimalSuffix(
    TokenDecoder &decoder,
    Transaction &transaction,
    int64_t number) -> std::optional<LexerToken> {

    assert(decoder.character() == Char::Space || decoder.character() == CharClass::IntegerSuffixChar);

    auto suffixTransaction = Transaction{decoder};
    if (decoder.character() == Char::Space) { // Skip optional space.
        decoder.next();
        if (decoder.character() != CharClass::IntegerSuffixChar) {
            // As a space acts as a separator, we accept the integer as a token and delegate the error
            // handling to the parser. Otherwise, the error location is misleading.
            suffixTransaction.rollback(); // Back to the point where we started testing for a suffix.
            transaction.commit(); // Only commit the integer itself.
            return decoder.createToken(TokenType::Integer, number);
        }
    }

    // At this point, we know that a letter follows the integer (with- or without a space).
    // Therefore, it must be a valid suffix. If not, it is a syntax error.
    String identifier;
    while (decoder.character() == CharClass::IntegerSuffixChar) {
        decoder.character().appendLowerCaseTo(identifier);
        decoder.next();
        if (identifier.size() > 12) {
            decoder.throwSyntaxError(u8"Unknown integer suffix.");
        }
    }

    // Check the table with valid integer suffixes.
    const auto it = LiteralTables::integerSuffixMap.find(identifier);
    if (it == LiteralTables::integerSuffixMap.end()) {
        decoder.throwSyntaxError(u8"Unknown integer suffix.");
    }

    // If it's a byte count, this is still an integer.
    if (std::holds_alternative<LiteralTables::ByteCountSuffix>(it->second)) {
        const auto factor = std::get<LiteralTables::ByteCountSuffix>(it->second).factor;
        if (factor <= 0 || willMultiplyOverflow(number, factor)) {
            decoder.throwError(ErrorCategory::LimitExceeded, u8"The byte count exceeds a 64bit value.");
        }
        suffixTransaction.commit();
        transaction.commit();
        return decoder.createToken(TokenType::Integer, number * factor);
    }

    // If it's a time unit, this is a time delta.
    suffixTransaction.commit();
    transaction.commit();
    const auto unit = std::get<LiteralTables::TimeDeltaSuffix>(it->second).unit;
    return decoder.createToken(TokenType::TimeDelta, TimeDelta{unit, number});
}


auto scanIntegerOrTimeDelta(TokenDecoder &decoder) -> std::optional<LexerToken> {
    if (decoder.character() != CharClass::NumberStart) {
        return {};
    }
    auto transaction = Transaction{decoder};
    auto sign = Sign::Positive;
    bool isDecimal = false;
    int64_t number = 0;
    if (decoder.character() == CharClass::PlusOrMinus) {
        if (decoder.character() == Char::Minus) {
            sign = Sign::Negative;
        }
        decoder.next(); // Skip the sign if there is one.
        decoder.expect(CharClass::DecimalDigit, u8"Expected a digit after the sign.");
    }
    if (decoder.character() == Char::Digit0) {
        std::size_t digitCount = 0;
        decoder.next(); // Skip the leading zero.
        if (decoder.character() == CharClass::LetterX) {
            decoder.next(); // Skip the letter X.
            parseNumber(decoder, NumberBase::Hexadecimal, sign, NumberSeparators::Yes).assignTo(number, digitCount);
            if (digitCount == 0) {
                decoder.throwSyntaxError(u8"Hexadecimal number must contain at least one digit.");
            }
        } else if (decoder.character() == CharClass::LetterB) {
            decoder.next(); // Skip the letter B.
            parseNumber(decoder, NumberBase::Binary, sign, NumberSeparators::Yes).assignTo(number, digitCount);
            if (digitCount == 0) {
                decoder.throwSyntaxError(u8"Binary number must contain at least one digit.");
            }
        } else if (decoder.character() == CharClass::DecimalDigit) {
            decoder.throwSyntaxError(u8"A leading zero in an integer value is not allowed.");
        } else {
            // It's a zero, followed by something else, so we assume this is a zero integer.
            number = 0;
            isDecimal = true;
        }
        // Specifically, check for a decimal point to inform the user that binary floats aren't supported.
        if (decoder.character() == Char::DecimalPoint) {
            decoder.throwSyntaxError(u8"Hexadecimal or binary floats are not supported by the language.");
        }
    } else { // If the number starts with a digit 1-9 expect a regular decimal.
        parseNumber(decoder, NumberBase::Decimal, sign, NumberSeparators::Yes).assignTo(number);
        isDecimal = true;
    }

    // No identifier must not follow Hexadecimal and binary number.
    if (!isDecimal) {
        if (decoder.character() != CharClass::ValidAfterValue) {
            decoder.throwSyntaxError(u8"Unexpected characters after integer value.");
        }
        transaction.commit();
        return decoder.createToken(TokenType::Integer, number);
    }

    // Is it possible that there is a suffix following the decimal integer?
    if (decoder.character() == Char::Space || decoder.character() == CharClass::IntegerSuffixChar) {
        // Scan for a suffix after the decimal number.
        return scanDecimalSuffix(decoder, transaction, number);
    }

    // Ruling out the possibility of a suffix, and with no valid after-value character, this must be a syntax error.
    if (decoder.character() != CharClass::ValidAfterValue) {
        decoder.throwSyntaxError(u8"Unexpected characters after integer value.");
    }

    // At this point, the integer is sufficiently terminated and can be tokenized.
    transaction.commit();
    return decoder.createToken(TokenType::Integer, number);
}


}

