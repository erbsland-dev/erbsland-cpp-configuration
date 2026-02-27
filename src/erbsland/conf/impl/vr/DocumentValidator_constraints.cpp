// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DocumentValidator.hpp"


#include "ValidationContext.hpp"
#include "ValidationError.hpp"

#include "../utilities/InternalError.hpp"


namespace erbsland::conf::impl {


void DocumentValidator::validateNameConstraints(const RulePtr &rule, const ValuePtr &value) {
    if (!rule->hasNameConstraints()) {
        return; // skip if this rule has no name constraints.
    }
    const auto name = value->name();
    if (value->name().type() == NameType::Index || value->name().type() == NameType::TextIndex) {
        throwValidationError(
            u8"Expected a named value, but got a list entry or text index",
            value->namePath(),
            value->location());
    }
    const auto nameRule = rule->nameConstraints();
    ERBSLAND_CONF_REQUIRE_SAFETY(nameRule != nullptr, "Unexpected missing name rule");
    const auto validationContext = ValidationContext{
        .target = ValidationTarget::Name,
        .value = value,
        .rule = nameRule,
    };
    validateConstraints(nameRule, validationContext);
}


void DocumentValidator::validateValueConstraints(const RulePtr &rule, const ValuePtr &value) {
    const auto validationContext = ValidationContext{
        .target = ValidationTarget::Value,
        .value = value,
        .rule = rule,
    };
    validateConstraints(rule, validationContext);
}


void DocumentValidator::validateConstraints(const RulePtr &rule, const ValidationContext &validationContext) {
    for (const auto &constraint : rule->constraintsImpl()) {
        ERBSLAND_CONF_REQUIRE_SAFETY(constraint->type() != vr::ConstraintType::Undefined, "Unexpected constraint type");
        ERBSLAND_CONF_REQUIRE_SAFETY(constraint->type() != vr::ConstraintType::Version, "Unexpected constraint type");
        if (constraint->type() == vr::ConstraintType::Key) {
            continue; // ignore key constraints for now.
        }
        try {
            constraint->validate(validationContext);
        } catch (const Error &error) {
            if (error.category() == ErrorCategory::Validation) {
                if (constraint->hasCustomError()) {
                    throw error.withMessage(constraint->customError());
                }
                if (rule->hasCustomError()) {
                    throw error.withMessage(rule->customError());
                }
            }
            throw;
        }
    }
}


auto DocumentValidator::expectedValueTypeText(const RulePtr &rule) const -> String {
    return impl::expectedValueTypeText(rule, _version);
}


auto DocumentValidator::parentLocationText(const conf::ValuePtr &value) -> String {
    if (value == nullptr || value->isDocument()) {
        // Accept value == nullptr, when no parent indicates a note at the document root.
        return u8"the document root";
    }
    if (value->type() == ValueType::SectionWithNames || value->type() == ValueType::IntermediateSection) {
        return u8format("the section '{}'", value->namePath().toText());
    }
    if (value->type() == ValueType::SectionWithTexts) {
        return u8format("the section with texts '{}'", value->namePath().toText());
    }
    return {};
}


void DocumentValidator::throwExpectedVsActual(const RulePtr &rule, const ValuePtr &value) const {
    impl::throwExpectedVsActual(rule, value, _version);
}


auto DocumentValidator::errorNamePathsOr(
    const NamePathList &paths,
    const bool forNegation) -> String {

    String result;
    if (paths.size() > 1) {
        if (forNegation) {
            result = u8"configure none of ";
        } else {
            result = u8"at least one of ";
        }
    } else {
        if (forNegation) {
            result = u8"not configure ";
        }
    }
    result += u8"'";
    for (std::size_t i = 0; i != paths.size(); ++i) {
        result += paths[i].toText();
        if (i < paths.size() - 1) {
            if (i == paths.size() - 2) {
                result += String{u8"', or '"};
            } else {
                result += String{u8"', '"};
            }
        }
    }
    result += u8"'";
    return result;
}


}
