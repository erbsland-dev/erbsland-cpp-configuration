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
#include "../vr/Rule.hpp"

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
    conf::ConstValueList tempValueList;
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


auto Value::size() const noexcept -> std::size_t {
    return 0;
}


auto Value::hasValue(const NamePathLike &) const noexcept -> bool {
    return false;
}


auto Value::value(const NamePathLike &) const noexcept -> conf::ValuePtr {
    return {};
}


auto Value::valueOrThrow(const NamePathLike &namePath) const -> conf::ValuePtr {
    throwValueNotFound(*this, namePath);
}


auto Value::begin() const noexcept -> ValueIterator {
    return {};
}


auto Value::end() const noexcept -> ValueIterator {
    return {};
}


auto Value::asInteger() const noexcept -> int64_t {
    return 0LL;
}


auto Value::asBoolean() const noexcept -> bool {
    return false;
}


auto Value::asFloat() const noexcept -> double {
    return 0.0;
}


auto Value::asText() const noexcept -> String {
    return {};
}


auto Value::asDate() const noexcept -> Date {
    return {};
}


auto Value::asTime() const noexcept -> Time {
    return {};
}


auto Value::asDateTime() const noexcept -> DateTime {
    return {};
}


auto Value::asBytes() const noexcept -> Bytes {
    return {};
}


auto Value::asTimeDelta() const noexcept -> TimeDelta {
    return {};
}


auto Value::asRegEx() const noexcept -> RegEx {
    return {};
}


auto Value::asValueList() const noexcept -> conf::ValueList {
    return {};
}


auto Value::asIntegerOrThrow() const -> int64_t {
    throwAsTypeMismatch(*this, ValueType::Integer);
}


auto Value::asBooleanOrThrow() const -> bool {
    throwAsTypeMismatch(*this, ValueType::Boolean);
}


auto Value::asFloatOrThrow() const -> double {
    throwAsTypeMismatch(*this, ValueType::Float);
}


auto Value::asTextOrThrow() const -> String {
    throwAsTypeMismatch(*this, ValueType::Text);
}


auto Value::asDateOrThrow() const -> Date {
    throwAsTypeMismatch(*this, ValueType::Date);
}


auto Value::asTimeOrThrow() const -> Time {
    throwAsTypeMismatch(*this, ValueType::Time);
}


auto Value::asDateTimeOrThrow() const -> DateTime {
    throwAsTypeMismatch(*this, ValueType::DateTime);
}


auto Value::asBytesOrThrow() const -> Bytes {
    throwAsTypeMismatch(*this, ValueType::Bytes);
}


auto Value::asTimeDeltaOrThrow() const -> TimeDelta {
    throwAsTypeMismatch(*this, ValueType::TimeDelta);
}


auto Value::asRegExOrThrow() const -> RegEx {
    throwAsTypeMismatch(*this, ValueType::RegEx);
}


auto Value::asValueListOrThrow() const -> conf::ValueList {
    throwAsTypeMismatch(*this, ValueType::ValueList);
}


auto Value::toTextRepresentation() const noexcept -> String {
    return {};
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


auto Value::wasValidated() const noexcept -> bool {
    return _rule != nullptr;
}


auto Value::validationRule() const noexcept -> vr::RulePtr {
    return _rule;
}


auto Value::isDefaultValue() const noexcept -> bool {
    return _isDefaultValue;
}


void Value::setParent(const conf::ValuePtr &parent) {
    _parent = parent;
}


void Value::addValue(const ValuePtr &) {
    throw std::logic_error("Child values are not supported for this type.");
}


auto Value::childrenImpl() const noexcept -> const std::vector<ValuePtr> & {
    static std::vector<ValuePtr> empty;
    return empty;
}


auto Value::valueImpl([[maybe_unused]] const Name &name) const noexcept -> ValuePtr {
    return nullptr;
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

