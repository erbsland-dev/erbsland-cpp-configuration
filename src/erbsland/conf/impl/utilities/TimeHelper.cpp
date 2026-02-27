// Copyright (c) 2024-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TimeHelper.hpp"


#include <array>
#include <tuple>


namespace erbsland::conf::impl::time {


auto isLeapYear(int year) noexcept -> bool {
    if (year < 0 || year > 9999) {
        return false;
    }
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}


auto daysInMonth(int year, int month) noexcept -> int {
    if (year < 0 || year > 9999) {
        return 0;
    }
    if (month < 1 || month > 12) {
        return 0;
    }
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    const static std::array<int, 12> months = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return months[static_cast<std::size_t>(month - 1)];
}


namespace {
constexpr auto daysPerCycle4{1'461ll}; // Each 4-year cycle has 36'524 days.
constexpr auto daysPerCycle100{36'524ll}; // Each 100-year cycle has 36'524 days. (daysPerCycle4 * 25 - 1)
constexpr auto daysPerCycle400{146'097ll}; /// Each 400-year cycle has 146'097 days. (daysPerCycle100 * 4 + 1)
constexpr auto maximumDaysSinceEpoch{3'652'425ll}; ///< Days for 10'000 years since epoch.
}


auto daysSinceEpoch(int year) noexcept -> int64_t {
    if (year < 0 || year > 9999) {
        return 0;
    }

    int64_t days = {};
    auto years = static_cast<int64_t>(year);
    const auto cycles400 = years / 400ll; years %= 400ll;
    days += daysPerCycle400 * cycles400;
    if (years > 0) {
        days += 1ll; // Add the leap year
    }
    const auto cycles100 = years / 100ll; years %= 100ll;
    days += daysPerCycle100 * cycles100;
    if (years > 0) {
        days -= 1ll; // Remove the leap year.
    }
    const auto cycles4 = years / 4ll; years %= 4ll;
    days += daysPerCycle4 * cycles4;
    if (years > 0) {
        days += 1ll; // Add the leap year
    }
    days += years * 365ll;
    return days;
}


namespace {
constexpr std::array<int, 13> doyn{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
constexpr std::array<int, 13> doyl{0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};
}


auto firstDayOfYearAndMonth(int year, int month) noexcept -> int {
    if (year < 0 || year > 9999 || month < 1 || month > 12) { return 0; }
    const auto &doy = isLeapYear(year) ? doyl : doyn;
    return doy[static_cast<std::size_t>(month - 1)];
}


auto daysSinceEpoch(int year, int month, int day) noexcept -> int64_t {
    if (year < 0 || year > 9999 || month < 1 || month > 12 || day < 1 || day > 31 ) { return 0; }
    return daysSinceEpoch(year) + firstDayOfYearAndMonth(year, month) + (day - 1);
}


auto extractYearAndDays(int64_t daysSinceEpoch) noexcept -> std::pair<int, int> {
    if (daysSinceEpoch <= 0) {
        return std::make_pair(0, 0); // clamp low
    }
    if (daysSinceEpoch >= maximumDaysSinceEpoch) {
        return std::make_pair(9999, 364); // clamp high
    }
    // The 400-year cycle is the only one with fixed size, and it always starts with a leap year.
    const auto years400 = daysSinceEpoch / daysPerCycle400; daysSinceEpoch %= daysPerCycle400;
    if (daysSinceEpoch < 366) { // We did hit the leap year.
        const auto years = static_cast<int>(years400 * 400ll);
        return std::make_pair(years, static_cast<int>(daysSinceEpoch));
    }
    daysSinceEpoch -= 1; // Remove the day from the leap year of the 400-year cycle.
    const auto years100 = daysSinceEpoch / daysPerCycle100; daysSinceEpoch %= daysPerCycle100;
    if (daysSinceEpoch < 365) { // We hit the exceptional non-leap year of the 100-year cycle.
        auto years = static_cast<int>(years400 * 400ll + years100 * 100ll);
        return std::make_pair(years, static_cast<int>(daysSinceEpoch));
    }
    daysSinceEpoch += 1; // Add the missing day from the leap year, to create block with uniform 4-year cycles.
    // Now we follow the regular 4-year cycle (leap, normal, normal, normal)
    const auto years4 = daysSinceEpoch / daysPerCycle4; daysSinceEpoch %= daysPerCycle4;
    auto years = years400 * 400ll + years100 * 100ll + years4 * 4ll;
    if (daysSinceEpoch >= 366) { // Not the initial leap year?
        daysSinceEpoch -= 1; // Remove the day from the leap year of the 4-year cycle.
        years += daysSinceEpoch / 365ll; daysSinceEpoch %= 365ll;
    }
    if (years > 9999) {
        return std::make_pair(9999, 364);
    }
    return std::make_pair(static_cast<int>(years), static_cast<int>(daysSinceEpoch));
}


auto extractMonthAndDay(int year, int dayOfYear) noexcept -> std::pair<int, int> {
    if (year < 0 || year > 9999) { return std::make_pair(0, 0); }
    const auto &doy = isLeapYear(year) ? doyl : doyn;
    if (dayOfYear < 0 || dayOfYear >= doy[12]) { return std::make_pair(0, 0); }
    for (std::size_t i = 1; i < doy.size(); ++i) {
        if (dayOfYear < doy[i]) {
            return std::make_pair(static_cast<int>(i), dayOfYear - doy[i - 1] + 1);
        }
    }
    return std::make_pair(0, 0); // coverage: This code is never reached.
}


}

