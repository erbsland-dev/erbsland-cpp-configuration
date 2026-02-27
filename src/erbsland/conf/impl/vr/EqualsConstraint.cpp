// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "EqualsConstraint.hpp"


#include "ValidationError.hpp"

#include "../value/Value.hpp"


namespace erbsland::conf::impl {


EqualsIntegerConstraint::EqualsIntegerConstraint(const Integer value) : EqualsConstraint(value) {
}


void EqualsIntegerConstraint::validateInteger(const ValidationContext &context, const Integer value) const {
    if (isNotValid(value, context)) {
        throwValidationError(u8format(u8"The value {} {}", comparisonText(), _value));
    }
}


void EqualsIntegerConstraint::validateText(const ValidationContext &context, const String &value) const {
    if (isNotValid(static_cast<Integer>(value.characterLength()), context)) {
        throwValidationError(u8format(u8"The number of characters in this text {} {}", comparisonText(), _value));
    }
}


void EqualsIntegerConstraint::validateBytes(const ValidationContext &context, const Bytes &value) const {
    if (isNotValid(static_cast<Integer>(value.size()), context)) {
        throwValidationError(u8format(u8"The number of bytes {} {}", comparisonText(), _value));
    }
}


void EqualsIntegerConstraint::validateValueList(const ValidationContext &context) const {
    if (isNotValid(static_cast<Integer>(context.value->asValueList().size()), context)) {
        throwValidationError(u8format(u8"The number of values in this list {} {}", comparisonText(), _value));
    }
}


void EqualsIntegerConstraint::validateSectionWithNames(const ValidationContext &context) const {
    if (isNotValid(static_cast<Integer>(context.value->size()), context)) {
        throwValidationError(u8format(u8"The number of entries in this section {} {}", comparisonText(), _value));
    }
}


void EqualsIntegerConstraint::validateSectionWithTexts(const ValidationContext &context) const {
    if (isNotValid(static_cast<Integer>(context.value->size()), context)) {
        throwValidationError(u8format(u8"The number of entries in this section {} {}", comparisonText(), _value));
    }
}


void EqualsIntegerConstraint::validateSectionList(const ValidationContext &context) const {
    if (isNotValid(static_cast<Integer>(context.value->size()), context)) {
        throwValidationError(u8format(u8"The number of entries in this section list {} {}", comparisonText(), _value));
    }
}


EqualsBooleanConstraint::EqualsBooleanConstraint(const bool value) : EqualsConstraint(value) {
}


void EqualsBooleanConstraint::validateBoolean(const ValidationContext &context, const bool value) const {
    if (isNotValid(value, context)) {
        const auto expectedValue = isNegated() ? !_value : _value;
        throwValidationError(u8format(u8"The value must be {}", expectedValue ? String{u8"true"} : String{u8"false"}));
    }
}


EqualsFloatConstraint::EqualsFloatConstraint(const Float value) : EqualsConstraint(value) {
}


void EqualsFloatConstraint::validateFloat(const ValidationContext &context, const Float value) const {
    if (isNotValid(value, context)) {
        throwValidationError(u8format(u8"The value {} {:.6} (within platform tolerance)", comparisonText(), _value));
    }
}


void EqualsTextConstraint::validateText(const ValidationContext &context, const String &value) const {
    if (isNotValid(value, context)) {
        throwValidationError(u8format(
            u8"The text {} \"{}\" ({})",
            comparisonText(),
            _value.toEscaped(EscapeMode::ErrorText),
            context.rule->caseSensitivity()));
    }
}


void EqualsBytesConstraint::validateBytes(const ValidationContext &context, const Bytes &value) const {
    if (isNotValid(value, context)) {
        throwValidationError(u8format(u8"The byte sequence {} \"{}\"", comparisonText(), _value.toHexForErrors()));
    }
}


EqualsMatrixConstraint::EqualsMatrixConstraint(const Integer rows, const Integer columns) :
    EqualsConstraint(rows), _columns{columns} {
}


auto EqualsMatrixConstraint::isNotValidColumns(
    const Integer &validatedValue, const ValidationContext &context) const -> bool {
    if (isNegated()) {
        return isEqual(validatedValue, _columns, context);
    }
    return !isEqual(validatedValue, _columns, context);
}


void EqualsMatrixConstraint::validateValueList(const ValidationContext &context) const {
    const auto &value = context.value;
    if (isNotValid(static_cast<Integer>(value->size()), context)) {
        throwValidationError(u8format(u8"The number of rows {} {}", comparisonText(), _value));
    }
    for (const auto &columns : *value) {
        if (isNotValidColumns(static_cast<Integer>(columns->size()), context)) {
            throwValidationError(u8format(u8"The number of columns {} {}", comparisonText(), _columns));
        }
    }
}


auto handleEqualsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    auto &node = context.node;
    auto &rule = context.rule;
    switch (rule->type().raw()) {
    case vr::RuleType::Integer:
        if (node->type() != ValueType::Integer) {
            throwValidationError(u8"The 'equals' constraint for an integer rule must be an integer");
        }
        return std::make_shared<EqualsIntegerConstraint>(node->asInteger());
    case vr::RuleType::Float:
        if (node->type() != ValueType::Float) {
            throwValidationError(u8"The 'equals' constraint for a float rule must be a float");
        }
        return std::make_shared<EqualsFloatConstraint>(node->asFloat());
    case vr::RuleType::Text:
        if (node->type() == ValueType::Text) {
            return std::make_shared<EqualsTextConstraint>(node->asText());
        }
        if (node->type() != ValueType::Integer) {
            throwValidationError(u8"The 'equals' constraint for a text rule must be a text or integer");
        }
        return std::make_shared<EqualsIntegerConstraint>(node->asInteger());
    case vr::RuleType::Bytes:
        if (node->type() == ValueType::Bytes) {
            return std::make_shared<EqualsBytesConstraint>(node->asBytes());
        }
        if (node->type() != ValueType::Integer) {
            throwValidationError(u8"The 'equals' constraint for a bytes rule must be a byte sequence or integer");
        }
        return std::make_shared<EqualsIntegerConstraint>(node->asInteger());
    case vr::RuleType::Boolean:
        if (node->type() != ValueType::Boolean) {
            throwValidationError(u8"The 'equals' constraint for a boolean rule must be a boolean");
        }
        return std::make_shared<EqualsBooleanConstraint>(node->asBoolean());
    case vr::RuleType::ValueList:
        if (node->type() != ValueType::Integer) {
            throwValidationError(u8"The 'equals' constraint for a value list must be an integer");
        }
        return std::make_shared<EqualsIntegerConstraint>(node->asInteger());
    case vr::RuleType::ValueMatrix:
        if (auto intList = node->asList<int>(); intList.size() == 2) {
            return std::make_shared<EqualsMatrixConstraint>(intList[0], intList[1]);
        }
        throwValidationError(u8"The 'equals' constraint for a value matrix must be a list with two integer values");
    case vr::RuleType::Section:
    case vr::RuleType::SectionList:
    case vr::RuleType::SectionWithTexts:
        if (node->type() != ValueType::Integer) {
            throwValidationError(u8"The 'equals' constraint for a section or section list must be an integer");
        }
        return std::make_shared<EqualsIntegerConstraint>(node->asInteger());
    default:
        break;
    };
    throwValidationError(u8format(u8"The 'equals' constraint is not supported for '{}' rules", rule->type().toText()));
}


}
