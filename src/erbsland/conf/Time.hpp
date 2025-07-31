// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TestFormat.hpp"
#include "TimeOffset.hpp"

#include "impl/ComparisonHelper.hpp"

#include <chrono>
#include <cstdint>


namespace erbsland::conf {


class DateTime;


/// A time value with nanosecond precision and optional offset.
///
/// @note About the implementation of the comparison operators:
///     Comparison of two times, without date and/or mixed offsets is very limited and usually makes no sense,
///     as for a proper comparison of a point-in-time the date for the given time is required as well.
///     Therefore, as this time class is a minimal data class, for time-comparison, local-time is assumed to be UTC.
///
/// @tested `TimeTest`
///
class Time final {
    friend class DateTime;

public:
    /// Create an undefined time.
    ///
    Time() = default;

    /// Creates a new time from the given elements.
    ///
    /// @param hour The hour in the range 0-23.
    /// @param minute The minute in the range 0-59.
    /// @param second The second in the range 0-59.
    /// @param nanosecond The second fraction in the range 0-999'999'999
    /// @param offset The time offset.
    /// @throws std::invalid_argument if any of the parameters is out of the valid range.
    ///
    Time(int hour, int minute, int second, int nanosecond, TimeOffset offset);

    /// Create a time from nanoseconds.
    ///
    /// @param nanoseconds The time in nanoseconds since midnight in the range 0-5'183'999'999'999'999.
    /// @param offset The time offset.
    /// @throws std::invalid_argument in case the nanoseconds exceed 24h or is negative.
    ///
    Time(int64_t nanoseconds, TimeOffset offset);

    /// Default destructor.
    ~Time() = default;
    /// Default copy constructor.
    Time(const Time&) = default;
    /// Default copy assignment.
    auto operator=(const Time&) -> Time& = default;

public: // operators
    ERBSLAND_CONF_COMPARE_MEMBER(const Time& other, nanosecondsForComparison(), other.nanosecondsForComparison());

public: // accessors
    /// Test if this time is undefined (created with the default constructor).
    ///
    /// @return `true` if this time is undefined.
    ///
    [[nodiscard]] auto isUndefined() const noexcept -> bool {
        return _nanoseconds < std::chrono::nanoseconds{0};
    }

    /// The hour part of this time (0-23).
    ///
    [[nodiscard]] auto hour() const noexcept -> int;

    /// The minute part of this time (0-59).
    ///
    [[nodiscard]] auto minute() const noexcept -> int;

    /// The second part of this time (0-59).
    ///
    [[nodiscard]] auto second() const noexcept -> int;

    /// The second fractions in nanoseconds (0-999'999'999)
    ///
    [[nodiscard]] auto secondFraction() const noexcept -> std::chrono::nanoseconds;

    /// The offset for this time.
    ///
    [[nodiscard]] auto offset() const noexcept -> const TimeOffset&;

public: // conversion
    /// Convert this time into nanoseconds since midnight.
    ///
    [[nodiscard]] auto toNanoseconds() const noexcept -> std::chrono::nanoseconds;

    /// Convert this time into text.
    ///
    /// Uses the ISO format hh:mm:ss.zzz. Adds the minimum number of second fractions. Always adds the offset or "z"
    /// for UTC times, unless this is a local time.
    ///
    /// @return The formatted time or an empty string for undefined times.
    ///
    [[nodiscard]] auto toText() const noexcept -> String;

private:
    /// Calculate the number of nanoseconds since midnight from the given time elements.
    ///
    static auto nanosecondsFromTime(int hour, int minute, int second, int nanosecond) noexcept -> int64_t;

    /// Convert the nanoseconds for comparison.
    ///
    [[nodiscard]] auto nanosecondsForComparison() const noexcept -> std::chrono::nanoseconds {
        return _nanoseconds + std::chrono::duration_cast<std::chrono::nanoseconds>(_offset.totalSeconds());
    }

private:
    std::chrono::nanoseconds _nanoseconds{-1};
    TimeOffset _offset;
};


}


template <>
struct std::formatter<erbsland::conf::Time> : std::formatter<std::string> {
    auto format(const erbsland::conf::Time time, format_context& ctx) const {
        return std::formatter<std::string>::format(time.toText().toCharString(), ctx);
    }
};
