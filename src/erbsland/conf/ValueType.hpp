// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "String.hpp"

#include "impl/ComparisonHelper.hpp"

#include <array>
#include <cstdint>


namespace erbsland::conf {


/// The type of value.
///
/// @tested `ValueTypeTest`
///
class ValueType {
public:
    enum Enum : uint8_t {
        Undefined, ///< Undefined type.
        Integer, ///< An integer value.
        Boolean, ///< A boolean value.
        Float, ///< A floating-point value.
        Text, ///< A text value.
        Date, ///< A date value.
        Time, ///< A time value.
        DateTime, ///< A date-time value.
        Bytes, ///< Binary data.
        TimeDelta, ///< A time delta value.
        RegEx, ///< A regular expression value.
        ValueList, ///< A list of values.
        SectionList, ///< A list of sections.
        IntermediateSection, ///< An intermediate section.
        SectionWithNames, ///< A section with names.
        SectionWithTexts, ///< A section with texts.
        Document, ///< The document.
    };

public:
    /// Create an undefined value type.
    ///
    constexpr ValueType() = default;

    /// Create a new value type.
    ///
    constexpr ValueType(const Enum value) noexcept : _value{value} {} // NOLINT(*-explicit-constructor)

    // defaults
    ~ValueType() = default;
    ValueType(const ValueType &) = default;
    auto operator=(const ValueType &) -> ValueType& = default;

public: // operators
    auto operator=(const Enum value) noexcept -> ValueType& { _value = value; return *this; }
    constexpr operator Enum() const noexcept { return _value; } // NOLINT(*-explicit-constructor)

public: // comparison
    ERBSLAND_CONF_COMPARE_MEMBER(const ValueType &other, _value, other._value);
    ERBSLAND_CONF_COMPARE_MEMBER(Enum value, _value, value);
    ERBSLAND_CONF_COMPARE_FRIEND(Enum a, const ValueType &b, a, b._value);

public: // tests
    /// Test if the type is undefined.
    ///
    [[nodiscard]] constexpr auto isUndefined() const noexcept -> bool { return _value == Undefined; }

    /// Test if this is any kind of value map (a section or document).
    ///
    [[nodiscard]] constexpr auto isMap() const noexcept -> bool {
        return _value == SectionWithTexts || _value == SectionWithNames || _value == IntermediateSection
            || _value == Document;
    }

    /// Test if this is any kind of list (section list or value list).
    ///
    [[nodiscard]] constexpr auto isList() const noexcept -> bool {
        return _value == ValueList || _value == SectionList;
    }

    /// Test if this is a single value.
    ///
    [[nodiscard]] constexpr auto isSingle() const noexcept -> bool {
        return _value == Integer || _value == Boolean || _value == Float || _value == Text ||
            _value == Date || _value == Time || _value == DateTime || _value == Bytes ||
            _value == TimeDelta || _value == RegEx;
    }

public: // transformation
    /// Convert this type into text.
    ///
    [[nodiscard]] auto toText() const noexcept -> const String&;

public: // enumeration
    [[nodiscard]] static auto all() noexcept -> const std::array<ValueType, 17>& {
        static const std::array<ValueType, 17> values = {
            Undefined, Integer, Boolean, Float, Text, Date, Time, DateTime, Bytes,
            TimeDelta, RegEx, ValueList, SectionList, IntermediateSection, SectionWithNames,
            SectionWithTexts, Document
        };
        return values;
    }

private:
    Enum _value{Undefined}; ///< The internal value.
    using ValueToTextEntry = std::tuple<Enum, String>;
    using ValueToTextMap = std::array<ValueToTextEntry, 17>;
    static ValueToTextMap _valueToTextMap;
};


}


template <>
struct std::hash<erbsland::conf::ValueType> {
    auto operator()(const erbsland::conf::ValueType& unit) const noexcept -> std::size_t {
        return std::hash<std::underlying_type_t<erbsland::conf::ValueType::Enum>>{}(
            static_cast<erbsland::conf::ValueType::Enum>(unit));
    }
};


template <>
struct std::formatter<erbsland::conf::ValueType> : std::formatter<std::string> {
    auto format(const erbsland::conf::ValueType valueType, format_context& ctx) const {
        return std::formatter<std::string>::format(valueType.toText().toCharString(), ctx);
    }
};

