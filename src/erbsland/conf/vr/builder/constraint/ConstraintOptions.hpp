// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../../impl/vr/Constraint.hpp"
#include "../../../impl/vr/Rule.hpp"
#include "../../../String.hpp"


namespace erbsland::conf::vr::builder {


/// Optional behavior shared by all builder constraints.
struct ConstraintOptions {
    bool isNegated{false};
    String errorMessage{};

    [[nodiscard]] auto prefixedConstraintName(const String &constraintName) const -> String {
        return withNotPrefix(constraintName, isNegated);
    }

    void applyTo(impl::Constraint &constraint, const String &constraintName) const {
        constraint.setName(prefixedConstraintName(constraintName));
        constraint.setNegated(isNegated);
        if (!errorMessage.empty()) {
            constraint.setErrorMessage(errorMessage);
        }
    }

    void addToRule(
        impl::Rule &rule,
        const impl::ConstraintPtr &constraint,
        const String &constraintName) const {

        applyTo(*constraint, constraintName);
        rule.addOrOverwriteConstraint(constraint);
    }

private:
    [[nodiscard]] static auto withNotPrefix(const String &name, const bool isNegated) -> String {
        if (!isNegated) {
            return name;
        }
        return String{u8"not_"} + name;
    }
};


}
