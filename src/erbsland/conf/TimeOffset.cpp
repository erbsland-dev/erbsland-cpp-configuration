// Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TimeOffset.hpp"


#include "impl/utf8/U8Format.hpp"

#include <stdexcept>


namespace erbsland::conf {


auto TimeOffset::isLocalTime() const noexcept -> bool {
    return _seconds == LOCAL_TIME_OFFSET;
}


auto TimeOffset::isUTC() const noexcept -> bool {
    return _seconds == std::chrono::seconds{0};
}


auto TimeOffset::totalSeconds() const noexcept -> std::chrono::seconds {
    if (isLocalTime()) {
        return {};
    }
    return _seconds;
}


auto TimeOffset::isNegative() const noexcept -> bool {
    if (isLocalTime()) {
        return false;
    }
    return _seconds < std::chrono::seconds{0};
}


auto TimeOffset::seconds() const noexcept -> int {
    if (isLocalTime()) {
        return 0;
    }
    return static_cast<int>(std::abs(_seconds.count()) % 60);
}


auto TimeOffset::minutes() const noexcept -> int {
    if (isLocalTime()) {
        return 0;
    }
    return static_cast<int>((std::abs(_seconds.count()) / 60) % 60);
}


auto TimeOffset::hours() const noexcept -> int {
    if (isLocalTime()) {
        return 0;
    }
    return static_cast<int>(std::abs(_seconds.count()) / 3600);
}


auto TimeOffset::toText(Precision precision) const noexcept -> String {
    if (isLocalTime()) {
        return {};
    }
    if (isUTC()) {
        return {u8"z"};
    }
    if (precision == Precision::Automatic) {
        if (seconds() == 0) {
            if (minutes() == 0) {
                precision = Precision::Hours;
            } else {
                precision = Precision::Minutes;
            }
        } else {
            precision = Precision::Seconds;
        }
    }
    String result = isNegative() ? u8"-" : u8"+";
    result += impl::u8format("{:02}", hours());
    if (precision == Precision::Minutes || precision == Precision::Seconds) {
        result += impl::u8format(":{:02}", minutes());
        if (precision == Precision::Seconds) {
            result += impl::u8format(":{:02}", seconds());
        }
    }
    return result;
}


}

