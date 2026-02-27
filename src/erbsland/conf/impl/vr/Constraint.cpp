// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Constraint.hpp"


#include "ValidationContext.hpp"

#include "../value/DirectStorageAccess.hpp"
#include "../value/Value.hpp"


namespace erbsland::conf::impl {


auto Constraint::name() const -> String {
    return _name;
}


auto Constraint::type() const -> vr::ConstraintType {
    return _type;
}


auto Constraint::hasCustomError() const -> bool {
    return !_errorMessage.empty();
}


auto Constraint::customError() const -> String {
    return _errorMessage;
}


auto Constraint::isNegated() const -> bool {
    return _isNegated;
}


auto Constraint::hasLocation() const noexcept -> bool {
    return !_location.isUndefined();
}


auto Constraint::location() const noexcept -> const Location& {
    return _location;
}


void Constraint::setLocation(const Location &newLocation) noexcept {
    _location = newLocation;
}


void Constraint::validate(const ValidationContext &context) const {
    try {
        if (context.target == ValidationTarget::Value) {
            validateValue(context);
        } else {
            validateName(context);
        }
    } catch (const Error &error) {
        if (error.location().isUndefined()) {
            // Add the name path and location to the error if it is missing.
            throw error.withNamePathAndLocation(context.value->namePath(), context.value->location());
        }
        throw;
    }
}


void Constraint::validateValue(const ValidationContext &context) const {
    const auto &value = context.value;
    switch (value->type().raw()) {
    case ValueType::Undefined:
        break;
    case ValueType::Integer:
        validateInteger(context, directStorageAccess<Integer>(value));
        break;
    case ValueType::Boolean:
        validateBoolean(context, directStorageAccess<bool>(value));
        break;
    case ValueType::Float:
        validateFloat(context, directStorageAccess<Float>(value));
        break;
    case ValueType::Text:
        validateText(context, directStorageAccess<String>(value));
        break;
    case ValueType::Date:
        validateDate(context, directStorageAccess<Date>(value));
        break;
    case ValueType::Time:
        validateTime(context, directStorageAccess<Time>(value));
        break;
    case ValueType::DateTime:
        validateDateTime(context, directStorageAccess<DateTime>(value));
        break;
    case ValueType::Bytes:
        validateBytes(context, directStorageAccess<Bytes>(value));
        break;
    case ValueType::TimeDelta:
        validateTimeDelta(context, directStorageAccess<TimeDelta>(value));
        break;
    case ValueType::RegEx:
        validateRegEx(context, directStorageAccess<RegEx>(value));
        break;
    case ValueType::ValueList:
        validateValueList(context);
        break;
    case ValueType::SectionList:
        validateSectionList(context);
        break;
    case ValueType::IntermediateSection:
        validateIntermediateSection(context);
        break;
    case ValueType::SectionWithNames:
    case ValueType::Document:
        validateSectionWithNames(context);
        break;
    case ValueType::SectionWithTexts:
        validateSectionWithTexts(context);
        break;
    }
}


void Constraint::validateName(const ValidationContext &context) const {
    try {
        validateText(context, context.value->name().asText());
    } catch (const Error &error) {
        throw error.withMessagePrefix(u8"Value name validation failed: ");
    }
}

void Constraint::validateInteger(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const Integer value) const {
    // ignore by default.
}


void Constraint::validateBoolean(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const bool value) const {
    // ignore by default.
}


void Constraint::validateFloat(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const Float value) const {
    // ignore by default.
}


void Constraint::validateText(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const String &value) const {
    // ignore by default
}


void Constraint::validateDate(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const Date &value) const {
    // ignore by default
}


void Constraint::validateTime(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const Time &value) const {
    // ignore by default
}


void Constraint::validateDateTime(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const DateTime &value) const {
    // ignore by default
}


void Constraint::validateBytes(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const Bytes &value) const {
    // ignore by default
}


void Constraint::validateTimeDelta(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const TimeDelta &value) const {
    // ignore by default
}


void Constraint::validateRegEx(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const RegEx &value) const {
    // ignore by default
}


void Constraint::validateValueList(
    [[maybe_unused]] const ValidationContext &context) const {
    // ignore by default
}


void Constraint::validateSectionList(
    [[maybe_unused]] const ValidationContext &context) const {
    // ignore by default
}


void Constraint::validateIntermediateSection(
    [[maybe_unused]] const ValidationContext &context) const {
    // ignore by default
}


void Constraint::validateSectionWithNames(
    [[maybe_unused]] const ValidationContext &context) const {
    // ignore by default
}


void Constraint::validateSectionWithTexts(
    [[maybe_unused]] const ValidationContext &context) const {
    // ignore by default
}


void Constraint::setName(String name) {
    _name = std::move(name);
}


void Constraint::setType(const vr::ConstraintType type) {
    _type = type;
}


void Constraint::setErrorMessage(String errorMessage) {
    _errorMessage = std::move(errorMessage);
}


void Constraint::setNegated(bool isNegated) {
    _isNegated = isNegated;
}


auto Constraint::isFromTemplate() const -> bool {
    return _isFromTemplate;
}


void Constraint::setFromTemplate(const bool isFromTemplate) {
    _isFromTemplate = isFromTemplate;
}


#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
auto internalView(const Constraint &constraint) -> InternalViewPtr {
    return constraint.internalView();
}


auto internalView(const ConstraintPtr &constraintPtr) -> InternalViewPtr {
    if (!constraintPtr) {
        return InternalView::create();
    }
    return internalView(*constraintPtr);
}


auto Constraint::internalView() const -> InternalViewPtr {
    auto result = InternalView::create();
    result->setValue("name", _name);
    result->setValue("type", _type.toText());
    result->setValue("errorMessage", _errorMessage);
    result->setValue("isNegated", _isNegated);
    return result;
}
#endif


}

