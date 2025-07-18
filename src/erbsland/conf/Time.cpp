// Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Time.hpp"


#include "impl/utf8/U8Format.hpp"

#include <algorithm>


namespace erbsland::conf {


Time::Time(int hour, int minute, int second, int nanosecond, TimeOffset offset)
    : _nanoseconds{nanosecondsFromTime(hour, minute, second, nanosecond)}, _offset{offset} {

    if (hour < 0 || hour > 23) {
        throw std::invalid_argument("Invalid hour");
    }
    if (minute < 0 || minute > 59) {
        throw std::invalid_argument("Invalid minute");
    }
    if (second < 0 || second > 59) {
        throw std::invalid_argument("Invalid second");
    }
    if (nanosecond < 0 || nanosecond > 999'999'999) {
        throw std::invalid_argument("Invalid nanosecond");
    }
}


Time::Time(int64_t nanoseconds, TimeOffset offset)
    : _nanoseconds{nanoseconds}, _offset{offset} {

    if (nanoseconds < 0 || nanoseconds >= 24LL * 60LL * 60LL * 1'000'000'000LL) {
        throw std::invalid_argument("Invalid nanoseconds");
    }
}


auto Time::hour() const noexcept -> int {
    if (isUndefined()) {
        return 0;
    }
    return static_cast<int>(_nanoseconds.count() / 3'600'000'000'000LL);
}


auto Time::minute() const noexcept -> int {
    if (isUndefined()) {
        return 0;
    }
    return static_cast<int>(_nanoseconds.count() / 60'000'000'000LL) % 60;
}


auto Time::second() const noexcept -> int {
    if (isUndefined()) {
        return 0;
    }
    return static_cast<int>(_nanoseconds.count() / 1'000'000'000LL) % 60;
}


auto Time::secondFraction() const noexcept -> std::chrono::nanoseconds {
    if (isUndefined()) {
        return {};
    }
    return _nanoseconds % 1'000'000'000LL;
}


auto Time::offset() const noexcept -> const TimeOffset& {
    return _offset;
}


auto Time::toNanoseconds() const noexcept -> std::chrono::nanoseconds {
    if (isUndefined()) {
        return {};
    }
    return _nanoseconds;
}


auto Time::toText() const noexcept -> String {
    if (isUndefined()) {
        return {};
    }
    String result;
    result.append(impl::u8format("{:02}:{:02}:{:02}", hour(), minute(), second()));
    if (const auto nanoseconds = this->secondFraction(); nanoseconds.count() != 0) {
        const auto fractionStr = impl::u8format("{:09}", nanoseconds.count());
        result.append(u8".");
        result.append(fractionStr.substr(0, fractionStr.find_last_not_of(u8'0') + 1));
    }
    if (!_offset.isLocalTime()) {
        result.append(_offset.toText(TimeOffset::Precision::Minutes));
    }
    return result;
}


auto Time::nanosecondsFromTime(int hour, int minute, int second, int nanosecond) noexcept -> int64_t {
    return static_cast<int64_t>(nanosecond) +
        (static_cast<int64_t>(second) +
        static_cast<int64_t>(minute) * 60LL +
        static_cast<int64_t>(hour) * 3600LL) * 1'000'000'000LL;
}


}

