// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RulesBuilder.hpp"


#include "RulesFromDocument.hpp"
#include "ValidationError.hpp"

#include "../../Error.hpp"


namespace erbsland::conf::impl {


RulesBuilder::RulesBuilder() : _rules(std::make_shared<Rules>()) {
}


void RulesBuilder::readFromDocument(const DocumentPtr &document) {
    RulesFromDocument rulesFromDocument{_rules, document};
    rulesFromDocument.read();
    _rules->validateDefinition();
}


auto RulesBuilder::resolveRuleNamePath(const NamePathLike &namePathLike) -> NamePath {
    NamePath namePath;
    if (std::holds_alternative<std::size_t>(namePathLike)) {
        throwValidationError("The given name-path is not valid");
    }
    if (std::holds_alternative<String>(namePathLike)) {
        namePath = NamePath::fromText(std::get<String>(namePathLike));
    } else if (std::holds_alternative<NamePath>(namePathLike)) {
        namePath = std::get<NamePath>(namePathLike);
    } else if (std::holds_alternative<Name>(namePathLike)) {
        namePath = NamePath{{std::get<Name>(namePathLike)}};
    }
    if (namePath.empty()) {
        throwValidationError("An empty name-path is not valid");
    }
    if (namePath.containsText() || namePath.containsIndex()) {
        throwValidationError("Text names or indexes are not allowed in a name-path for validation rules");
    }
    return namePath;
}


void RulesBuilder::reset() {
    _rules = std::make_shared<Rules>();
}


auto RulesBuilder::takeRules() -> RulesPtr {
    _rules->validateDefinition();
    auto result = _rules;
    reset();
    return result;
}


}

