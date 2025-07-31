// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "String.hpp"

#include "impl/ComparisonHelper.hpp"

#include <array>
#include <cstdint>


namespace erbsland::conf {


class Date;
class Time;
class DateTime;
class Bytes;
class TimeDelta;
class RegEx;


/// The type of value.
///
/// @tested `ValueTypeTest`
///
class ValueType final {
public:
    /// The enum for this type.
    ///
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
    /// @name Construction and Assignment
    /// @{

    /// Create an undefined value type.
    ///
    constexpr ValueType() = default;

    /// Create a new value type.
    ///
    /// @param value The enum value.
    ///
    constexpr ValueType(const Enum value) noexcept : _value{value} {} // NOLINT(*-explicit-constructor)

    /// Default destructor.
    ~ValueType() = default;
    /// Default copy constructor.
    ValueType(const ValueType &) = default;
    /// Default copy assignment.
    auto operator=(const ValueType &) -> ValueType& = default;

    /// @}

public: // operators
    /// @name Operators
    /// @{

    /// Assign an enum.
    auto operator=(const Enum value) noexcept -> ValueType& { _value = value; return *this; }
    /// Cast back to the enum value.
    constexpr operator Enum() const noexcept { return _value; } // NOLINT(*-explicit-constructor)

    /// @}

public: // comparison
    ERBSLAND_CONF_COMPARE_MEMBER(const ValueType &other, _value, other._value);
    ERBSLAND_CONF_COMPARE_MEMBER(Enum value, _value, value);
    ERBSLAND_CONF_COMPARE_FRIEND(Enum a, const ValueType &b, a, b._value);

public: // tests
    /// @name Tests
    /// @{

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

    /// @}

public: // transformation
    /// Convert this type into text.
    ///
    [[nodiscard]] auto toText() const noexcept -> const String&;

    /// Get a value type for a single value, native type.
    ///
    /// Please note that this method does not support sections or value lists.
    ///
    template<typename T>
    [[nodiscard]] constexpr static auto from() noexcept -> ValueType {
        using BaseT = std::remove_cvref_t<T>;
        if constexpr (std::is_same_v<BaseT, bool>) {
            return Boolean;
        } else if constexpr (std::is_integral_v<BaseT>) {
            return Integer;
        } else if constexpr (std::is_floating_point_v<BaseT>) {
            return Float;
        } else if constexpr (std::is_same_v<BaseT, String> || std::is_same_v<BaseT, std::u8string> || std::is_same_v<BaseT, std::string>) {
            return Text;
        } else if constexpr (std::is_same_v<BaseT, conf::Date>) {
            return Date;
        } else if constexpr (std::is_same_v<BaseT, conf::Time>) {
            return Time;
        } else if constexpr (std::is_same_v<BaseT, conf::DateTime>) {
            return DateTime;
        } else if constexpr (std::is_same_v<BaseT, conf::TimeDelta>) {
            return TimeDelta;
        } else if constexpr (std::is_same_v<BaseT, conf::RegEx>) {
            return RegEx;
        } else if constexpr (std::is_same_v<BaseT, conf::Bytes>) {
            return Bytes;
        }
        return Undefined;
    }

public:
    /// Access the underlying enum value.
    ///
    [[nodiscard]] constexpr auto raw() const noexcept -> Enum {
        return _value;
    }

public: // enumeration
    /// Get an array with all value types.
    ///
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
