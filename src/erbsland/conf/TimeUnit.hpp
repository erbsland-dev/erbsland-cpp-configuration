// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "String.hpp"

#include "impl/utilities/ComparisonHelper.hpp"

#include <array>
#include <cstdint>
#include <unordered_map>


namespace erbsland::conf {


/// A single time unit used in the configuration.
///
/// @tested `TimeUnitTest`
///
class TimeUnit final {
public:
    /// The enumeration.
    ///
    /// @internal
    /// @warning The numeric values of the enumeration are used as indices for
    /// `_valueMap`. Do not change the order unless `_valueMap` is updated too.
    ///
    enum Enum : uint8_t {
        Nanoseconds = 0,
        Microseconds,
        Milliseconds,
        Seconds,
        Minutes,
        Hours,
        Days,
        Weeks,
        Months,
        Years,
    };

public:
    /// @name Construction and Assignment
    /// @{

    /// Create a new time unit.
    ///
    /// @param value The enum value.
    ///
    constexpr TimeUnit(Enum value) noexcept : _value{value} {} // NOLINT(*-explicit-constructor)

    /// Default constructor.
    TimeUnit() = default;
    /// Default destructor.
    ~TimeUnit() = default;
    /// Default copy constructor.
    TimeUnit(const TimeUnit &) = default;
    /// Default copy assignment.
    auto operator=(const TimeUnit &) -> TimeUnit& = default;
    /// @}

public: // comparison
    ERBSLAND_CONF_COMPARE_MEMBER(const TimeUnit &other, _value, other._value);
    ERBSLAND_CONF_COMPARE_MEMBER(Enum other, _value, other);
    ERBSLAND_CONF_COMPARE_FRIEND(Enum a, const TimeUnit &b, a, b._value);

public: // conversion
    /// @name Conversion
    /// @{

    /// Cast this into its enum value.
    [[nodiscard]] constexpr explicit operator Enum() const noexcept { return _value; }
    /// Get this unit as a long text.
    [[nodiscard]] auto toTextLong() const noexcept -> const String&;
    /// Get this unit as a short text.
    [[nodiscard]] auto toTextShort() const noexcept -> const String&;
    /// Get this unit as a lowercase, singular text
    [[nodiscard]] auto toTextLowercaseSingular() const noexcept -> const String&;
    /// Get the second factor of this unit.
    [[nodiscard]] auto secondFactor() const noexcept -> double;
    /// Get the nanosecond factor of this unit.
    [[nodiscard]] auto nanosecondsFactor() const noexcept -> int64_t;
    /// @}

public: // enumeration
    /// Get an array with all time units.
    ///
    [[nodiscard]] static auto all() noexcept -> const std::array<TimeUnit, 10>& {
        static const std::array<TimeUnit, 10> values = {
            Nanoseconds, Microseconds, Milliseconds, Seconds, Minutes, Hours, Days, Weeks, Months, Years
        };
        return values;
    }

private:
    struct ValueEntry {
        Enum unit;
        String textLong;
        String textShort;
        String textForTest;
        double secondFactor;
        int64_t nanosecondsFactor;
    };
    using ValueMap = std::array<ValueEntry, 10>;
    [[nodiscard]] static auto mapEntry(Enum value) noexcept -> const ValueEntry&;

private:
    Enum _value{Seconds};
    static ValueMap _valueMap;
};


}


template <>
struct std::hash<erbsland::conf::TimeUnit> {
    auto operator()(const erbsland::conf::TimeUnit& unit) const noexcept -> std::size_t {
        return std::hash<std::underlying_type_t<erbsland::conf::TimeUnit::Enum>>{}(
            static_cast<erbsland::conf::TimeUnit::Enum>(unit));
    }
};


template <>
struct std::formatter<erbsland::conf::TimeUnit> : std::formatter<std::string> {
    auto format(const erbsland::conf::TimeUnit timeUnit, format_context& ctx) const {
        return std::formatter<std::string>::format(timeUnit.toTextLong().toCharString(), ctx);
    }
};
