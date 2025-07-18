// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TimeUnit.hpp"

#include <map>


namespace erbsland::conf {


/// A combined time delta value.
///
/// @note Months and years are incompatible units as they don't map exactly to seconds. When converting time deltas into
/// seconds, that contain months and years, a month counts as 2'628'000 seconds (30.44 days) and a year
/// as 31'557'600 seconds (365.25 days). Therefore, you must be carefully examine the delta, before converting it
/// into seconds. If you alter a date, using this time delta, you better algorithmically add the years and months
/// first, before adding seconds.
/// @tested `TimeDeltaTest`
///
class TimeDelta final {
public:
    using Count = int64_t;

public:
    /// Create a time delta with a single unit.
    ///
    TimeDelta(TimeUnit unit, Count count) noexcept : _counts{std::pair{unit, count}} {};

    // defaults
    TimeDelta() = default;
    ~TimeDelta() = default;
    TimeDelta(const TimeDelta&) = default;
    auto operator=(const TimeDelta &) -> TimeDelta& = default;

public: // operators
    auto operator==(const TimeDelta &other) const noexcept -> bool { return std::abs(toSeconds() - other.toSeconds()) < 1e-9; }
    auto operator!=(const TimeDelta &other) const noexcept -> bool { return !operator==(other); }
    auto operator<(const TimeDelta &other) const noexcept -> bool { return toSeconds() < other.toSeconds(); }
    auto operator<=(const TimeDelta &other) const noexcept -> bool { return operator<(other) || operator==(other); }
    auto operator>(const TimeDelta &other) const noexcept -> bool { return toSeconds() > other.toSeconds(); }
    auto operator>=(const TimeDelta &other) const noexcept -> bool { return operator>(other) || operator==(other); }
    auto operator+(const TimeDelta &other) const noexcept -> TimeDelta;
    auto operator-(const TimeDelta &other) const noexcept -> TimeDelta;
    auto operator+=(const TimeDelta &other) noexcept -> TimeDelta&;
    auto operator-=(const TimeDelta &other) noexcept -> TimeDelta&;
    friend auto operator-(const TimeDelta &other) noexcept -> TimeDelta;

public: // accessors
    template<TimeUnit::Enum tUnit>
    [[nodiscard]] auto count() const noexcept -> Count {
        return _counts.contains(tUnit) ? _counts.at(tUnit) : 0;
    }
    [[nodiscard]] auto count(TimeUnit unit) const noexcept -> Count {
        return _counts.contains(unit) ? _counts.at(unit) : 0;
    }
    template<TimeUnit::Enum tUnit>
    void setCount(Count count) noexcept {
        _counts[tUnit] = count;
    }
    void setCount(TimeUnit unit, Count count) noexcept {
        _counts[unit] = count;
    }
    [[nodiscard]] auto nanoseconds() const noexcept -> Count { return count<TimeUnit::Nanoseconds>(); }
    void setNanoseconds(Count nanoseconds) noexcept { setCount<TimeUnit::Nanoseconds>(nanoseconds); }
    [[nodiscard]] auto microseconds() const noexcept -> Count { return count<TimeUnit::Microseconds>(); }
    void setMicroseconds(Count microseconds) noexcept { setCount<TimeUnit::Microseconds>(microseconds); }
    [[nodiscard]] auto milliseconds() const noexcept -> Count { return count<TimeUnit::Milliseconds>(); }
    void setMilliseconds(Count milliseconds) noexcept { setCount<TimeUnit::Milliseconds>(milliseconds); }
    [[nodiscard]] auto seconds() const noexcept -> Count { return count<TimeUnit::Seconds>(); }
    void setSeconds(Count seconds) noexcept { setCount<TimeUnit::Seconds>(seconds); }
    [[nodiscard]] auto minutes() const noexcept -> Count { return count<TimeUnit::Minutes>(); }
    void setMinutes(Count minutes) noexcept { setCount<TimeUnit::Minutes>(minutes); }
    [[nodiscard]] auto hours() const noexcept -> Count { return count<TimeUnit::Hours>(); }
    void setHours(Count hours) noexcept { setCount<TimeUnit::Hours>(hours); }
    [[nodiscard]] auto days() const noexcept -> Count { return count<TimeUnit::Days>(); }
    void setDays(Count days) noexcept { setCount<TimeUnit::Days>(days); }
    [[nodiscard]] auto weeks() const noexcept -> Count { return count<TimeUnit::Weeks>(); }
    void setWeeks(Count weeks) noexcept { setCount<TimeUnit::Weeks>(weeks); }
    [[nodiscard]] auto months() const noexcept -> Count { return count<TimeUnit::Months>(); }
    void setMonths(Count months) noexcept { setCount<TimeUnit::Months>(months); }
    [[nodiscard]] auto years() const noexcept -> Count { return count<TimeUnit::Years>(); }
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
    /// This function may lose precision in the second fraction, when very large numbers for days or weeks are used.
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

    /// Convert this time delta into a representation for the test adapter.
    ///
    /// If the number of counts is not equal one, it returns either `<none>` or `<multiple>` to make sure a
    /// test will fail that expected exactly one count.
    ///
    [[nodiscard]] auto toTestText() const -> String;

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

