// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../constants/Limits.hpp"
#include "../decoder/DecodedChar.hpp"

#include <cassert>
#include <cstdint>


namespace erbsland::conf::impl {


/// The base for a number.
///
/// This class is used to specify the base for a number, and also provides related information to the base.
///
/// @tested `NumberBaseTest`
///
class NumberBase final {
public:
    enum Value : uint8_t {
        Binary,
        Decimal,
        Hexadecimal,
    };

public: // construction
    NumberBase() = default;
    constexpr NumberBase(const Value value) noexcept : _value{value} {} // NOLINT(*-explicit-constructor)

public: // operators
    constexpr auto operator==(const Value other) const noexcept -> bool { return _value == other; }
    constexpr auto operator!=(const Value other) const noexcept -> bool { return _value != other; }

public: // Access the properties
    [[nodiscard]] constexpr auto raw() const noexcept -> Value { return _value; }
    [[nodiscard]] constexpr auto maximumDigits() const noexcept -> std::size_t {
        switch (_value) {
        case Binary: return limits::maxBinaryDigits;
        case Decimal: return limits::maxDecimalDigits;
        case Hexadecimal: return limits::maxHexadecimalDigits;
        default: return {};
        }
    }
    [[nodiscard]] constexpr auto factor() const noexcept -> uint64_t {
        switch (_value) {
        case Binary: return 2U;
        case Decimal: return 10U;
        case Hexadecimal: return 16U;
        default: return {};
        }
    }
    [[nodiscard]] constexpr auto isValidDigit(const Char &character) const noexcept -> bool {
        switch (_value) {
        case Binary: return character == CharClass::BinaryDigit;
        case Decimal: return character == CharClass::DecimalDigit;
        case Hexadecimal: return character == CharClass::HexDigit;
        default: return false;
        }
    }

private:
    Value _value{Decimal};
};


}

