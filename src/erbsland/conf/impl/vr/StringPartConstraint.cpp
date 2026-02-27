// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringPartConstraint.hpp"


#include "MinMaxConstraint.hpp"
#include "ValidationError.hpp"


namespace erbsland::conf::impl {


void StringPartConstraint::validateText(const ValidationContext &context, const String &value) const {
    bool doesMatch = false;
    for (const auto &expectedValue : _expectedValues) {
        if (doesPartMatch(expectedValue, value, context)) {
            doesMatch = true;
            break;
        }
    }
    if (doesMatch == isNegated()) {
        String expected;
        for (const auto &expValue : _expectedValues) {
            if (!expected.empty()) {
                expected.append(u8" or ");
            }
            expected.append(u8format(u8"\"{}\"", expValue.toSafeText()));
        }
        throwValidationError(u8format(
            u8"The text {} {} {} ({})",
            isNegated() ? String{u8"must not"} : String{u8"does not"},
            partText(),
            expected,
            context.rule->caseSensitivity()));
    }
}


namespace {
template<typename Constraint>
[[nodiscard]] auto createConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (rule->type() == vr::RuleType::Text) {
        const auto textValues = node->asList<String>();
        if (textValues.empty()) {
            throwValidationError(u8format(
                u8"The '{}' constraint must specify a single text value or a list of texts",
                node->name()));
        }
        return std::make_shared<Constraint>(textValues);
    }
    throwValidationError(u8format(
        u8"The '{}' constraint is not supported for '{}' rules",
        node->name(),
        rule->type().toText()));
}
}


auto handleStartsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    return createConstraint<StartsConstraint>(context);
}


auto handleEndsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    return createConstraint<EndsConstraint>(context);
}


auto handleContainsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    return createConstraint<ContainsConstraint>(context);
}


}
