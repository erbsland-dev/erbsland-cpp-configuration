// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "String.hpp"

#include "impl/ComparisonHelper.hpp"

#include <array>
#include <cstdint>


namespace erbsland::conf {


/// The category of an error.
///
/// @tested `ErrorCategoryTest`
///
class ErrorCategory {
public:
    enum Enum : uint8_t {
        // Error categories defined by the language specification
        IO                  = 1, ///< A problem occurred while reading data from an I/O stream.
        Encoding            = 2, ///< The document contains a problem with UTF-8 encoding.
        UnexpectedEnd       = 3, ///< The document ended unexpectedly.
        Character           = 4, ///< The document contains a control character that is not allowed.
        Syntax              = 5, ///< The document has a syntax error.
        LimitExceeded       = 6, ///< The size of a name, text, or buffer exceeds the permitted limit.
        NameConflict        = 7, ///< The same name has already been defined earlier in the document.
        Indentation         = 8, ///< The indentation of a continued line does not match the previous line.
        Unsupported         = 9, ///< The requested feature/version is not supported by this parser.
        Signature           = 10, ///< The document’s signature was rejected.
        Access              = 11, ///< The document was rejected due to an access check.
        Validation          = 12, ///< The document did not meet one of the validation rules.
        Internal            = 99, ///< The parser encountered an unexpected internal error.
        // Additional categories for the API of this parser.
        ValueNotFound       = 101, ///< A value with a given name-path couldn't be found.
        WrongType           = 102, ///< A value exists but has the wrong type for a conversion.
    };

private:
    constexpr static auto _enumCount = 15;

public:
    /// Create an undefined value type.
    ///
    constexpr ErrorCategory() = default;

    /// Create a new value type.
    ///
    constexpr ErrorCategory(Enum value) noexcept : _value{value} {} // NOLINT(*-explicit-constructor)

    // defaults
    ~ErrorCategory() = default;
    ErrorCategory(const ErrorCategory &) = default;
    auto operator=(const ErrorCategory &) -> ErrorCategory& = default;

public: // operators
    auto operator=(Enum value) noexcept -> ErrorCategory& { _value = value; return *this; }
    constexpr explicit operator Enum() const noexcept { return _value; }
    explicit operator int() const noexcept { return toCode(); }

public: // comparison
    ERBSLAND_CONF_COMPARE_MEMBER(const ErrorCategory &other, _value, other._value);
    ERBSLAND_CONF_COMPARE_MEMBER(Enum value, _value, value);
    ERBSLAND_CONF_COMPARE_FRIEND(Enum a, const ErrorCategory &b, a, b._value);

public: // conversion
    /// Get the text representation of this error category.
    ///
    [[nodiscard]] auto toText() const noexcept -> const String&;

    /// Get the code for this error category.
    ///
    [[nodiscard]] auto toCode() const noexcept -> int;

private:
    Enum _value{Internal}; ///< The internal value.
    using ValueEntry = std::tuple<Enum, int, String>;
    using ValueMap = std::array<ValueEntry, _enumCount>;
    static ValueMap _valueMap;
};


}


template <>
struct std::formatter<erbsland::conf::ErrorCategory> : std::formatter<std::string> {
    auto format(const erbsland::conf::ErrorCategory errorCategory, format_context& ctx) const {
        return std::formatter<std::string>::format(errorCategory.toText().toCharString(), ctx);
    }
};

