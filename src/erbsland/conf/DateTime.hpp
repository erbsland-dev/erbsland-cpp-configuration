// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Date.hpp"
#include "Time.hpp"


namespace erbsland::conf {


/// A date-time value, with a date, time and time-offset part.
///
/// @important As this date-time class is meant to be converted into a full features date-time instance,
/// for time-comparison, local-time is assumed to be UTC.
/// As querying the current offset for a local-time is omitted, as this would add unnecessary complexity
/// to this data class.
/// Date-times with different offsets are properly compared, with the offset applied to the compared point-in-time.
///
/// @tested `DateTimeTest`
///
class DateTime final {
public:
    /// Create an undefined date-time value.
    ///
    DateTime() = default;

    /// Create a new date-time value.
    ///
    /// @tparam FwdDate A date type that can be forwarded to the internal `Date` constructor.
    /// @tparam FwdTime  A time type that can be forwarded to the internal `Time` constructor.
    /// @param date The date component.
    /// @param time The time component.
    ///
    template <typename FwdDate, typename FwdTime>
    DateTime(FwdDate &&date, FwdTime &&time)
        : _date{std::forward<FwdDate>(date)}, _time{std::forward<FwdTime>(time)} {
    }

    /// Default destructor.
    ~DateTime() = default;
    /// Default copy constructor.
    DateTime(const DateTime&) = default;
    /// Default copy assignment.
    auto operator=(const DateTime &other) -> DateTime& = default;

public: // operators
    ERBSLAND_CONF_COMPARE_MEMBER(const DateTime &other, valueForComparison(), other.valueForComparison());

public: // accessors
    /// Test if this date/time is undefined.
    ///
    /// @return `true` if this date-time is undefined.
    ///
    [[nodiscard]] auto isUndefined() const noexcept -> bool { return _date.isUndefined(); }

    /// Access the date part.
    ///
    /// @return The date part.
    ///
    [[nodiscard]] auto date() const noexcept -> const Date& { return _date; }

    /// Access the time part.
    ///
    /// @return The time part.
    ///
    [[nodiscard]] auto time() const noexcept -> const Time& { return _time; }

public: // conversion
    /// Convert this date-time into text.
    ///
    /// Uses the ISO format yyyy-mm-dd hh:mm:ss.zzz, with these rules:
    ///
    /// - If there is a second fraction, only the minimum required digits for the fractions are displayed.
    /// - For UTC times, the suffix `z` is added.
    /// - For times with offset, the offset with hours and minutes `+02:00` is added.
    /// - Local times have no suffix.
    ///
    /// @return The formatted date-time or an empty string for undefined date-times.
    ///
    [[nodiscard]] auto toText() const noexcept -> String;

private:
    /// Create a pair for comparison.
    ///
    [[nodiscard]] auto valueForComparison() const noexcept -> std::pair<int64_t, int64_t> {
        constexpr auto nanosecondsPerDay = 86'400'000'000'000ll;
        const auto nanoseconds = _time.nanosecondsForComparison().count();
        const auto dayOffset = nanoseconds / nanosecondsPerDay; // will be -1, 0 or 1
        const auto days = _date.toDaysSinceEpoch() + dayOffset;
        return {days, nanoseconds - (nanosecondsPerDay * dayOffset)};
    }

private:
    Date _date;
    Time _time;
};


}


template <>
struct std::formatter<erbsland::conf::DateTime> : std::formatter<std::string> {
    auto format(const erbsland::conf::DateTime dateTime, format_context& ctx) const {
        return std::formatter<std::string>::format(dateTime.toText().toCharString(), ctx);
    }
};
