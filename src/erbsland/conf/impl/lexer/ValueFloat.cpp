// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ValueFloat.hpp"


#include <string>


namespace erbsland::conf::impl::lexer {


/// The result when parsing a decimal number.
///
struct ParseDecimalDigitsResult {
    std::size_t digitCount; ///< The number of digits.
    bool zeroPrefixed; ///< If the number consists of more than one digit and has a zero prefix.
};


/// Converts a string representation of a floating point number to a Float, performing necessary
/// normalization and error checking. Handles removal of digit separators and leading '+'.
/// Throws if the value is invalid or out of range.
///
/// @param decoder  Reference to the decoder for error reporting.
/// @param value    The input string containing the floating point value.
/// @return         Parsed Float value.
///
auto checkAndConvertFloat(TokenDecoder &decoder, String value) -> Float {
    // Remove a leading plus, if present.
    if (value.front() == U'+') {
        value.erase(0, 1);
    }
    // Remove all digit separators (single quotes).
    value.erase(
        std::remove(value.begin(), value.end(), U'\''),
        value.end());
    double result = 0;
#if __cpp_lib_to_chars >= 201611L
    // Use std::from_chars if available for robust conversion.
    auto begin = reinterpret_cast<const char*>(value.data());
    auto end = begin + value.size();
    auto [ptr, ec] = std::from_chars(begin, end, result);
    if (ec != std::errc{}) {
        if (ec == std::errc::result_out_of_range) {
            decoder.throwSyntaxError(u8"The floating point number is out of range.");
        }
        decoder.throwSyntaxError(u8"The floating point number is invalid.");
    }
#else
    // Fallback for older STLs: use std::stod.
    try {
        result = std::stod(value.toCharString());
    } catch (const std::invalid_argument&) {
        decoder.throwSyntaxError(u8"Invalid floating-point literal syntax.");
    } catch (const std::out_of_range&) {
        decoder.throwSyntaxError(u8"Floating-point literal out of range.");
    }
#endif
    return result;
}


/// Parse decimal digits with number separators and return the number of digits, and if the number is zero-prefixed.
///
[[nodiscard]] inline auto parseDecimalDigits(TokenDecoder &decoder) -> ParseDecimalDigitsResult {
    std::size_t digitCount = 0;
    bool hasZeroPrefix = false;
    while (decoder.character() != Char::EndOfData) {
        if (digitCount == 0 && decoder.character() == Char::Digit0) {
            hasZeroPrefix = true;
        }
        if (decoder.character() == Char::DigitSeparator) {
            if (digitCount == 0) {
                decoder.throwSyntaxError(u8"Number cannot start with a digit separator.");
            }
            decoder.next(); // Skip it, but expect another digit.
            if (decoder.character() == Char::DigitSeparator) {
                decoder.throwSyntaxError(u8"Number cannot contain two consecutive digit separators.");
            }
            if (decoder.character() != CharClass::DecimalDigit) {
                decoder.throwSyntaxOrUnexpectedEndError(u8"Expected another digit after the digit separator.");
            }
        }
        if (decoder.character() != CharClass::DecimalDigit) {
            break;
        }
        ++digitCount;
        decoder.next();
    }
    if (hasZeroPrefix && digitCount == 1) {
        hasZeroPrefix = false; // if the number only consists of a single zero digit, this is no prefix.
    }
    return {.digitCount=digitCount, .zeroPrefixed=hasZeroPrefix};
}


auto scanNaN(TokenDecoder &decoder, Transaction &transaction) -> std::optional<LexerToken> {
    decoder.next();
    if (decoder.character() == CharClass::LetterA) {
        decoder.next();
        if (decoder.character() == CharClass::LetterN) {
            decoder.next();
            // Ensure no invalid trailing characters.
            if (decoder.character() != CharClass::ValidAfterValue) {
                decoder.throwSyntaxError(u8"Unexpected characters after 'NaN' literal.");
            }
            transaction.commit();
            return decoder.createToken(TokenType::Float, std::numeric_limits<Float>::quiet_NaN());
        }
    }
    return std::nullopt;
}


auto scanInf(TokenDecoder &decoder, Transaction &transaction, const bool isNegative) -> std::optional<LexerToken> {
    if (decoder.character() == CharClass::LetterI) {
        decoder.next();
        if (decoder.character() == CharClass::LetterN) {
            decoder.next();
            if (decoder.character() == CharClass::LetterF) {
                decoder.next();
                if (decoder.character() != CharClass::ValidAfterValue) {
                    decoder.throwSyntaxError(u8"Unexpected characters after “inf” literal.");
                }
                transaction.commit();
                if (isNegative) {
                    return decoder.createToken(TokenType::Float, -std::numeric_limits<Float>::infinity());
                }
                return decoder.createToken(TokenType::Float, std::numeric_limits<Float>::infinity());
            }
        }
    }
    return std::nullopt;
}


auto scanLiteralFloat(TokenDecoder &decoder) -> std::optional<LexerToken> {
    // Early exit if there's clearly not a literal float keyword ahead.
    if (decoder.character() != CharClass::FloatLiteralStart) {
        return std::nullopt;
    }
    auto transaction = Transaction{decoder};
    bool isNegative = false;
    if (decoder.character() == CharClass::PlusOrMinus) { // Consume optional sign.
        if (decoder.character() == Char::Minus) {
            isNegative = true;
        }
        decoder.next();
    }
    if (decoder.character() == CharClass::LetterN) {
        if (auto token = scanNaN(decoder, transaction)) {
            return token;
        }
    }
    return scanInf(decoder, transaction, isNegative);
}


/// Scans and parses the exponent portion (e.g., 'E+10') of a floating point number.
/// Ensures that an exponent is present and valid, and converts the entire captured float string.
///
/// @param decoder         The token decoder for reading input and reporting errors.
/// @param transaction     Active transaction holding the string to parse.
/// @return                Parsed floating point LexerToken, or nullopt on failure.
///
auto scanFloatAfterExponent(
    TokenDecoder &decoder,
    Transaction &transaction) -> std::optional<LexerToken> {

    // Allow optional sign after 'E' or 'e'.
    if (decoder.character() == CharClass::PlusOrMinus) {
        decoder.next();
    }
    // Exponent must have at least one digit.
    if (decoder.character() != CharClass::DecimalDigit) {
        decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a decimal digit after the exponent.");
    }
    // Read up to 6 decimal digits for the exponent.
    std::size_t digitCount = 0;
    while (decoder.character() == CharClass::DecimalDigit) {
        if (digitCount >= 6) {
            decoder.throwError(ErrorCategory::LimitExceeded, u8"Exponent too long: maximum 6 digits allowed.");
        }
        decoder.next();
        digitCount += 1;
    }
    // Require that the exponent is properly terminated.
    if (decoder.character() != CharClass::ValidAfterValue) {
        decoder.throwSyntaxError(u8"Unexpected trailing characters after exponent.");
    }
    auto value = checkAndConvertFloat(decoder, transaction.capturedString());
    transaction.commit();
    return decoder.createToken(TokenType::Float, value);
}

/// Scans the portion after the decimal point of a floating point value (including optional exponent).
/// Validates correct delimiter placement and digit count.
///
/// @param decoder         The token decoder to read input.
/// @param transaction     Reference for transaction capturing the float string.
/// @param totalDigits     The number of digits already encountered before the decimal.
/// @return                Parsed floating point LexerToken or nullopt on parse error.
///
auto scanFloatAfterDecimalPoint(
    TokenDecoder &decoder,
    Transaction &transaction,
    std::size_t totalDigits) -> std::optional<LexerToken> {

    if (decoder.character() == CharClass::DecimalDigit) {
        // Parse digits after decimal point (the fraction part).
        const auto fractionResult = parseDecimalDigits(decoder);
        totalDigits += fractionResult.digitCount;
    } else if (totalDigits == 0) {
        // No digits before or after decimal point is not a valid float.
        if (decoder.character() != CharClass::ValidAfterValue) {
            decoder.throwSyntaxError(u8"Unexpected character after decimal point.");
        }
        decoder.throwError(
            ErrorCategory::Syntax,
            u8"Floating-point literal must include digits before or after the decimal point.");
    }
    if (totalDigits > 20) {
        decoder.throwError(ErrorCategory::LimitExceeded,
            u8"Literal too long: maximum 20 digits allowed (excluding sign and decimal).");
    }
    if (decoder.character() == CharClass::ExponentStart) {
        // Handle scientific notation (e.g., 'e10').
        decoder.next();
        auto token = scanFloatAfterExponent(decoder, transaction);
        if (!token) {
            decoder.throwSyntaxOrUnexpectedEndError(u8"Missing exponent digits: at least one digit required.");
        }
        return token;
    }
    // Ensure that the float is properly terminated.
    if (decoder.character() != CharClass::ValidAfterValue) {
        decoder.throwSyntaxError(u8"Unexpected trailing characters after exponent.");
    }
    auto value = checkAndConvertFloat(decoder, transaction.capturedString());
    transaction.commit();
    return decoder.createToken(TokenType::Float, value);
}


auto scanFloatFractionOnly(TokenDecoder &decoder) -> std::optional<LexerToken> {
    // Check if the next character could possibly start a float fraction.
    if (!(decoder.character() == CharClass::PlusOrMinus || decoder.character() == Char::DecimalPoint)) {
        return std::nullopt;
    }
    auto transaction = Transaction{decoder};
    if (decoder.character() == CharClass::PlusOrMinus) {
        decoder.next();
    }
    if (decoder.character() != Char::DecimalPoint) {
        return std::nullopt;
    }
    decoder.next();
    return scanFloatAfterDecimalPoint(decoder, transaction, 0);
}


auto scanFloatWithWholePart(TokenDecoder &decoder) -> std::optional<LexerToken> {
    // Only parse if number start is detected (digit or sign).
    if (decoder.character() != CharClass::NumberStart) {
        return std::nullopt;
    }
    auto transaction = Transaction{decoder};
    if (decoder.character() == CharClass::PlusOrMinus) {
        decoder.next();
    }
    if (decoder.character() != CharClass::DecimalDigit) {
        return std::nullopt;
    }
    // Parse the whole-number part of the float.
    std::size_t totalDigits = 0;
    const auto wholeResult = parseDecimalDigits(decoder);
    totalDigits += wholeResult.digitCount;
    if (decoder.character() == CharClass::ExponentStart) {
        decoder.next();
        // Exponent found, enforce syntax and digit count rules.
        if (wholeResult.zeroPrefixed) {
            decoder.throwSyntaxError(u8"Leading zeros not allowed in floating-point literals.");
        }
        if (totalDigits > 20) {
            decoder.throwError(ErrorCategory::LimitExceeded,
                u8"Literal too long: maximum 20 digits allowed (excluding sign and decimal).");
        }
        return scanFloatAfterExponent(decoder, transaction);
    }
    // If no decimal point follows, it's not a float value.
    if (decoder.character() != Char::DecimalPoint) {
        return std::nullopt;
    }
    if (totalDigits > 1 && wholeResult.zeroPrefixed) {
        decoder.throwSyntaxError(u8"Leading zeros not allowed in floating-point literals.");
    }
    decoder.next();
    return scanFloatAfterDecimalPoint(decoder, transaction, totalDigits);
}


}

