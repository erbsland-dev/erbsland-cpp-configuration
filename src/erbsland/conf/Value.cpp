// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Value.hpp"


#include "Error.hpp"

#include "impl/TokenType.hpp"


namespace erbsland::conf {


auto Value::empty() const noexcept -> bool {
    return size() == 0;
}


auto Value::firstValue() const noexcept -> ValuePtr {
    if (empty()) {
        return {};
    }
    return value(0);
}


auto Value::lastValue() const noexcept -> ValuePtr {
    if (empty()) {
        return {};
    }
    return value(size() - 1);
}


auto Value::valueOrThrow(const std::size_t index) const -> ValuePtr {
    auto result = value(index);
    if (result == nullptr) {
        auto path = this->namePath();
        path.append(Name::createIndex(index));
        throw Error(ErrorCategory::ValueNotFound, u8"Value not found", path);
    }
    return result;
}

auto Value::valueOrThrow(const Name &name) const -> ValuePtr {
    auto result = value(name);
    if (result == nullptr) {
        auto path = this->namePath();
        path.append(name);
        throw Error(ErrorCategory::ValueNotFound, u8"Value not found", path);
    }
    return result;
}

auto Value::valueOrThrow(const NamePath &namePath) const -> ValuePtr {
    auto result = value(namePath);
    if (result == nullptr) {
        auto path = this->namePath();
        path.append(namePath);
        throw Error(ErrorCategory::ValueNotFound, u8"Value not found", path);
    }
    return result;
}


namespace {
template<typename T>
[[nodiscard]] auto valueGetter(const Value &thisValue, const NamePath &namePath, const T &defaultValue) noexcept -> T {
    const auto valuePtr = thisValue.value(namePath);
    if (valuePtr == nullptr) {
        return defaultValue;
    }
    return valuePtr->convertTo<T>();
}
}


auto Value::getInteger(const NamePath &namePath, const Integer defaultValue) const noexcept -> Integer {
    return valueGetter(*this, namePath, defaultValue);
}


auto Value::getBoolean(const NamePath &namePath, bool defaultValue) const noexcept -> bool {
    return valueGetter(*this, namePath, defaultValue);
}


auto Value::getFloat(const NamePath &namePath, Float defaultValue) const noexcept -> Float {
    return valueGetter(*this, namePath, defaultValue);
}


auto Value::getText(const NamePath &namePath, const String &defaultValue) const noexcept -> String {
    return valueGetter(*this, namePath, defaultValue);
}


auto Value::getDate(const NamePath &namePath, const Date &defaultValue) const noexcept -> Date {
    return valueGetter(*this, namePath, defaultValue);
}


auto Value::getTime(const NamePath &namePath, const Time &defaultValue) const noexcept -> Time {
    return valueGetter(*this, namePath, defaultValue);
}


auto Value::getDateTime(const NamePath &namePath, const DateTime &defaultValue) const noexcept -> DateTime {
    return valueGetter(*this, namePath, defaultValue);
}


auto Value::getBytes(const NamePath &namePath, const Bytes &defaultValue) const noexcept -> Bytes {
    return valueGetter(*this, namePath, defaultValue);
}


auto Value::getTimeDelta(const NamePath &namePath, const TimeDelta &defaultValue) const noexcept -> TimeDelta {
    return valueGetter(*this, namePath, defaultValue);
}


auto Value::getList(const NamePath &namePath) const noexcept -> ValueList {
    return valueGetter(*this, namePath, ValueList{});
}


auto Value::getRegEx(const NamePath &namePath, const String &defaultValue) const noexcept -> String {
    // Explicit, because there is no `convertTo` overload.
    const auto valuePtr = value(namePath);
    if (valuePtr == nullptr) {
        return defaultValue;
    }
    return valuePtr->toRegEx();
}


namespace {
template<typename ReturnType, ValueType::Enum tValueType>
[[nodiscard]] auto valueGetterOrThrow(const Value &thisValue, const NamePath &namePath) -> ReturnType {
    const auto valuePtr = thisValue.value(namePath);
    if (valuePtr == nullptr) {
        auto path = thisValue.namePath();
        path.append(namePath);
        throw Error(ErrorCategory::ValueNotFound, u8"Value not found.", path);
    }
    if (valuePtr->type() != tValueType) {
        auto path = thisValue.namePath();
        path.append(namePath);
        throw Error(ErrorCategory::WrongType, u8"Value has not the expected type.", path);
    }
    return valuePtr->convertTo<ReturnType>();
}
}


auto Value::getIntegerOrThrow(const NamePath &namePath) const -> Integer {
    return valueGetterOrThrow<Integer, ValueType::Integer>(*this, namePath);
}


auto Value::getBooleanOrThrow(const NamePath &namePath) const -> bool {
    return valueGetterOrThrow<bool, ValueType::Boolean>(*this, namePath);
}


auto Value::getFloatOrThrow(const NamePath &namePath) const -> Float {
    return valueGetterOrThrow<Float, ValueType::Float>(*this, namePath);
}


auto Value::getTextOrThrow(const NamePath &namePath) const -> String {
    return valueGetterOrThrow<String, ValueType::Text>(*this, namePath);
}


auto Value::getDateOrThrow(const NamePath &namePath) const -> Date {
    return valueGetterOrThrow<Date, ValueType::Date>(*this, namePath);
}


auto Value::getTimeOrThrow(const NamePath &namePath) const -> Time {
    return valueGetterOrThrow<Time, ValueType::Time>(*this, namePath);
}


auto Value::getDateTimeOrThrow(const NamePath &namePath) const -> DateTime {
    return valueGetterOrThrow<DateTime, ValueType::DateTime>(*this, namePath);
}


auto Value::getBytesOrThrow(const NamePath &namePath) const -> Bytes {
    return valueGetterOrThrow<Bytes, ValueType::Bytes>(*this, namePath);
}


auto Value::getTimeDeltaOrThrow(const NamePath &namePath) const -> TimeDelta {
    return valueGetterOrThrow<TimeDelta, ValueType::TimeDelta>(*this, namePath);
}


auto Value::getListOrThrow(const NamePath &namePath) const -> ValueList {
    return valueGetterOrThrow<ValueList, ValueType::ValueList>(*this, namePath);
}


auto Value::getRegExOrThrow(const NamePath &namePath) const -> String {
    const auto valuePtr = value(namePath);
    if (valuePtr == nullptr) {
        auto path = this->namePath();
        path.append(namePath);
        throw Error(ErrorCategory::ValueNotFound, u8"Value not found.", path);
    }
    if (valuePtr->type() != ValueType::RegEx) {
        auto path = this->namePath();
        path.append(namePath);
        throw Error(ErrorCategory::WrongType, u8"Value has not the expected type.", path);
    }
    return valuePtr->toRegEx();
}


}

