// Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TimeUnit.hpp"


#include <cassert>


namespace erbsland::conf {


TimeUnit::ValueMap TimeUnit::_valueMap = {
    ValueEntry{
        Nanoseconds,
        u8"Nanoseconds",
        u8"ns",
        u8"nanosecond",
        1e-9,
        1ll},
    ValueEntry{
        Microseconds,
        u8"Microseconds",
        u8"µs",
        u8"microsecond",
        1e-6,
        1'000ll},
    ValueEntry{
        Milliseconds,
        u8"Milliseconds",
        u8"ms",
        u8"millisecond",
        1e-3,
        1'000'000ll},
    ValueEntry{
        Seconds,
        u8"Seconds",
        u8"s",
        u8"second",
        1.0,
        1'000'000'000ll},
    ValueEntry{
        Minutes,
        u8"Minutes",
        u8"m",
        u8"minute",
        60.0,
        60'000'000'000ll},
    ValueEntry{Hours,
        u8"Hours",
        u8"h",
        u8"hour",
        3'600.0,
        3'600'000'000'000ll},
    ValueEntry{
        Days,
        u8"Days",
        u8"d",
        u8"day",
        86'400.0,
        86'400'000'000'000ll},
    ValueEntry{
        Weeks,
        u8"Weeks",
        u8"",
        u8"week",
        604'800.0,
        604'800'000'000'000ll},
    ValueEntry{
        Months,
        u8"Months",
        u8"",
        u8"month",
        2'628'000.0,
        2'628'000'000'000'000ll},
    ValueEntry{
        Years,
        u8"Years",
        u8"",
        u8"year",
        31'557'600.0,
        31'557'600'000'000'000ll},
};


auto TimeUnit::mapEntry(Enum value) noexcept -> const ValueEntry & {
    const auto index = static_cast<std::size_t>(value);
    assert(index < _valueMap.size());
    const auto &entry = _valueMap[index];
    assert(entry.unit == value);
    return entry;
}


auto TimeUnit::toTextLong() const noexcept -> const String& {
    return mapEntry(_value).textLong;
}


auto TimeUnit::toTextShort() const noexcept -> const String& {
    return mapEntry(_value).textShort;
}


auto TimeUnit::toTextLowercaseSingular() const noexcept -> const String & {
    return mapEntry(_value).textForTest;
}


auto TimeUnit::secondFactor() const noexcept -> double {
    return mapEntry(_value).secondFactor;
}


auto TimeUnit::nanosecondsFactor() const noexcept -> int64_t {
    return mapEntry(_value).nanosecondsFactor;
}


}

