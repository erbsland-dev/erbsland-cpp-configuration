// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../NumberBase.hpp"

#include "../decoder/TokenDecoder.hpp"


namespace erbsland::conf::impl::lexer {


// @tested via Lexer tests


/// The result of the `parseNumber` call.
///
struct ParseNumberResult {
    int64_t value;
    std::size_t digitCount;

    void assignTo(int64_t &value) const noexcept {
        value = this->value;
    }

    void assignTo(int64_t &value, std::size_t &digitCount) const noexcept {
        value = this->value;
        digitCount = this->digitCount;
    }
};

/// If number separators are accepted.
///
enum class NumberSeparators : uint8_t {
    Yes,
    No
};

/// The sign of a number.
///
enum class Sign : uint8_t {
    Positive,
    Negative
};


/// Generic parse the number part of a decimal value.
///
/// @param decoder The decoder to use.
/// @param sign The sign of the parsed value.
/// @param numberSeparators If number separators are allowed.
/// @param base The base for the number.
/// @param fixedDigitCount If a fixed number of digits is expected.
/// @return The parsed number and the number of digits.
/// @throws Error If the parsed number exceeds the 64-bit limits, or has a problem with the number separators.
///
[[nodiscard]] auto parseNumber(
    Decoder &decoder,
    NumberBase base,
    Sign sign,
    NumberSeparators numberSeparators,
    std::size_t fixedDigitCount = 0) -> ParseNumberResult;


}

