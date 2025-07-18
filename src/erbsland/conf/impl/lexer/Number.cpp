// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Number.hpp"


#include "../SaturationMath.hpp"
#include "../YieldMacros.hpp"


namespace erbsland::conf::impl::lexer {


/// Helper for `parseNumber`.
///
/// @param decoder The decoder.
/// @param base The base of the number to parse.
/// @param digitCount The number of digits.
///
void handleDigitSeparator(Decoder &decoder, const NumberBase base, const std::size_t digitCount) {
    if (decoder.character() == Char::DigitSeparator) {
        if (digitCount == 0) {
            decoder.throwSyntaxError(u8"Number cannot start with a digit separator.");
        }
        decoder.next(); // Skip it, but expect another digit.
        if (decoder.character() == Char::DigitSeparator) {
            decoder.throwSyntaxError(u8"Number cannot contain two consecutive digit separators.");
        }
        if (!base.isValidDigit(decoder.character())) {
            decoder.throwSyntaxOrUnexpectedEndError(u8"Expected another digit after the digit separator.");
        }
    }
}


auto parseNumber(
    Decoder &decoder,
    const NumberBase base,
    const Sign sign,
    const NumberSeparators numberSeparators,
    const std::size_t fixedDigitCount) -> ParseNumberResult {

    if (decoder.character() == Char::EndOfData) {
        decoder.throwUnexpectedEndOfDataError(u8"Expected a number, but the document ended at this point.");
    }
    uint64_t value = 0;
    std::size_t digitCount = 0;
    while (decoder.character() != Char::EndOfData) {
        if (fixedDigitCount > 0 && digitCount >= fixedDigitCount) {
            break;
        }
        if (digitCount > base.maximumDigits()) {
            decoder.throwNumberLimitExceededError();
        }
        if (numberSeparators == NumberSeparators::Yes) {
            handleDigitSeparator(decoder, base, digitCount);
        }
        if (base.isValidDigit(decoder.character())) {
            const auto digitValue = static_cast<uint64_t>(decoder.character().toHexDigitValue());
            if (willMultiplyOverflow(value, base.factor())) {
                decoder.throwNumberLimitExceededError();
            }
            value *= base.factor();
            if (willAddOverflow(value, digitValue)) {
                decoder.throwNumberLimitExceededError();
            }
            value += digitValue;
        } else {
            break;
        }
        ++digitCount;
        decoder.next();
    }
    decoder.checkForErrorAndThrowIt(); // Check if the number parsing was stopped because of an error.
    if (fixedDigitCount > 0 && digitCount < fixedDigitCount) {
        // For a fixed digit count, return -1 instead of throwing an error, as this is used to test numbers
        // and backtrack if the number does not have the expected number of digits.
        return {.value=-1, .digitCount=digitCount};
    }
    // Checks if the number is in the required 64-bit limits.
    if (sign == Sign::Negative) {
        if (value > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1) {
            decoder.throwNumberLimitExceededError();
        }
        // Special handling of the largest negative number to avoid undefined behavior.
        if (value == static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1) {
            value = std::numeric_limits<int64_t>::min();
        } else {
            value = -value;
        }
    } else {
        if (value > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
            decoder.throwNumberLimitExceededError();
        }
    }
    return {.value=static_cast<int64_t>(value), .digitCount=digitCount};
}


}

