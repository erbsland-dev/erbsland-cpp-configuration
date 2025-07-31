// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TestFormat.hpp"
#include "TimeUnit.hpp"

#include <algorithm>
#include <map>
#include <ranges>


namespace erbsland::conf {


/// A combined time delta value.
///
/// @note Months and years are incompatible units because they do not map exactly to seconds.
/// When converting time deltas that include months and years, a month counts as 2,628,000 seconds (30.44 days)
/// and a year as 31,557,600 seconds (365.25 days). Therefore, you should carefully examine the delta before
/// converting it to seconds. If you alter a date using this time delta, it is best to add years and months first
/// before adding other units.
///
/// @tested `TimeDeltaTest`
///
class TimeDelta final {
public:
    /// The integer type used to represent time quantities.
    ///
    using Count = int64_t;

public:
    /// Create a time delta with a single unit.
    ///
    TimeDelta(TimeUnit unit, Count count) noexcept : _counts{std::pair{unit, count}} {};

    /// Default constructor.
    TimeDelta() = default;
    /// Default destructor.
    ~TimeDelta() = default;
    /// Default copy constructor.
    TimeDelta(const TimeDelta&) = default;
    /// Default copy assignment.
    auto operator=(const TimeDelta &) -> TimeDelta& = default;

public: // operators
    /// Compare two time deltas for near-equality.
    ///
    /// This comparison allows for small rounding differences in fractional seconds.
    ///
    /// @param other The other time delta to compare.
    /// @return `true` if the deltas differ by less than one nanosecond in seconds.
    ///
    auto operator==(const TimeDelta &other) const noexcept -> bool { return std::abs(toSeconds() - other.toSeconds()) < 1e-9; }

    /// Compare two time deltas for inequality.
    ///
    /// @param other The other time delta to compare.
    /// @return `true` if the time deltas differ, `false` otherwise.
    ///
    auto operator!=(const TimeDelta &other) const noexcept -> bool { return !operator==(other); }

    /// Compare two time deltas for less-than.
    ///
    /// @param other The other time delta to compare.
    /// @return `true` if this time delta is less than the other, `false` otherwise.
    ///
    auto operator<(const TimeDelta &other) const noexcept -> bool { return toSeconds() < other.toSeconds(); }

    /// Compare two time deltas for less-or-equal.
    ///
    /// @param other The other time delta to compare.
    /// @return `true` if this time delta is less than or equal to the other, `false` otherwise.
    ///
    auto operator<=(const TimeDelta &other) const noexcept -> bool { return operator<(other) || operator==(other); }

    /// Compare two time deltas for greater-than.
    ///
    /// @param other The other time delta to compare.
    /// @return `true` if this time delta is greater than the other, `false` otherwise.
    ///
    auto operator>(const TimeDelta &other) const noexcept -> bool { return toSeconds() > other.toSeconds(); }

    /// Compare two time deltas for greater-or-equal.
    ///
    /// @param other The other time delta to compare.
    /// @return `true` if this time delta is greater than or equal to the other, `false` otherwise.
    ///
    auto operator>=(const TimeDelta &other) const noexcept -> bool { return operator>(other) || operator==(other); }

    /// Add two time deltas.
    ///
    /// @param other The time delta to add.
    /// @return The sum of the two time deltas.
    ///
    auto operator+(const TimeDelta &other) const noexcept -> TimeDelta;

    /// Subtract one time delta from another.
    ///
    /// @param other The time delta to subtract.
    /// @return The difference of the two time deltas.
    ///
    auto operator-(const TimeDelta &other) const noexcept -> TimeDelta;

    /// Add a time delta to this time delta.
    ///
    /// @param other The time delta to add.
    /// @return Reference to this time delta.
    ///
    auto operator+=(const TimeDelta &other) noexcept -> TimeDelta&;

    /// Subtract a time delta from this time delta.
    ///
    /// @param other The time delta to subtract.
    /// @return Reference to this time delta.
    ///
    auto operator-=(const TimeDelta &other) noexcept -> TimeDelta&;

    /// Unary minus for time delta.
    ///
    /// @param other The time delta to negate.
    /// @return The negated time delta.
    ///
    friend auto operator-(const TimeDelta &other) noexcept -> TimeDelta;

public: // accessors
    /// Test if this time delta is zero.
    ///
    [[nodiscard]] auto isZero() const noexcept -> bool {
        return _counts.empty() || std::ranges::all_of(_counts, [](const auto &pair) {
            return pair.second == 0;
        });
    }

    /// Test if this time delta is empty.
    ///
    /// This is the case if the time delta is default constructed and contains no counts.
    ///
    [[nodiscard]] auto empty() const noexcept -> bool { return _counts.empty(); }

    /// Test if this time delta combines multiple counts.
    ///
    [[nodiscard]] auto hasMultipleCounts() const noexcept -> bool { return _counts.size() > 1; }

    /// Get the count for a specific time unit.
    ///
    /// @tparam tUnit The time unit enum value.
    /// @return The count for the specified unit, or zero if not set.
    ///
    template<TimeUnit::Enum tUnit>
    [[nodiscard]] auto count() const noexcept -> Count {
        return _counts.contains(tUnit) ? _counts.at(tUnit) : 0;
    }

    /// Get the count for a specific time unit.
    ///
    /// @param unit The time unit.
    /// @return The count for the specified unit, or zero if not set.
    ///
    [[nodiscard]] auto count(TimeUnit unit) const noexcept -> Count {
        return _counts.contains(unit) ? _counts.at(unit) : 0;
    }

    /// Set the count for a specific time unit.
    ///
    /// @tparam tUnit The time unit enum value.
    /// @param count The count to set for the unit.
    ///
    template<TimeUnit::Enum tUnit>
    void setCount(Count count) noexcept {
        _counts[tUnit] = count;
    }

    /// Set the count for a specific time unit.
    ///
    /// @param unit The time unit.
    /// @param count The count to set for the unit.
    ///
    void setCount(TimeUnit unit, Count count) noexcept {
        _counts[unit] = count;
    }

    /// Get all times units that are defined for this delta.
    ///
    [[nodiscard]] auto units() const noexcept -> std::vector<TimeUnit> {
        std::vector<TimeUnit> result;
        result.reserve(_counts.size());
        for (const auto &unit : _counts | std::views::keys) {
            result.push_back(unit);
        }
        return result;
    }

    /// Get the nanosecond component of this time delta.
    ///
    /// @return The nanoseconds count.
    ///
    [[nodiscard]] auto nanoseconds() const noexcept -> Count { return count<TimeUnit::Nanoseconds>(); }

    /// Set the nanosecond component of this time delta.
    ///
    /// @param nanoseconds The nanoseconds count to set.
    ///
    void setNanoseconds(Count nanoseconds) noexcept { setCount<TimeUnit::Nanoseconds>(nanoseconds); }

    /// Get the microsecond component of this time delta.
    ///
    /// @return The microseconds count.
    ///
    [[nodiscard]] auto microseconds() const noexcept -> Count { return count<TimeUnit::Microseconds>(); }

    /// Set the microsecond component of this time delta.
    ///
    /// @param microseconds The microseconds count to set.
    ///
    void setMicroseconds(Count microseconds) noexcept { setCount<TimeUnit::Microseconds>(microseconds); }

    /// Get the millisecond component of this time delta.
    ///
    /// @return The milliseconds count.
    ///
    [[nodiscard]] auto milliseconds() const noexcept -> Count { return count<TimeUnit::Milliseconds>(); }

    /// Set the millisecond component of this time delta.
    ///
    /// @param milliseconds The milliseconds count to set.
    ///
    void setMilliseconds(Count milliseconds) noexcept { setCount<TimeUnit::Milliseconds>(milliseconds); }

    /// Get the second component of this time delta.
    ///
    /// @return The seconds count.
    ///
    [[nodiscard]] auto seconds() const noexcept -> Count { return count<TimeUnit::Seconds>(); }

    /// Set the second component of this time delta.
    ///
    /// @param seconds The seconds count to set.
    ///
    void setSeconds(Count seconds) noexcept { setCount<TimeUnit::Seconds>(seconds); }

    /// Get the minute component of this time delta.
    ///
    /// @return The minutes count.
    ///
    [[nodiscard]] auto minutes() const noexcept -> Count { return count<TimeUnit::Minutes>(); }

    /// Set the minute component of this time delta.
    ///
    /// @param minutes The minutes count to set.
    ///
    void setMinutes(Count minutes) noexcept { setCount<TimeUnit::Minutes>(minutes); }

    /// Get the hour component of this time delta.
    ///
    /// @return The hours count.
    ///
    [[nodiscard]] auto hours() const noexcept -> Count { return count<TimeUnit::Hours>(); }

    /// Set the hour component of this time delta.
    ///
    /// @param hours The hours count to set.
    ///
    void setHours(Count hours) noexcept { setCount<TimeUnit::Hours>(hours); }

    /// Get the day component of this time delta.
    ///
    /// @return The days count.
    ///
    [[nodiscard]] auto days() const noexcept -> Count { return count<TimeUnit::Days>(); }

    /// Set the day component of this time delta.
    ///
    /// @param days The days count to set.
    ///
    void setDays(Count days) noexcept { setCount<TimeUnit::Days>(days); }

    /// Get the week component of this time delta.
    ///
    /// @return The weeks count.
    ///
    [[nodiscard]] auto weeks() const noexcept -> Count { return count<TimeUnit::Weeks>(); }

    /// Set the week component of this time delta.
    ///
    /// @param weeks The weeks count to set.
    ///
    void setWeeks(Count weeks) noexcept { setCount<TimeUnit::Weeks>(weeks); }

    /// Get the month component of this time delta.
    ///
    /// @return The months count.
    ///
    [[nodiscard]] auto months() const noexcept -> Count { return count<TimeUnit::Months>(); }

    /// Set the month component of this time delta.
    ///
    /// @param months The months count to set.
    ///
    void setMonths(Count months) noexcept { setCount<TimeUnit::Months>(months); }

    /// Get the year component of this time delta.
    ///
    /// @return The years count.
    ///
    [[nodiscard]] auto years() const noexcept -> Count { return count<TimeUnit::Years>(); }

    /// Set the year component of this time delta.
    ///
    /// @param years The years count to set.
    ///
    void setYears(Count years) noexcept { setCount<TimeUnit::Years>(years); }

public: // conversion
    /// Test if this time delta is based on seconds.
    ///
    /// This is true, if there are no months and years in the time-delta. Only if this function returns `true`,
    /// the method `toSeconds()` will return a precise conversion. If this method is `false`, the result of
    /// `toSeconds()` is an approximation.
    ///
    /// @return `true` if this time delta can be precisely converted into seconds.
    ///
    [[nodiscard]] auto isSecondBased() const noexcept -> bool { return months() == 0 and years() == 0; }

    /// Converts this time delta into seconds.
    ///
    /// This function may lose precision in the fractional seconds when very large values for days or weeks are used.
    ///
    /// @return The time delta in seconds, with fractions.
    ///
    [[nodiscard]] auto toSeconds() const noexcept -> double;

    /// Tries to convert this time-delta to nano-seconds.
    ///
    /// @return The number of nanoseconds.
    /// @throws std::domain_error If the resulting nanoseconds value would exceed the 64-bit integer.
    ///
    [[nodiscard]] auto toNanoseconds() const -> int64_t;

    /// Convert this time delta into a text representation
    ///
    [[nodiscard]] auto toText() const -> String;

private:
    std::map<TimeUnit, Count> _counts;
};


}


template <>
struct std::formatter<erbsland::conf::TimeDelta> : std::formatter<std::string> {
    auto format(const erbsland::conf::TimeDelta &timeDelta, format_context& ctx) const {
        return std::formatter<std::string>::format(timeDelta.toText().toCharString(), ctx);
    }
};
