// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


namespace erbsland::conf {


template <typename tExpectedType>
requires(
    std::is_same_v<tExpectedType, bool> ||
    (!std::is_integral_v<tExpectedType> && !std::is_floating_point_v<tExpectedType>))
auto Value::get(
    [[maybe_unused]] const NamePathLike &namePath,
    impl::value_get_default_param_t<tExpectedType> defaultValue) const noexcept -> tExpectedType {

    return defaultValue;
}


template <typename tExpectedType>
requires(std::is_integral_v<tExpectedType> && !std::is_same_v<tExpectedType, bool>)
auto Value::get(const NamePathLike &namePath, tExpectedType defaultValue) const noexcept -> tExpectedType {
    return static_cast<tExpectedType>(getInteger(namePath, static_cast<Integer>(defaultValue)));
}


template <typename tExpectedType>
requires(
    std::is_floating_point_v<tExpectedType> && !std::is_integral_v<tExpectedType> &&
    !std::is_same_v<tExpectedType, bool>)
auto Value::get(const NamePathLike &namePath, tExpectedType defaultValue) const noexcept -> tExpectedType {
    return static_cast<tExpectedType>(getFloat(namePath, static_cast<Float>(defaultValue)));
}


template<typename tDefaultString>
requires StringLike<tDefaultString>
[[nodiscard]] auto Value::getText(
    const NamePathLike &namePath,
    const tDefaultString &defaultValue) const noexcept -> String {

    if constexpr (std::is_same_v<tDefaultString, std::u8string>) {
        return getTextStdU8String(namePath, defaultValue);
    } else if constexpr (std::is_same_v<tDefaultString, std::string>) {
        return getTextStdString(namePath, defaultValue);
    } else {
        return getTextString(namePath, defaultValue);
    }
}


template <>
[[nodiscard]] inline auto Value::get<bool>(const NamePathLike &namePath, bool defaultValue) const noexcept -> bool {
    return getBoolean(namePath, defaultValue);
}
template <>
[[nodiscard]] inline auto Value::get<String>(const NamePathLike &namePath, const String &defaultValue) const noexcept
    -> String {
    return getTextString(namePath, defaultValue);
}
template <>
[[nodiscard]] inline auto
Value::get<std::u8string>(const NamePathLike &namePath, const std::u8string &defaultValue) const noexcept
    -> std::u8string {
    return getTextStdU8String(namePath, defaultValue).raw();
}
template <>
[[nodiscard]] inline auto
Value::get<std::string>(const NamePathLike &namePath, const std::string &defaultValue) const noexcept -> std::string {
    return getTextStdString(namePath, defaultValue).toCharString();
}
template <>
[[nodiscard]] inline auto Value::get<Date>(const NamePathLike &namePath, const Date &defaultValue) const noexcept
    -> Date {
    return getDate(namePath, defaultValue);
}
template <>
[[nodiscard]] inline auto Value::get<Time>(const NamePathLike &namePath, const Time &defaultValue) const noexcept
    -> Time {
    return getTime(namePath, defaultValue);
}
template <>
[[nodiscard]] inline auto
Value::get<DateTime>(const NamePathLike &namePath, const DateTime &defaultValue) const noexcept -> DateTime {
    return getDateTime(namePath, defaultValue);
}
template <>
[[nodiscard]] inline auto
Value::get<TimeDelta>(const NamePathLike &namePath, const TimeDelta &defaultValue) const noexcept -> TimeDelta {
    return getTimeDelta(namePath, defaultValue);
}
template <>
[[nodiscard]] inline auto Value::get<Bytes>(const NamePathLike &namePath, const Bytes &defaultValue) const noexcept
    -> Bytes {
    return getBytes(namePath, defaultValue);
}
template <>
[[nodiscard]] inline auto Value::get<RegEx>(const NamePathLike &namePath, const RegEx &defaultValue) const noexcept
    -> RegEx {
    return getRegEx(namePath, defaultValue);
}


template <>
[[nodiscard]] inline auto Value::getOrThrow<bool>(const NamePathLike &namePath) const -> bool {
    return getBooleanOrThrow(namePath);
}
template <>
[[nodiscard]] inline auto Value::getOrThrow<String>(const NamePathLike &namePath) const -> String {
    return getTextOrThrow(namePath);
}
template <>
[[nodiscard]] inline auto Value::getOrThrow<std::u8string>(const NamePathLike &namePath) const -> std::u8string {
    return getTextOrThrow(namePath).raw();
}
template <>
[[nodiscard]] inline auto Value::getOrThrow<std::string>(const NamePathLike &namePath) const -> std::string {
    return getTextOrThrow(namePath).toCharString();
}
template <>
[[nodiscard]] inline auto Value::getOrThrow<Date>(const NamePathLike &namePath) const -> Date {
    return getDateOrThrow(namePath);
}
template <>
[[nodiscard]] inline auto Value::getOrThrow<Time>(const NamePathLike &namePath) const -> Time {
    return getTimeOrThrow(namePath);
}
template <>
[[nodiscard]] inline auto Value::getOrThrow<DateTime>(const NamePathLike &namePath) const -> DateTime {
    return getDateTimeOrThrow(namePath);
}
template <>
[[nodiscard]] inline auto Value::getOrThrow<TimeDelta>(const NamePathLike &namePath) const -> TimeDelta {
    return getTimeDeltaOrThrow(namePath);
}
template <>
[[nodiscard]] inline auto Value::getOrThrow<Bytes>(const NamePathLike &namePath) const -> Bytes {
    return getBytesOrThrow(namePath);
}
template <>
[[nodiscard]] inline auto Value::getOrThrow<RegEx>(const NamePathLike &namePath) const -> RegEx {
    return getRegExOrThrow(namePath);
}


}