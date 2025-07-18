// Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TimeDelta.hpp"


#include "impl/SaturationMath.hpp"
#include "impl/utf8/U8Format.hpp"

#include <functional>
#include <numeric>
#include <ranges>


namespace erbsland::conf {
template<typename Operator>
auto combineTimeDeltas(const TimeDelta &a, const TimeDelta &b) -> TimeDelta {
    TimeDelta result;
    Operator op;
    for (const auto unit : TimeUnit::all()) {
        if (const auto count = op(a.count(unit), b.count(unit)); count != 0) {
            result.setCount(unit, count);
        }
    }
    return result;
}


auto TimeDelta::operator+(const TimeDelta &other) const noexcept -> TimeDelta {
    return combineTimeDeltas<std::plus<>>(*this, other);
}


auto TimeDelta::operator-(const TimeDelta &other) const noexcept -> TimeDelta {
    return combineTimeDeltas<std::minus<>>(*this, other);
}


auto TimeDelta::operator+=(const TimeDelta &other) noexcept -> TimeDelta & {
    return *this = *this + other;
}


auto TimeDelta::operator-=(const TimeDelta &other) noexcept -> TimeDelta & {
    return *this = *this - other;
}


auto operator-(const TimeDelta &other) noexcept -> TimeDelta {
    TimeDelta result;
    for (const auto unit : TimeUnit::all()) {
        if (const auto count = -(other.count(unit)); count != 0) {
            result.setCount(unit, count);
        }
    }
    return result;
}


auto TimeDelta::toSeconds() const noexcept -> double {
    auto transformed_range = TimeUnit::all() | std::views::transform([this](TimeUnit unit) {
        return static_cast<double>(count(unit)) * unit.secondFactor();
    });
    return std::accumulate(transformed_range.begin(), transformed_range.end(), 0.0);
}


auto TimeDelta::toNanoseconds() const -> int64_t {
    int64_t result = 0;
    for (const auto unit : TimeUnit::all()) {
        auto count = static_cast<int64_t>(this->count(unit));
        const auto factor = unit.nanosecondsFactor();
        if (impl::willMultiplyOverflow(count, factor)) {
            throw std::domain_error("Nanoseconds overflow: the result exceeds int64_t limits");
        }
        const auto delta = count * factor;
        if (impl::willAddOverflow(result, delta)) {
            throw std::domain_error("Nanoseconds overflow: the result exceeds int64_t limits");
        }
        result += delta;
    }
    return result;
}


auto TimeDelta::toText() const -> String {
    String result;
    for (const auto unit : TimeUnit::all()) {
        if (auto count = static_cast<int64_t>(this->count(unit)); count != 0) {
            if (!result.empty()) {
                result.append(u8", ");
            }
            result.append(impl::u8format("{}", count));
            if (unit.textShort().empty()) {
                result.append(u8" ");
                result.append(unit.textLong());
            } else {
                result.append(unit.textShort());
            }
        }
    }
    return result;
}


auto TimeDelta::toTestText() const -> String {
    if (_counts.empty()) {
        return u8"<none>";
    }
    if (_counts.size() > 1) {
        return u8"<multiple>";
    }
    auto [unit, count] = *_counts.begin();
    return impl::u8format("{},{}", count, unit.textForTest());
}


}

