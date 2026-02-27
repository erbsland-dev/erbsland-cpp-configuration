// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RulesFromDocument.hpp"


#include "DependencyDefinition.hpp"
#include "RulesConstants.hpp"
#include "ValidationError.hpp"

#include "../utilities/InternalError.hpp"
#include "../value/ValueHelper.hpp"
#include "../value/ValueTreeWalker.hpp"


namespace erbsland::conf::impl {


void RulesFromDocument::handleTypeOrTemplate(const conf::ValuePtr &node, const RulePtr &rule) {
    if (const auto value = node->value(vrc::cType); value != nullptr) {
        if (value->type() != ValueType::Text) {
            throwValidationError(u8"The 'type' value must be a text", value->namePath(), value->location());
        }
        auto ruleType = vr::RuleType::fromText(value->asText());
        if (ruleType == vr::RuleType::Undefined) {
            throwValidationError(u8"Unknown rule type", value->namePath(), value->location());
        }
        const auto useTemplateValue = node->value(vrc::cUseTemplate);
        if (useTemplateValue != nullptr && !useTemplateValue->type().isStructural()) {
            throwValidationError(u8"The section cannot have both a 'type' and a 'use_template' value");
        }
        rule->setType(ruleType);
    } else if (const auto useTemplateValue = node->value(vrc::cUseTemplate); useTemplateValue != nullptr) {
        // Process the template first.
        processTemplate(node, useTemplateValue, rule);
    } else {
        throwValidationError(u8"The section must have either a 'type' or a 'use_template' value");
    }
}


void RulesFromDocument::handleCaseSensitive(const conf::ValuePtr &node, const RulePtr &rule) {
    if (auto caseSensitive = node->value(vrc::cCaseSensitive); caseSensitive != nullptr) {
        if (caseSensitive->type() != ValueType::Boolean) {
            throwValidationError(u8"The 'case_sensitive' value must be boolean");
        }
        rule->setCaseSensitivity(
            caseSensitive->asBoolean() ? CaseSensitivity::CaseSensitive : CaseSensitivity::CaseInsensitive);
    }
}


void RulesFromDocument::processTemplate(
    const conf::ValuePtr &node,
    const conf::ValuePtr &useTemplateValue,
    const RulePtr &rule) {

    ERBSLAND_CONF_REQUIRE_SAFETY(useTemplateValue != nullptr, "useTemplateValue must not be null");
    ERBSLAND_CONF_REQUIRE_SAFETY(rule != nullptr, "rule must not be null");
    try {
        if (useTemplateValue->type() != ValueType::Text) {
            throwValidationError(u8"The 'use_template' value must be a text");
        }
        if (!_pathForTemplate.empty()) {
            throwValidationError(u8"You must not use 'use_template' in template definitions");
        }
        NamePath templateNamePath;
        try {
            templateNamePath = NamePath{{vrc::cReservedTemplate, Name::createRegular(useTemplateValue->asText())}};
        } catch (const Error &error) {
            throwValidationError(u8format(
                u8"The name specified in 'use_template' is not a valid template name: {}",
                error.message()));
        }
        const auto templateNode = _document->value(templateNamePath);
        if (templateNode == nullptr) {
            throwValidationError(u8"The template referenced by 'use_template' does not exist");
        }
        if (templateNode->type() != ValueType::SectionWithNames && templateNode->type() != ValueType::SectionList) {
            throwValidationError(u8"Template definitions must be sections or section lists");
        }
        _pathForTemplate = node->namePath();
        if (templateNode->type() == ValueType::SectionList) {
            processAlternatives(templateNode, rule);
        } else {
            processNodeRules(templateNode, rule);
        }
        // Process all template nodes, like regular ones.
        ValueTreeWalker walker;
        walker.setRoot(templateNode);
        auto filter = [](const conf::ValuePtr &processedNode) -> bool {
            return processedNode->type().isMap() || processedNode->type() == ValueType::SectionList;
        };
        walker.setFilter(filter);
        auto visit = [this, templateNode](const conf::ValuePtr &processedNode) -> void {
            if (processedNode == templateNode) {
                return; // The template root is already merged into the current rule.
            }
            processDocumentNode(processedNode);
        };
        walker.walk(visit);
        _pathForTemplate.clear();
    } catch (const Error &error) {
        if (error.location().isUndefined()) {
            throw error.withNamePathAndLocation(useTemplateValue->namePath(), useTemplateValue->location());
        }
        throw;
    }
}


void RulesFromDocument::processImplicitRules(const conf::ValuePtr &node, const RulePtr &rule) {
    ERBSLAND_CONF_REQUIRE_SAFETY(node->type() == ValueType::IntermediateSection, "Expected intermediate section node");
    // For intermediate sections, create a rule that expects a section but do not add any constraints.
    rule->setLocation(node->location());
    rule->setType(vr::RuleType::Section);
}


void RulesFromDocument::processAlternatives(const conf::ValuePtr &node, const RulePtr &rule) {
    ERBSLAND_CONF_REQUIRE_SAFETY(node->type() == ValueType::SectionList, "Expected section list node");
    // For section lists, create a rule with alternatives. As we traverse the whole node tree,
    // the alternatives will be automatically created later.
    rule->setLocation(node->location());
    rule->setType(vr::RuleType::Alternatives);
}


void RulesFromDocument::processNameNode(const conf::ValuePtr &node) {
    const auto rule = std::make_shared<Rule>();
    rule->setLocation(node->location());
    rule->setRuleNamePath(createRuleNamePath(node->namePath()));
    _rules->addRule(rule); // add the rule early to ensure we have a valid parent node.
    rule->setType(vr::RuleType::Text);
    if (node->type() == ValueType::SectionList) {
        throwValidationError(u8"Name node-rules definitions cannot be alternatives");
    }
    if (node->type() == ValueType::IntermediateSection) {
        throwValidationError(u8"Name node-rules definition must not have subsections");
    }
    processNameNodeRules(node, rule);
}


void RulesFromDocument::processDependencies(const conf::ValuePtr &node) {
    if (node->type() != ValueType::SectionList) {
        throwValidationError(u8"Dependency 'vr_dependency' node-rules definitions must be section lists");
    }
    const auto parentRule = getParentRuleForNode(node);
    ERBSLAND_CONF_REQUIRE_SAFETY(parentRule != nullptr, "Expected parent rule for key node");
    // Process the individual dependency definitions.
    for (const auto &dependencyNode : getImplValue(node)->childrenImpl()) {
        if (dependencyNode->type() != ValueType::SectionWithNames) {
            throwValidationError(u8"Dependency definitions must be sections with regular names");
        }
        auto sourceSpecified = false;
        auto targetSpecified = false;
        auto mode = DependencyMode::Undefined;
        NamePathList sourcePaths;
        NamePathList targetPaths;
        String errorMessage;
        for (const auto &child : dependencyNode->childrenImpl()) {
            try {
                if (child->name() == vrc::depMode) {
                    if (child->type() != ValueType::Text) {
                        throwValidationError(u8"The 'mode' value in 'vr_dependency' must be a text value");
                    }
                    mode = DependencyMode::fromText(child->asText());
                    if (mode == DependencyMode::Undefined) {
                        throwValidationError(
                            u8"The 'mode' value in 'vr_dependency' must be one of: 'if', 'if_not', 'or', 'xnor', 'xor'");
                    }
                } else if (child->name() == vrc::depSource || child->name() == vrc::depTarget) {
                    auto namePathTexts = child->asList<String>();
                    if (namePathTexts.empty()) {
                        throwValidationError(u8format(
                            u8"The '{}' value in 'vr_dependency' must one or more text values",
                            child->name()));
                    }
                    if (namePathTexts.size() > 10) {
                        throwValidationError(u8format(
                            u8"This implementation does not support more than 10 '{}' paths in one 'vr_dependency'",
                            child->name()));
                    }
                    NamePathList paths;
                    for (const auto &namePathText : namePathTexts) {
                        NamePath path;
                        try {
                            path = NamePath::fromText(namePathText);
                        } catch (const Error&) {
                            throwValidationError(u8format(
                                u8"A name path in '{}' in 'vr_dependency' is not valid",
                                child->name()));
                        }
                        paths.emplace_back(std::move(path));
                    }
                    if (child->name() == vrc::depSource) {
                        sourcePaths = std::move(paths);
                        sourceSpecified = true;
                    } else {
                        targetPaths = std::move(paths);
                        targetSpecified = true;
                    }
                } else if (child->name() == vrc::depError) {
                    if (child->type() != ValueType::Text) {
                        throwValidationError(u8"The 'error' value in 'vr_dependency' must be a text value");
                    }
                    errorMessage = child->asText();
                } else {
                    throwValidationError(u8"Unexpected element in 'vr_dependency'");
                }
            } catch (const Error &error) {
                if (error.location().isUndefined()) {
                    throw error.withNamePathAndLocation(child->namePath(), child->location());
                }
                throw;
            }
        }
        if (mode == DependencyMode::Undefined) {
            throwValidationError(u8"A 'vr_dependency' definition must have a 'mode' value");
        }
        if (!sourceSpecified) {
            throwValidationError(u8"A 'vr_dependency' definition must have a 'source' value");
        }
        if (!targetSpecified) {
            throwValidationError(u8"A 'vr_dependency' definition must have a 'target' value");
        }
        auto dependencyDefinition = DependencyDefinition::create(
            mode, sourcePaths, targetPaths, errorMessage);
        dependencyDefinition->setLocation(dependencyNode->location());
        parentRule->addDependencyDefinition(dependencyDefinition);
    }
}


void RulesFromDocument::processKey(const conf::ValuePtr &node) {
    if (node->type() != ValueType::SectionList) {
        throwValidationError(u8"Key 'vr_key' node-rules definitions must be section lists");
    }
    const auto parentRule = getParentRuleForNode(node);
    ERBSLAND_CONF_REQUIRE_SAFETY(parentRule != nullptr, "Expected parent rule for key node");
    // Assign the individual keys
    for (const auto &child : getImplValue(node)->childrenImpl()) {
        Name name;
        const auto nameValue = child->value(vrc::keyName);
        if (nameValue != nullptr) {
            if (nameValue->type() != ValueType::Text) {
                throwValidationError(u8"The 'name' in 'vr_key' must be a text value with a regular name");
            }
            try {
                name = Name::createRegular(nameValue->asText());
            } catch (const Error &error) {
                throwValidationError(u8format(
                    u8"The 'name' in 'vr_key' is not a valid regular name: {}",
                    error.message()),
                    nameValue->namePath(),
                    nameValue->location());
            }
        }
        const auto keyPathValue = child->value(vrc::keyKey);
        if (keyPathValue == nullptr) {
            throwValidationError(u8"A 'vr_key' definition must have a 'key' value");
        }
        const auto keyPaths = keyPathValue->toValueList();
        if (keyPaths.empty()) {
            throwValidationError(u8"The 'key' in 'vr_key' must be either a text value or a list of text values");
        }
        if (keyPaths.size() > 10) {
            throwValidationError(u8"This implementation does not support more than 10 'key' paths in one 'vr_key'");
        }
        for (const auto &keyPath : keyPaths) {
            if (keyPath->type() != ValueType::Text) {
                throwValidationError(u8format(
                    u8"Expected one or more text values in 'key' of 'vr_key', but got {}",
                    keyPath->type().toValueDescription(true)),
                    keyPath->namePath(),
                    keyPath->location());
            }
        }
        auto caseSensitivity = CaseSensitivity::CaseInsensitive;
        const auto caseSensitiveValue = child->value(vrc::cCaseSensitive);
        if (caseSensitiveValue != nullptr) {
            if (caseSensitiveValue->type() != ValueType::Boolean) {
                throwValidationError(u8"The 'case_sensitive' value must be boolean");
            }
            caseSensitivity = caseSensitiveValue->asBoolean() ? CaseSensitivity::CaseSensitive : CaseSensitivity::CaseInsensitive;
        }
        KeyDefinition::Keys keys;
        for (const auto &keyPath : keyPaths) {
            try {
                keys.emplace_back(NamePath::fromText(keyPath->asText()));
            } catch (const Error &error) {
                throwValidationError(u8format(
                    u8"A name path in 'key' in 'vr_key' is not valid: {}", error.message()),
                    keyPath->namePath(),
                    keyPath->location());
            }
        }
        parentRule->addKeyDefinition(KeyDefinition::create(
            name,
            keys,
            caseSensitivity,
            child->location()));

        // scan for additional unwanted elements.
        for (const auto &subChild : *child) {
            if (subChild->name() != vrc::keyKey &&
                subChild->name() != vrc::keyName &&
                subChild->name() != vrc::cCaseSensitive) {
                throwValidationError(u8"Unexpected element in 'vr_key'", subChild->namePath(), subChild->location());
            }
        }
    }
}


auto RulesFromDocument::getParentRuleForNode(const conf::ValuePtr &node) const -> RulePtr {
    auto ruleNamePath = createRuleNamePath(node->namePath());
    if (ruleNamePath.empty()) {
        return nullptr;
    }
    if (ruleNamePath.size() == 1) {
        return _rules->root();
    }
    return _rules->ruleForNamePath(ruleNamePath, ruleNamePath.size() - 1);
}


auto RulesFromDocument::createRuleNamePath(const NamePath &namePath) const -> NamePath {
    if (namePath.empty() || _pathForTemplate.empty()) {
        return namePath;
    }
    if (namePath.front() == vrc::cReservedTemplate) {
        auto result = _pathForTemplate;
        auto it = namePath.begin();
        ++it; // skip "vr_template"
        if (it != namePath.end()) {
            ++it; // skip template name
        }
        for (; it != namePath.end(); ++it) {
            result.append(*it);
        }
        return result;
    }
    return namePath;
}


auto RulesFromDocument::createTargetNamePath(const NamePath &namePath) const -> NamePath {
    NamePath result;
    std::size_t startIndex = 0;
    if (isTemplatePath(namePath)) {
        ERBSLAND_CONF_REQUIRE_SAFETY(!_pathForTemplate.empty(), "Expected non-empty _pathForTemplate");
        result = _pathForTemplate;
        startIndex = 2; // skip "vr_template.<template-name>"
    }
    appendRegularNames(result, namePath, startIndex);
    return result;
}


auto RulesFromDocument::isTemplatePath(const NamePath &namePath) -> bool {
    return !namePath.empty() && namePath.front() == vrc::cReservedTemplate;
}


void RulesFromDocument::appendRegularNames(NamePath &result, const NamePath &namePath, const std::size_t startIndex) {
    for (std::size_t i = startIndex; i < namePath.size(); ++i) {
        const auto &name = namePath.at(i);
        if (name.type() != NameType::Regular) {
            continue;
        }
        if (name.isEscapedReservedValidationRule()) {
            result.append(name.withReservedVRPrefixRemoved());
        } else {
            result.append(name);
        }
    }
}


}
