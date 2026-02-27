// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../attribute/Attribute.hpp"

#include "../../../impl/utf8/U8Format.hpp"
#include "../../../vr/RuleType.hpp"

#include <initializer_list>


namespace erbsland::conf::vr::builder {


/// Base interface for builder constraints with common validation helpers.
struct ConstraintAttribute : Attribute {
protected:
    static void requireRuleTypeForConstraint(
        const impl::Rule &rule,
        const String &constraintName,
        const std::initializer_list<vr::RuleType> supportedTypes) {

        if (!hasRuleType(rule, supportedTypes)) {
            throwUnsupportedConstraint(rule, constraintName);
        }
    }

private:
    [[nodiscard]] static auto hasRuleType(
        const impl::Rule &rule,
        const std::initializer_list<vr::RuleType> supportedTypes) -> bool {

        for (const auto supportedType : supportedTypes) {
            if (rule.type() == supportedType) {
                return true;
            }
        }
        return false;
    }

    [[noreturn]] static void throwUnsupportedConstraint(const impl::Rule &rule, const String &constraintName) {
        throwValidationError(impl::u8format(
            u8"The '{}' constraint is not supported for '{}' rules",
            constraintName,
            rule.type().toText()));
    }
};


}
