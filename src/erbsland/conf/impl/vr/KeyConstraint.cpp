// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "KeyConstraint.hpp"


#include "ValidationError.hpp"

#include <utility>


namespace erbsland::conf::impl {


KeyConstraint::KeyConstraint(KeyReferences keyReferences) : _keyReferences{std::move(keyReferences)} {
    setType(vr::ConstraintType::Key);
}


auto KeyConstraint::getKeyReferences() const -> const KeyReferences& {
    return _keyReferences;
}


auto handleKeyConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    if (node->type() != ValueType::Text && node->type() != ValueType::ValueList) {
        throwValidationError(u8"The 'key' value must be a text or a list of text with the referenced keys");
    }
    if (node->type() == ValueType::ValueList) {
        for (const auto &keyReference : node->asValueList()) {
            if (keyReference->type() != ValueType::Text) {
                throwValidationError(u8"The 'key' value must be a text or a list of text with the referenced keys");
            }
        }
    }
    KeyConstraint::KeyReferences keyReferences;
    for (const auto &keyReferenceValue : node->toValueList()) {
        try {
            keyReferences.emplace_back(NamePath::fromText(keyReferenceValue->asText()));
        } catch (const Error &error) {
            throwValidationError(u8format(
                u8"Invalid name-path for key reference: {}",
                error.message()),
                keyReferenceValue->namePath(),
                keyReferenceValue->location());
        }
        // the validation of the name-path is done in `RulesDefinitionValidator`.
    }
    return std::make_shared<KeyConstraint>(keyReferences);
}


}

