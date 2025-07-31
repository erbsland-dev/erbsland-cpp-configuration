// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Value.hpp"


#include "BytesValue.hpp"
#include "Document.hpp"
#include "Section.hpp"
#include "SectionList.hpp"
#include "ValueList.hpp"
#include "ValueWithConvertibleType.hpp"
#include "ValueWithNativeType.hpp"

#include "../utf8/U8Format.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>


namespace erbsland::conf::impl {


auto Value::name() const noexcept -> Name {
    return _name;
}


auto Value::namePath() const noexcept -> NamePath {
    if (!hasParent()) { // A standalone value with no parent.
        return _name;
    }
    conf::ValueList tempValueList;
    tempValueList.reserve(10);
    tempValueList.emplace_back(shared_from_this());
    for (auto value = parent(); value != nullptr && !value->isRoot(); value = value->parent()) {
        tempValueList.emplace_back(value);
    }
    NameList nameList;
    nameList.reserve(tempValueList.size() + 1);
    for (const auto &value : std::views::reverse(tempValueList)) {
        nameList.emplace_back(value->name());
    }
    return NamePath{std::move(nameList)};
}


auto Value::hasParent() const noexcept -> bool {
    return !_parent.expired();
}


auto Value::parent() const noexcept -> conf::ValuePtr {
    return _parent.lock();
}


auto Value::hasLocation() const noexcept -> bool {
    return !_location.isUndefined();
}


auto Value::location() const noexcept -> Location {
    return _location;
}


void Value::setLocation(const Location &newLocation) noexcept {
    _location = newLocation;
}


void Value::setParent(const conf::ValuePtr &parent) {
    _parent = parent;
}


void Value::addValue(const ValuePtr &childValue) {
    throw std::logic_error("Child values are not supported for this type.");
}


void Value::throwAsTypeMismatch(const conf::Value &thisValue, ValueType expectedType) {
    throw Error(
        ErrorCategory::TypeMismatch,
        u8format(u8"A value has not the required type. Expected '{}' but got '{}'.", expectedType, thisValue.type()),
        thisValue.location(),
        thisValue.namePath()
    );
}


void Value::throwValueNotFound(const conf::Value &thisValue, const NamePathLike &namePath) {

    throwErrorWithPath(
        ErrorCategory::ValueNotFound,
        u8"A value was not found.",
        thisValue,
        namePath);
}


void Value::throwTypeMismatch(
    const conf::Value &thisValue,
    ValueType expectedType,
    ValueType actualType,
    const NamePathLike &namePath) {

    throwErrorWithPath(
        ErrorCategory::TypeMismatch,
        u8format(u8"A value has not the required type. Expected '{}' but got '{}'.", expectedType, actualType),
        thisValue,
        namePath);
}


auto Value::createInteger(Integer value) noexcept -> ValuePtr {
    return std::make_shared<IntegerValue>(value);
}


auto Value::createBoolean(bool value) noexcept -> ValuePtr {
    return std::make_shared<BooleanValue>(value);
}


auto Value::createFloat(Float value) noexcept -> ValuePtr {
    return std::make_shared<FloatValue>(value);
}


auto Value::createText(const String &value) noexcept -> ValuePtr {
    return std::make_shared<TextValue>(value);
}


auto Value::createText(String &&value) noexcept -> ValuePtr {
    return std::make_shared<TextValue>(std::move(value));
}


auto Value::createDate(const Date &value) noexcept -> ValuePtr {
    return std::make_shared<DateValue>(value);
}


auto Value::createTime(const Time &value) noexcept -> ValuePtr {
    return std::make_shared<TimeValue>(value);
}


auto Value::createDateTime(const DateTime &value) noexcept -> ValuePtr {
    return std::make_shared<DateTimeValue>(value);
}


auto Value::createBytes(const Bytes &value) noexcept -> ValuePtr {
    return std::make_shared<BytesValue>(value);
}


auto Value::createBytes(Bytes &&value) noexcept -> ValuePtr {
    return std::make_shared<BytesValue>(std::move(value));
}


auto Value::createTimeDelta(const TimeDelta &value) noexcept -> ValuePtr {
    return std::make_shared<TimeDeltaValue>(value);
}


auto Value::createRegEx(const RegEx &value) noexcept -> ValuePtr {
    return std::make_shared<RegExValue>(value);
}


auto Value::createRegEx(RegEx &&value) noexcept -> ValuePtr {
    return std::make_shared<RegExValue>(std::move(value));
}


auto Value::createValueList(std::vector<ValuePtr> &&valueList) noexcept -> ValuePtr {
    auto result = std::make_shared<ValueList>(std::move(valueList));
    result->initializeChildren();
    return result;
}


auto Value::createSectionList() noexcept -> ValuePtr {
    return std::make_shared<SectionList>();
}


auto Value::createIntermediateSection() noexcept -> ValuePtr {
    return std::make_shared<IntermediateSection>();
}


auto Value::createSectionWithNames() noexcept -> ValuePtr {
    return std::make_shared<SectionWithNames>();
}


auto Value::createSectionWithTexts() noexcept -> ValuePtr {
    return std::make_shared<SectionWithTexts>();
}


}

