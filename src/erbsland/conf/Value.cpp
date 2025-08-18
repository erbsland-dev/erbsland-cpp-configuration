// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Value.hpp"


#include "Error.hpp"

#include "impl/TestTextHelper.hpp"
#include "impl/TokenType.hpp"
#include "impl/ValueTreeHelper.hpp"
#include "impl/value/Value.hpp"


namespace erbsland::conf {


auto Value::toTestText(TestFormat format) const noexcept -> String {
    return impl::toTestText(*this, format);
}


auto Value::toTestValueTree(const TestFormat format) const noexcept -> String {
    auto thisValue = shared_from_this();
    auto lines = impl::ValueTreeHelper(thisValue, format).createLines();
    std::size_t resultSize = 0;
    for (const auto &line : lines) {
        resultSize += line.size();
        resultSize += 1;
    }
    String result;
    result.reserve(resultSize);
    for (const auto &line : lines) {
        result.append(line);
        result.append(u8"\n");
    }
    return result;
}


auto Value::empty() const noexcept -> bool {
    return size() == 0;
}


auto Value::firstValue() const noexcept -> ValuePtr {
    if (empty()) {
        return {};
    }
    return value(0U);
}


auto Value::lastValue() const noexcept -> ValuePtr {
    if (empty()) {
        return {};
    }
    return value(size() - 1U);
}


auto Value::getInteger(const NamePathLike &namePath, const Integer defaultValue) const noexcept -> Integer {
    return impl::Value::valueGetter<Integer>(*this, namePath, defaultValue);
}


auto Value::getBoolean(const NamePathLike &namePath, bool defaultValue) const noexcept -> bool {
    return impl::Value::valueGetter<bool>(*this, namePath, defaultValue);
}


auto Value::getFloat(const NamePathLike &namePath, Float defaultValue) const noexcept -> Float {
    return impl::Value::valueGetter<Float>(*this, namePath, defaultValue);
}


auto Value::getText(const NamePathLike &namePath, const String &defaultValue) const noexcept -> String {
    return impl::Value::valueGetter<String>(*this, namePath, defaultValue);
}


auto Value::getDate(const NamePathLike &namePath, const Date &defaultValue) const noexcept -> Date {
    return impl::Value::valueGetter<Date>(*this, namePath, defaultValue);
}


auto Value::getTime(const NamePathLike &namePath, const Time &defaultValue) const noexcept -> Time {
    return impl::Value::valueGetter<Time>(*this, namePath, defaultValue);
}


auto Value::getDateTime(const NamePathLike &namePath, const DateTime &defaultValue) const noexcept -> DateTime {
    return impl::Value::valueGetter<DateTime>(*this, namePath, defaultValue);
}


auto Value::getBytes(const NamePathLike &namePath, const Bytes &defaultValue) const noexcept -> Bytes {
    return impl::Value::valueGetter<Bytes>(*this, namePath, defaultValue);
}


auto Value::getTimeDelta(const NamePathLike &namePath, const TimeDelta &defaultValue) const noexcept -> TimeDelta {
    return impl::Value::valueGetter<TimeDelta>(*this, namePath, defaultValue);
}


auto Value::getRegEx(const NamePathLike &namePath, const RegEx &defaultValue) const noexcept -> RegEx {
    return impl::Value::valueGetter<RegEx>(*this, namePath, defaultValue);
}


auto Value::getValueList(const NamePathLike &namePath) const noexcept -> ValueList {
    const auto valueAtPath = value(namePath);
    if (valueAtPath == nullptr) {
        return {};
    }
    return valueAtPath->asValueList();
}


auto Value::getIntegerOrThrow(const NamePathLike &namePath) const -> Integer {
    return impl::Value::valueGetterOrThrow<Integer, ValueType::Integer>(*this, namePath);
}


auto Value::getBooleanOrThrow(const NamePathLike &namePath) const -> bool {
    return impl::Value::valueGetterOrThrow<bool, ValueType::Boolean>(*this, namePath);
}


auto Value::getFloatOrThrow(const NamePathLike &namePath) const -> Float {
    return impl::Value::valueGetterOrThrow<Float, ValueType::Float>(*this, namePath);
}


auto Value::getTextOrThrow(const NamePathLike &namePath) const -> String {
    return impl::Value::valueGetterOrThrow<String, ValueType::Text>(*this, namePath);
}


auto Value::getDateOrThrow(const NamePathLike &namePath) const -> Date {
    return impl::Value::valueGetterOrThrow<Date, ValueType::Date>(*this, namePath);
}


auto Value::getTimeOrThrow(const NamePathLike &namePath) const -> Time {
    return impl::Value::valueGetterOrThrow<Time, ValueType::Time>(*this, namePath);
}


auto Value::getDateTimeOrThrow(const NamePathLike &namePath) const -> DateTime {
    return impl::Value::valueGetterOrThrow<DateTime, ValueType::DateTime>(*this, namePath);
}


auto Value::getBytesOrThrow(const NamePathLike &namePath) const -> Bytes {
    return impl::Value::valueGetterOrThrow<Bytes, ValueType::Bytes>(*this, namePath);
}


auto Value::getRegExOrThrow(const NamePathLike &namePath) const -> RegEx {
    return impl::Value::valueGetterOrThrow<RegEx, ValueType::RegEx>(*this, namePath);
}


auto Value::getTimeDeltaOrThrow(const NamePathLike &namePath) const -> TimeDelta {
    return impl::Value::valueGetterOrThrow<TimeDelta, ValueType::TimeDelta>(*this, namePath);
}


auto Value::getValueListOrThrow(const NamePathLike &namePath) const -> ValueList {
    return impl::Value::valueGetterOrThrow<ValueList, ValueType::ValueList>(*this, namePath);
}


auto Value::getSectionWithNames(const NamePathLike &namePath) const noexcept -> ValuePtr {
    return impl::Value::sectionGetter<ValueType::SectionWithNames>(*this, namePath);
}


auto Value::getSectionList(const NamePathLike &namePath) const noexcept -> ValuePtr {
    return impl::Value::sectionGetter<ValueType::SectionList>(*this, namePath);
}


auto Value::getSectionWithNamesOrThrow(const NamePathLike &namePath) const -> ValuePtr {
    return impl::Value::getterOrThrow<ValueType::SectionWithNames>(*this, namePath);
}


auto Value::getSectionListOrThrow(const NamePathLike &namePath) const -> ValuePtr {
    return impl::Value::getterOrThrow<ValueType::SectionList>(*this, namePath);
}


auto Value::getSectionWithTexts(const NamePathLike &namePath) const noexcept -> ValuePtr {
    return impl::Value::sectionGetter<ValueType::SectionWithTexts>(*this, namePath);
}


auto Value::getSectionWithTextsOrThrow(const NamePathLike &namePath) const -> ValuePtr {
    return impl::Value::getterOrThrow<ValueType::SectionWithTexts>(*this, namePath);
}


}

