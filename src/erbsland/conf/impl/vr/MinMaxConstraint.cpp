// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "MinMaxConstraint.hpp"


#include "ValidationContext.hpp"
#include "ValidationError.hpp"

#include <cmath>
#include <limits>


namespace erbsland::conf::impl {


namespace {
[[nodiscard]] auto toInteger(const std::size_t value) noexcept -> Integer {
    if (value > static_cast<std::size_t>(std::numeric_limits<Integer>::max())) {
        return std::numeric_limits<Integer>::max();
    }
    return static_cast<Integer>(value);
}
}


void MinMaxIntegerConstraint::validateInteger(
    [[maybe_unused]] const ValidationContext &context,
    const Integer value) const {

    if (isNotValid(value)) {
        throwValidationError(u8format(u8"The value must be {} {}", comparisonText(), _value));
    }
}


void MinMaxIntegerConstraint::validateText(
    [[maybe_unused]] const ValidationContext &context,
    const String &value) const {

    if (isNotValid(toInteger(value.characterLength()))) {
        throwValidationError(u8format(u8"The number of characters in this text must be {} {}", comparisonText(), _value));
    }
}


void MinMaxIntegerConstraint::validateBytes(
    [[maybe_unused]] const ValidationContext &context,
    const Bytes &value) const {

    if (isNotValid(toInteger(value.size()))) {
        throwValidationError(u8format(u8"The number of bytes must be {} {}", comparisonText(), _value));
    }
}


void MinMaxIntegerConstraint::validateValueList(const ValidationContext &context) const {
    std::size_t valueCount = 0;
    if (context.value->type().isList()) {
        valueCount = context.value->size();
    }
    if (isNotValid(toInteger(valueCount))) {
        throwValidationError(u8format(u8"The number of values in this list must be {} {}", comparisonText(), _value));
    }
}


void MinMaxIntegerConstraint::validateSectionList(const ValidationContext &context) const {
    if (isNotValid(toInteger(context.value->size()))) {
        throwValidationError(u8format(u8"The number of entries in this section list must be {} {}", comparisonText(), _value));
    }
}


void MinMaxIntegerConstraint::validateSectionWithNames(const ValidationContext &context) const {
    if (isNotValid(toInteger(context.value->size()))) {
        throwValidationError(u8format(u8"The number of entries in this section must be {} {}", comparisonText(), _value));
    }
}


void MinMaxIntegerConstraint::validateSectionWithTexts(const ValidationContext &context) const {
    if (isNotValid(toInteger(context.value->size()))) {
        throwValidationError(u8format(u8"The number of entries in this section must be {} {}", comparisonText(), _value));
    }
}


void MinMaxFloatConstraint::validateFloat(
    [[maybe_unused]] const ValidationContext &context,
    const Float value) const {

    if (std::isnan(value) || isNotValid(value)) {
        throwValidationError(u8format(u8"The value must be {} {}", comparisonText(), _value));
    }
}


auto MinMaxMatrixConstraint::isSecondNotValid(const Integer validatedValue) const -> bool {
    if (isNegated()) {
        return !compare(validatedValue, _second);
    }
    return compare(validatedValue, _second);
}


void MinMaxMatrixConstraint::validateValueList(const ValidationContext &context) const {
    const auto &value = context.value;
    std::size_t rowCount = 0;
    if (value->type().isList()) {
        rowCount = value->size();
    }
    if (isNotValid(toInteger(rowCount))) {
        throwValidationError(
            u8format(u8"The number of rows in this value matrix must be {} {}", comparisonText(), _value));
    }
    for (const auto &columns : *value) {
        std::size_t columnCount = 1;
        if (columns->type().isList()) {
            columnCount = columns->size();
        }
        if (isSecondNotValid(toInteger(columnCount))) {
            throwValidationError(
                u8format(u8"The number of columns in this row must be {} {}", comparisonText(), _second),
                value->namePath(),
                value->location());
        }
    }
}


void MinMaxDateConstraint::validateDate(
    [[maybe_unused]] const ValidationContext &context,
    const Date &value) const {

    if (isNotValid(value)) {
        throwValidationError(u8format(u8"The date must be {} {}", comparisonText(), _value.toText()));
    }
}


void MinMaxDateConstraint::validateDateTime(
    [[maybe_unused]] const ValidationContext &context,
    const DateTime &value) const {

    if (isNotValid(value.date())) {
        throwValidationError(u8format(u8"The date in this date-time must be {} {}", comparisonText(), _value.toText()));
    }
}


void MinMaxDateTimeConstraint::validateDate(
    [[maybe_unused]] const ValidationContext &context,
    const Date &value) const {

    if (isNotValid(DateTime{value, Time{}})) {
        throwValidationError(u8format(u8"The date must be {} {}", comparisonText(), _value.date().toText()));
    }
}


void MinMaxDateTimeConstraint::validateDateTime(
    [[maybe_unused]] const ValidationContext &context,
    const DateTime &value) const {

    if (isNotValid(value)) {
        throwValidationError(u8format(u8"The date-time must be {} {}", comparisonText(), _value.toText()));
    }
}


namespace {
template<typename tConstraint, typename tValueType>
[[nodiscard]] auto createConstraint(
    const MinMaxConstraint::MinOrMax minOrMax,
    const RulePtr &rule,
    const conf::ValuePtr &node) -> ConstraintPtr {

    if (node->type() != ValueType::from<tValueType>()) {
        throwValidationError(u8format(
                u8"The '{}' constraint for the '{}' rule must be of the type {}",
                node->name(),
                rule->type().toText(),
                ValueType::from<tValueType>().toText()));
    }
    return std::make_shared<tConstraint>(minOrMax, node->asType<tValueType>());
}
}


auto handleMinMaxConstraint(
    const MinMaxConstraint::MinOrMax minOrMax,
    const ConstraintHandlerContext &context) -> ConstraintPtr {

    const auto &node = context.node;
    const auto &rule = context.rule;
    switch (rule->type().raw()) {
    case vr::RuleType::Integer:
        return createConstraint<MinMaxIntegerConstraint, Integer>(minOrMax, rule, node);
    case vr::RuleType::Float:
        return createConstraint<MinMaxFloatConstraint, Float>(minOrMax, rule, node);
    case vr::RuleType::Text:
    case vr::RuleType::Bytes:
        return createConstraint<MinMaxIntegerConstraint, Integer>(minOrMax, rule, node);
    case vr::RuleType::Date:
        return createConstraint<MinMaxDateConstraint, Date>(minOrMax, rule, node);
    case vr::RuleType::DateTime:
        return createConstraint<MinMaxDateTimeConstraint, DateTime>(minOrMax, rule, node);
    case vr::RuleType::ValueList:
        return createConstraint<MinMaxIntegerConstraint, Integer>(minOrMax, rule, node);
    case vr::RuleType::ValueMatrix:
        if (auto intList = node->asList<int>(); intList.size() == 2) {
            return std::make_shared<MinMaxMatrixConstraint>(minOrMax, intList[0], intList[1]);
        }
        throwValidationError(u8format(
            u8"The '{}' constraint for a value matrix must be a list with two integer values",
            node->name()));
    case vr::RuleType::Section:
    case vr::RuleType::SectionList:
    case vr::RuleType::SectionWithTexts:
        return createConstraint<MinMaxIntegerConstraint, Integer>(minOrMax, rule, node);
    default:
        break;
    }
    throwValidationError(u8format(
        u8"The '{}' constraint is not supported for '{}' rules",
        node->name(),
        rule->type().toText()));
}


auto handleMinimumConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    return handleMinMaxConstraint(MinMaxConstraint::Min, context);
}


auto handleMaximumConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    return handleMinMaxConstraint(MinMaxConstraint::Max, context);
}


}
