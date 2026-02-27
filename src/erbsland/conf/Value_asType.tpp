// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


namespace erbsland::conf {


template <typename T>
auto Value::asType() const noexcept -> T {
    static_assert(
        ValueType::from<T>().raw() != ValueType::Undefined,
        "no type support available for the specified template argument.");
    return {};
}


template <typename T>
auto Value::asTypeOrThrow() const -> T {
    static_assert(
        ValueType::from<T>().raw() != ValueType::Undefined,
        "no type support available for the specified template argument.");
    throw Error(ErrorCategory::TypeMismatch, "asTypeOrThrow() does not support the given type.");
}


template <typename T>
requires std::is_integral_v<T>
auto Value::asType() const noexcept -> T {
    return impl::saturatingCast<T, Integer>(asInteger());
}


template <typename T>
requires std::is_integral_v<T>
auto Value::asTypeOrThrow() const -> T {
    const auto value = asIntegerOrThrow();
    if (impl::willSaturatingCastOverflow<T, Integer>(asInteger())) {
        throw Error(ErrorCategory::TypeMismatch, u8"The value exceeds the expected range.", namePath(), location());
    }
    return static_cast<T>(value);
}


template <typename T>
requires std::is_floating_point_v<T>
auto Value::asType() const noexcept -> T {
    const auto value = asFloat();
    if constexpr (sizeof(T) < sizeof(Float)) {
        if (!std::isfinite(value)) {
            return static_cast<T>(value);
        }
        const auto max = static_cast<Float>(std::numeric_limits<T>::max());
        const auto lowest = static_cast<Float>(std::numeric_limits<T>::lowest());
        if (value > max) {
            return std::numeric_limits<T>::max();
        }
        if (value < lowest) {
            return std::numeric_limits<T>::lowest();
        }
    }
    return static_cast<T>(value);
}


template <typename T>
requires std::is_floating_point_v<T>
auto Value::asTypeOrThrow() const -> T {
    const auto value = asFloatOrThrow();
    if constexpr (sizeof(T) < sizeof(Float)) {
        if (!std::isfinite(value)) {
            return static_cast<T>(value);
        }
        const auto max = static_cast<Float>(std::numeric_limits<T>::max());
        const auto lowest = static_cast<Float>(std::numeric_limits<T>::lowest());
        if (value > max || value < lowest) {
            throw Error(ErrorCategory::TypeMismatch, u8"The value exceeds the expected range.", namePath(), location());
        }
    }
    return static_cast<T>(value);
}


template <>
[[nodiscard]] inline auto Value::asType<bool>() const noexcept -> bool {
    return asBoolean();
}


template <>
[[nodiscard]] inline auto Value::asType<String>() const noexcept -> String {
    return asText();
}


template <>
[[nodiscard]] inline auto Value::asType<std::u8string>() const noexcept -> std::u8string {
    return asText().raw();
}
template <>
[[nodiscard]] inline auto Value::asType<std::string>() const noexcept -> std::string {
    return asText().toCharString();
}
template <>
[[nodiscard]] inline auto Value::asType<Date>() const noexcept -> Date {
    return asDate();
}
template <>
[[nodiscard]] inline auto Value::asType<Time>() const noexcept -> Time {
    return asTime();
}
template <>
[[nodiscard]] inline auto Value::asType<DateTime>() const noexcept -> DateTime {
    return asDateTime();
}
template <>
[[nodiscard]] inline auto Value::asType<TimeDelta>() const noexcept -> TimeDelta {
    return asTimeDelta();
}
template <>
[[nodiscard]] inline auto Value::asType<Bytes>() const noexcept -> Bytes {
    return asBytes();
}
template <>
[[nodiscard]] inline auto Value::asType<RegEx>() const noexcept -> RegEx {
    return asRegEx();
}
template <>
[[nodiscard]] inline auto Value::asType<ValueList>() const noexcept -> ValueList {
    return asValueList();
}


template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<bool>() const -> bool {
    return asBooleanOrThrow();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<String>() const -> String {
    return asTextOrThrow();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<std::u8string>() const -> std::u8string {
    return asTextOrThrow().raw();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<std::string>() const -> std::string {
    return asTextOrThrow().toCharString();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<Date>() const -> Date {
    return asDateOrThrow();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<Time>() const -> Time {
    return asTimeOrThrow();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<DateTime>() const -> DateTime {
    return asDateTimeOrThrow();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<TimeDelta>() const -> TimeDelta {
    return asTimeDeltaOrThrow();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<Bytes>() const -> Bytes {
    return asBytesOrThrow();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<RegEx>() const -> RegEx {
    return asRegExOrThrow();
}
template <>
[[nodiscard]] inline auto Value::asTypeOrThrow<ValueList>() const -> ValueList {
    return asValueListOrThrow();
}


}