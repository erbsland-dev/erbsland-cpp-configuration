// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "InConstraint.hpp"


#include "ValidationError.hpp"


namespace erbsland::conf::impl {


void InIntegerConstraint::validateInteger(const ValidationContext &context, const Integer value) const {
    if (isNotValid(value, context)) {
        String expected;
        for (std::size_t i = 0; i < _values.size(); ++i) {
            if (i != 0) {
                expected.append(u8" or ");
            }
            expected.append(u8format(u8"{}", _values[i]));
        }
        throwValidationError(u8format(u8"The value {} {}", comparisonText(), expected));
    }
}


void InFloatConstraint::validateFloat(const ValidationContext &context, const Float value) const {
    if (isNotValid(value, context)) {
        String expected;
        for (std::size_t i = 0; i < _values.size(); ++i) {
            if (i != 0) {
                expected.append(u8" or ");
            }
            expected.append(u8format(u8"{:.6}", _values[i]));
        }
        throwValidationError(u8format(u8"The value {} {} (within platform tolerance)", comparisonText(), expected));
    }
}


void InTextConstraint::validateText(const ValidationContext &context, const String &value) const {
    if (isNotValid(value, context)) {
        String expected;
        for (std::size_t i = 0; i < _values.size(); ++i) {
            if (i != 0) {
                expected.append(u8" or ");
            }
            expected.append(u8format(u8"\"{}\"", _values[i].toEscaped(EscapeMode::ErrorText)));
        }
        throwValidationError(u8format(u8"The text {} {} ({})",
            comparisonText(),
            expected,
            context.rule->caseSensitivity()));
    }
}


void InBytesConstraint::validateBytes(const ValidationContext &context, const Bytes &value) const {
    if (isNotValid(value, context)) {
        String expected;
        for (std::size_t i = 0; i < _values.size(); ++i) {
            if (i != 0) {
                expected.append(u8" or ");
            }
            expected.append(u8format(u8"\"{}\"", _values[i].toHexForErrors()));
        }
        throwValidationError(u8format(u8"The byte sequence {} {}", comparisonText(), expected));
    }
}


namespace {
template<typename Constraint, typename ValueType>
[[nodiscard]] auto createInConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    const auto values = node->asList<ValueType>();
    if (values.empty()) {
        throwValidationError(u8format(
            u8"The '{}' constraint must specify a single {} value or a list of {} values",
            node->name(),
            rule->type().toText(),
            rule->type().toText()));
    }
    if (Constraint::hasDuplicate(values, rule->caseSensitivity())) {
        throwValidationError(u8format(
            u8"The '{}' list must not contain duplicate values",
            node->name()));
    }
    return std::make_shared<Constraint>(values);
}
}


auto handleInConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    switch (context.rule->type().raw()) {
    case vr::RuleType::Integer:
        return createInConstraint<InIntegerConstraint, Integer>(context);
    case vr::RuleType::Float:
        return createInConstraint<InFloatConstraint, Float>(context);
    case vr::RuleType::Text:
        return createInConstraint<InTextConstraint, String>(context);
    case vr::RuleType::Bytes:
        return createInConstraint<InBytesConstraint, Bytes>(context);
    default:
        break;
    }
    throwValidationError(u8format(
        u8"The '{}' constraint is not supported for '{}' rules",
        context.node->name(),
        context.rule->type().toText()));
}


}
