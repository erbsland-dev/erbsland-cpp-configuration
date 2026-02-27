// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TypeTraits.hpp"

#include "../../String.hpp"
#include "../../TestFormat.hpp"
#include "../../Value.hpp"


namespace erbsland::conf::impl {


template<typename T>
[[nodiscard]] auto toTestText(const T &, TestFormat) noexcept -> String {
    static_assert(always_false_v<T>, "Not implemented for the given type.");
    return {};
}

template<typename T> requires(std::is_same_v<T, bool>)
[[nodiscard]] auto toTestText(const T &value, TestFormat) noexcept -> String {
    return value ? String{u8"true"} : String{u8"false"};
}

template<typename T> requires(!std::is_same_v<T, bool> && (std::is_integral_v<T> || std::is_floating_point_v<T>))
[[nodiscard]] auto toTestText(const T &value, TestFormat) noexcept -> String {
    return u8format(u8"{}", value);
}

template<>
[[nodiscard]] inline auto toTestText(const String &value, TestFormat) noexcept -> String {
    return u8format(u8"\"{}\"", value.toEscaped(EscapeMode::FullTestAdapter));
}

template<>
[[nodiscard]] inline auto toTestText(const Date &value, TestFormat) noexcept -> String {
    return value.toText();
}

template<>
[[nodiscard]] inline auto toTestText(const Time &value, TestFormat) noexcept -> String {
    return value.toText();
}

template<>
[[nodiscard]] inline auto toTestText(const DateTime &value, TestFormat) noexcept -> String {
    return value.toText();
}

template<>
[[nodiscard]] inline auto toTestText(const Bytes &value, TestFormat) noexcept -> String {
    return value.toHex();
}

template<>
[[nodiscard]] inline auto toTestText(const TimeUnit &value, TestFormat) noexcept -> String {
    return value.toTextLowercaseSingular();
}

template<>
[[nodiscard]] inline auto toTestText(const RegEx &value, TestFormat) noexcept -> String {
    return u8format(u8"\"{}\"", value.toText().toEscaped(EscapeMode::FullTestAdapter));
}

template<>
[[nodiscard]] inline auto toTestText(const TimeDelta &value, TestFormat format) noexcept -> String {
    if (value.empty()) {
        return u8"<none>";
    }
    if (value.hasMultipleCounts()) {
        return u8"<multiple>";
    }
    const auto unit = value.units().front();
    const auto count = value.count(unit);
    return u8format("{},{}", count, toTestText(unit, format));
}


template<>
[[nodiscard]] inline auto toTestText(const conf::Value &value, const TestFormat format) noexcept -> String {
    String valueText;
    switch (value.type().raw()) {
    case ValueType::Integer:
        valueText = toTestText(value.asInteger(), format); break;
    case ValueType::Boolean:
        valueText = toTestText(value.asBoolean(), format); break;
    case ValueType::Float:
        valueText = toTestText(value.asFloat(), format); break;
    case ValueType::Text:
        valueText = toTestText(value.asText(), format); break;
    case ValueType::Date:
        valueText = toTestText(value.asDate(), format); break;
    case ValueType::Time:
        valueText = toTestText(value.asTime(), format); break;
    case ValueType::DateTime:
        valueText = toTestText(value.asDateTime(), format); break;
    case ValueType::Bytes:
        valueText = toTestText(value.asBytes(), format); break;
    case ValueType::TimeDelta:
        valueText = toTestText(value.asTimeDelta(), format); break;
    case ValueType::RegEx:
        valueText = toTestText(value.asRegEx(), format); break;
    case ValueType::ValueList:
    case ValueType::SectionList:
    case ValueType::IntermediateSection:
    case ValueType::SectionWithNames:
    case ValueType::SectionWithTexts:
    case ValueType::Document:
        if (format.isSet(TestFormat::ShowContainerSize)) {
            valueText = u8format(u8"size={}", value.size());
        }
        break;
    case ValueType::Undefined:
        return u8"Undefined()";
    }
    return u8format(u8"{}({})", value.type(), valueText);
}


}

