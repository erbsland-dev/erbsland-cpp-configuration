// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RulesFromDocument.hpp"


#include "ValidationError.hpp"
#include "VersionMask.hpp"


namespace erbsland::conf::impl {


auto RulesFromDocument::handleDefault(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (!rule->type().acceptsDefaults()) {
        throwValidationError(u8format(
            u8"A default value cannot be used for '{}' node rules", rule->type().toText()));
    }
    if (!rule->type().matchesValueType(node->type())) {
        throwValidationError(u8format(
            u8"The 'default' value must be {}", rule->type().expectedValueTypeText()));
    }
    rule->setDefaultValue(std::dynamic_pointer_cast<const Value>(node)->deepCopy());
    return {};
}


auto RulesFromDocument::handleDescription(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (node->type() != ValueType::Text) {
        throwValidationError(u8"The 'description' value must be text");
    }
    rule->setDescription(node->asText());
    return {};
}


auto RulesFromDocument::handleError(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (node->type() != ValueType::Text) {
        throwValidationError(u8"The 'error' value must be text");
    }
    rule->setErrorMessage(node->asText());
    return {};
}


auto RulesFromDocument::handleIsOptional(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (node->type() != ValueType::Boolean) {
        throwValidationError(u8"The 'is_optional' value must be boolean");
    }
    rule->setOptional(node->asBoolean());
    return {};
}


auto RulesFromDocument::handleIsSecret(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (node->type() != ValueType::Boolean) {
        throwValidationError(u8"The 'is_secret' value must be boolean");
    }
    rule->setSecret(node->asBoolean());
    return {};
}


auto RulesFromDocument::handleTitle(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (node->type() != ValueType::Text) {
        throwValidationError(u8"The 'title' value must be a text");
    }
    rule->setTitle(node->asText());
    return {};
}


auto RulesFromDocument::handleVersion(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    const auto versionList = node->asList<Integer>();
    if (versionList.empty()) {
        throwValidationError(u8"The 'version' value must be one or more integers");
    }
    // test for uniqueness.
    for (std::size_t i = 0; i < versionList.size(); ++i) {
        if (versionList[i] < 0) {
            throwValidationError(u8"The values in 'version' must be non-negative integers");
        }
        for (auto j = i + 1; j < versionList.size(); ++j) {
            if (versionList[i] == versionList[j]) {
                throwValidationError(u8"The values in 'version' must be unique");
            }
        }
    }
    auto mask = VersionMask::fromIntegers(versionList);
    if (context.isNegated) {
        mask = !mask;
    }
    rule->limitVersionMask(mask);
    return {};
}


auto RulesFromDocument::handleMinimumVersion(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (node->type() != ValueType::Integer) {
        throwValidationError(u8"The 'minimum_version' value must be an integer");
    }
    auto version = node->asInteger();
    if (version < 0) {
        throwValidationError(u8"The 'minimum_version' value must be non-negative"
        );
    }
    auto mask = VersionMask::fromRanges({VersionRange{version, std::numeric_limits<Integer>::max()}});
    if (context.isNegated) {
        mask = !mask;
    }
    rule->limitVersionMask(mask);
    return {};
}


auto RulesFromDocument::handleMaximumVersion(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (node->type() != ValueType::Integer) {
        throwValidationError(u8"The 'maximum_version' value must be an integer");
    }
    auto version = node->asInteger();
    if (version < 0) {
        throwValidationError(u8"The 'minimum_version' value must be non-negative"
        );
    }
    auto mask = VersionMask::fromRanges({VersionRange{0, version}});
    if (context.isNegated) {
        mask = !mask;
    }
    rule->limitVersionMask(mask);
    return {};
}


}
