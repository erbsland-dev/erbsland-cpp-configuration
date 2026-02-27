// Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Date.hpp"


#include "TimeUnit.hpp"

#include "impl/utf8/U8Format.hpp"
#include "impl/utilities/TimeHelper.hpp"


namespace erbsland::conf {


using namespace impl::time;


Date::Date(int year, int month, int day) : _daysSinceEpoch{daysSinceEpoch(year, month, day)} {
    if (!isValid(year, month, day)) {
        throw std::invalid_argument("Invalid date");
    }
}


auto Date::year() const noexcept -> int {
    if (isUndefined()) { return 0; }
    return extractYearAndDays(_daysSinceEpoch).first;
}


auto Date::month() const noexcept -> int {
    if (isUndefined()) { return 0; }
    const auto [year, doy] = extractYearAndDays(_daysSinceEpoch);
    return extractMonthAndDay(year, doy).first;
}


auto Date::day() const noexcept -> int {
    if (isUndefined()) { return 0; }
    const auto [year, doy] = extractYearAndDays(_daysSinceEpoch);
    return extractMonthAndDay(year, doy).second;
}


auto Date::toText() const noexcept -> String {
    if (isUndefined()) {
        return {};
    }
    const auto [year, doy] = extractYearAndDays(_daysSinceEpoch);
    const auto [month, day] = extractMonthAndDay(year, doy);
    return impl::u8format("{:04}-{:02}-{:02}", year, month, day);
}


auto Date::isValid(int year, int month, int day) noexcept -> bool {
    if (year < 1 || year > 9999 || month < 1 || month > 12 || day < 1 || day > daysInMonth(year, month)) {
        return false;
    }
    return true;
}


}

