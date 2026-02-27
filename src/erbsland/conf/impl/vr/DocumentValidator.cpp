// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DocumentValidator.hpp"


#include "KeyConstraint.hpp"
#include "ValidationError.hpp"

#include "../utilities/InternalError.hpp"
#include "../value/ValueHelper.hpp"

#include <ranges>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>


namespace erbsland::conf::impl {


DocumentValidator::DocumentValidator(RulePtr root, conf::ValuePtr value, const Integer version)
:
    _root{std::move(root)},
    _value{std::move(value)},
    _version{version} {

    ERBSLAND_CONF_REQUIRE_SAFETY(_root != nullptr, "The root rule must not be null");
    ERBSLAND_CONF_REQUIRE_SAFETY(_value != nullptr, "The value must not be null");
    ERBSLAND_CONF_REQUIRE_DEBUG(_root->type() == vr::RuleType::Section, "The root rule must be a section");
    ERBSLAND_CONF_REQUIRE_DEBUG(
        _value->isDocument() || _value->isSectionWithNames(),
        "The value must be a document or a section with names");
}


void DocumentValidator::validate() {
    if (_root->empty()) {
        return;
    }

    validatePass1();
    validatePass2();
}


void DocumentValidator::validatePass1() {

    // initialize the use-indexes flag with root key definitions
    _useIndexes = _root->hasKeyDefinitions();

    std::vector<Frame> stack;
    stack.reserve(32);
    stack.emplace_back(Frame{.valueNode=_value, .ruleNode=_root});

    while (!stack.empty()) {
        auto [value, rule] = stack.back();
        stack.pop_back();
        ERBSLAND_CONF_REQUIRE_SAFETY(value != nullptr, "The value node must not be null");
        ERBSLAND_CONF_REQUIRE_SAFETY(rule != nullptr, "The rule node must not be null");
        if (value != _value) { // do not validate the root value.
            const auto valueImpl = getImplValue(value);
            // Drop defaults from previous validations for this node before evaluating constraints and descendants.
            valueImpl->removeDefaultValues();
            rule = validate(rule, valueImpl);
            if (rule == nullptr) { // = skip this branch (not-validated or no matching alternative)
                continue; // skip this branch
            }
            valueImpl->setValidationRule(rule);
            if (rule->type() == vr::RuleType::ValueList || rule->type() == vr::RuleType::ValueMatrix) {
                // Value list and matrix entries are already validated at this point.
                // Skipping the rest of this branch.
                continue;
            }
        } else { // for the root value, only remove defaults and assign the root rule to mark it as validated.
            callImplValueFn(value, [&rule](auto &&valueImpl) -> void {
                valueImpl->removeDefaultValues();
                valueImpl->setValidationRule(rule);
            });
        }
        // Descend into the child values:
        // Add in reverse order to preserve the original order of validation.
        std::unordered_set<RulePtr> rulesWithMatchingValues;
        for (const auto &child : std::ranges::reverse_view(*value)) {
            auto nextValue = getImplValue(child);
            auto nextRule = nextRuleForValue(rule, nextValue); // may throw
            rulesWithMatchingValues.insert(nextRule);
            stack.emplace_back(Frame{.valueNode=std::move(nextValue), .ruleNode=std::move(nextRule)});
        }
        // Now handle the rules that had no matching values.
        for (const auto &childRule : rule->childrenImpl()) {
            if (rulesWithMatchingValues.contains(childRule)) {
                continue; // ignore all rules we already matched with values.
            }
            handleMissingValues(childRule, value);
        }
    }
}


void DocumentValidator::validatePass2() {
    if (!_useIndexes && !_useDependencies) {
        return; // Skip pass 2 if we have no indexes and no dependency checks.
    }

    // for the second pass, scan the value tree and the assigned rules.
    struct Pass2Frame {
        conf::ValuePtr value;
        RulePtr rule;
        std::size_t addedIndexes{0}; // How many indexes were added to the stack.
        bool isExit{false}; // if this frame triggers the exit.

        [[nodiscard]] static auto createEnter(conf::ValuePtr valueNode, RulePtr ruleNode) noexcept -> Pass2Frame {
            return {.value=std::move(valueNode), .rule=std::move(ruleNode)};
        }
        [[nodiscard]] auto createExit() const noexcept -> Pass2Frame {
            return {.value=value, .rule=rule, .addedIndexes=addedIndexes, .isExit=true};
        }
    };

    std::vector<Pass2Frame> stack;
    stack.reserve(32);
    stack.emplace_back(Pass2Frame::createEnter(_value, _root));
    KeyIndexList keyIndexStack;

    while (!stack.empty()) {
        auto frame = stack.back();
        stack.pop_back();
        ERBSLAND_CONF_REQUIRE_SAFETY(frame.value != nullptr, "The value node must not be null");
        ERBSLAND_CONF_REQUIRE_SAFETY(frame.rule != nullptr, "The rule node must not be null");

        if (frame.isExit) {
            if (frame.addedIndexes > 0) {
                ERBSLAND_CONF_REQUIRE_SAFETY(keyIndexStack.size() >= frame.addedIndexes, "Index stack mismatch");
                keyIndexStack.resize(keyIndexStack.size() - frame.addedIndexes);
            }
            continue;
        }

        if (frame.rule->hasKeyDefinitions()) {
            // Validate the definitions and add all names indexes to the index stack.
            auto keyIndexes = buildKeyIndexes(frame.value, frame.rule);
            frame.addedIndexes = keyIndexes.size();
            keyIndexStack.reserve(keyIndexStack.size() + keyIndexes.size());
            keyIndexStack.insert(
                keyIndexStack.end(),
                std::make_move_iterator(keyIndexes.begin()),
                std::make_move_iterator(keyIndexes.end()));
        }
        if (frame.rule->hasConstraint(vr::ConstraintType::Key)) {
            validateKeyConstraint(keyIndexStack, frame.value, frame.rule);
        }
        if (frame.rule->hasDependencyDefinitions()) {
            validateDependencies(frame.value, frame.rule);
        }
        stack.emplace_back(frame.createExit());
        for (const auto &child : std::ranges::reverse_view(*frame.value)) {
            if (child->isDefaultValue()) {
                continue; // ignore default values applied to the value tree.
            }
            auto childRule = std::dynamic_pointer_cast<Rule>(child->validationRule());
            if (childRule != nullptr) {
                if (childRule->type() == vr::RuleType::NotValidated) {
                    continue; // ignore not validated value trees.
                }
                stack.emplace_back(Pass2Frame::createEnter(child, childRule));
            }
        }
    }
}


auto DocumentValidator::buildKeyIndexes(const conf::ValuePtr &value, const RulePtr &rule) -> KeyIndexList {
    KeyIndexList result;
    for (const auto &keyDefinition : rule->keyDefinitions()) {
        auto keyIndex = buildKeyIndexAndValidateUniqueness(value, keyDefinition);
        if (!keyIndex->name().empty()) {
            // only store named key indexes.
            result.emplace_back(std::move(keyIndex));
        }
    }
    return result;
}


auto DocumentValidator::buildKeyIndexAndValidateUniqueness(
    [[maybe_unused]] const conf::ValuePtr &value,
    const KeyDefinitionPtr &keyDefinition) -> KeyIndexPtr {

    NamePath listPath;
    std::vector<NamePath> valuePaths;
    for (const auto &key : keyDefinition->keys()) {
        ERBSLAND_CONF_REQUIRE_DEBUG(!key.containsIndex(), "The key must not contain an index");
        ERBSLAND_CONF_REQUIRE_DEBUG(!key.containsText(), "The key must not contain text");
        auto entryIndex = key.find(vrc::cReservedEntry);
        auto newListPath = key.subPath(0, entryIndex);
        if (listPath.empty()) {
            listPath = newListPath;
        } else {
            ERBSLAND_CONF_REQUIRE_SAFETY(listPath == newListPath, "The list portion of key paths must be equal");
        }
        auto valuePath = key.subPath(entryIndex + 1);
        ERBSLAND_CONF_REQUIRE_SAFETY(!valuePath.empty(), "The value path must not be empty");
        ERBSLAND_CONF_REQUIRE_SAFETY(
            valuePath.find(vrc::cReservedEntry) == NamePath::npos,
            "A key must not point into nested lists.");
        valuePaths.emplace_back(std::move(valuePath));
    }

    auto keyIndex = std::make_shared<KeyIndex>(
        keyDefinition->name(), keyDefinition->caseSensitivity(), valuePaths.size());
    // first, try to get a section list or return an empty index.
    if (!value->hasValue(listPath)) {
        return keyIndex;
    }
    auto listValue = value->value(listPath);
    if (listValue->type() != ValueType::SectionList) {
        return keyIndex;
    }
    // add all values for each entry to the index and check for uniqueness.
    for (const auto &entry : *listValue) {
        StringList keyElements;
        bool atLeastOneValueExists = false;
        for (const auto &valuePath : valuePaths) {
            const auto entryValue = entry->value(valuePath);
            if (entryValue != nullptr && (entryValue->type() == ValueType::Text || entryValue->type() == ValueType::Integer)) {
                keyElements.emplace_back(entryValue->toTextRepresentation());
                atLeastOneValueExists = true;
            } else {
                keyElements.emplace_back();
            }
        }
        if (!atLeastOneValueExists) {
            continue; // ignore entries with non-existing values
        }
        auto success = keyIndex->tryAddKey(Key{keyElements});
        if (!success) {
            if (valuePaths.size() == 1) {
                throwValidationError(u8format(
                    u8"The key '{}' is not unique in the list '{}'. Found a duplicate",
                    valuePaths.front().toText(),
                    listValue->namePath()),
                    entry->namePath(),
                    entry->location());
            }
            StringList keyNamePathsForError;
            for (const auto &valuePath : valuePaths) {
                keyNamePathsForError.emplace_back(valuePath.toText());
            }
            throwValidationError(u8format(
                u8"The combines keys '{}' are not unique in the list '{}'. Found a duplicate",
                String{u8"', '"}.join(keyNamePathsForError),
                listValue->namePath()),
                entry->namePath(),
                entry->location());
        }
    }
    return keyIndex; // At this point, no duplicates were found and the index was successfully created.
}


void DocumentValidator::validateKeyConstraint(
    const KeyIndexList &indexStack,
    const conf::ValuePtr &value,
    const RulePtr &rule) {

    ERBSLAND_CONF_REQUIRE_DEBUG(
        value->type() == ValueType::Text || value->type() == ValueType::Integer,
        "The key constraint can only be applied to text or integer values");
    // Prepare the actual key text that must be found in the index.
    const auto testedKey = value->toTextRepresentation();
    auto keyConstraint = std::dynamic_pointer_cast<KeyConstraint>(rule->constraint(vr::ConstraintType::Key));
    ERBSLAND_CONF_REQUIRE_DEBUG(keyConstraint != nullptr, "Missing key constraint");
    auto keyReferences = keyConstraint->getKeyReferences();
    ERBSLAND_CONF_REQUIRE_DEBUG(!keyReferences.empty(), "Key references cannot be empty");
    bool foundKey = false;
    std::vector<KeyIndexPtr> matchingIndexes;
    matchingIndexes.reserve(keyReferences.size());
    for (const auto &keyReference : keyReferences) {
        ERBSLAND_CONF_REQUIRE_DEBUG(!keyReference.empty(), "Key reference cannot be empty");
        const auto &keyName = keyReference.at(0);
        ERBSLAND_CONF_REQUIRE_DEBUG(keyName.type() == NameType::Regular, "First element must be a regular name");
        KeyIndexPtr keyIndex;
        for (const auto &index : std::ranges::reverse_view(indexStack)) {
            if (index->name() == keyName) {
                keyIndex = index;
                break;
            }
        }
        ERBSLAND_CONF_REQUIRE_DEBUG(keyIndex != nullptr, "Missing key index");
        matchingIndexes.emplace_back(keyIndex);
        if (keyReference.size() > 1) {
            ERBSLAND_CONF_REQUIRE_DEBUG(keyReference.at(1).type() == NameType::Index, "Second element must be an index");
            auto index = keyReference.at(1).asIndex();
            // test for a partial key.
            if (keyIndex->hasKey(testedKey, index)) {
                foundKey = true;
                break;
            }
        } else {
            // test the full key.
            if (keyIndex->hasKey(testedKey)) {
                foundKey = true;
                break;
            }
        }
    }
    if (!foundKey) {
        if (keyConstraint->hasCustomError()) {
            throwValidationError(keyConstraint->customError(), value->namePath(), value->location());
        }
        throwValidationError(
            u8"This value must refer to an existing key, but no matching entry was found",
            value->namePath(),
            value->location());
    }
}


void DocumentValidator::validateDependencies(const conf::ValuePtr &value, const RulePtr &rule) {
    for (const auto &dependency : rule->dependencyDefinitions()) {
        bool hasSource = false;
        bool hasTarget = false;
        for (const auto &path : dependency->sources()) {
            if (const auto depValue = value->value(path); depValue != nullptr) {
                if (!depValue->isDefaultValue()) {
                    hasSource = true;
                    break;
                }
            }
        }
        for (const auto &path : dependency->targets()) {
            if (const auto depValue = value->value(path); depValue != nullptr) {
                if (!depValue->isDefaultValue()) {
                    hasTarget = true;
                    break;
                }
            }
        }
        if (!dependency->mode().isValid(hasSource, hasTarget)) {
            if (dependency->hasErrorMessage()) {
                throwValidationError(dependency->errorMessage(), value->namePath(), value->location());
            }

            String message;
            switch (dependency->mode().raw()) {
            case DependencyMode::If:
                message = u8format(
                    u8"If {} is configured, you must also configure {}",
                    errorNamePathsOr(dependency->sources(), false),
                    errorNamePathsOr(dependency->targets(), false));
                break;
            case DependencyMode::IfNot:
                message = u8format(
                    u8"If {} is configured, you must {}",
                    errorNamePathsOr(dependency->sources(), false),
                    errorNamePathsOr(dependency->targets(), true));
                break;
            case DependencyMode::OR: {
                auto allNamePaths = dependency->sources();
                allNamePaths.insert(allNamePaths.end(), dependency->targets().begin(), dependency->targets().end());
                message = u8format(
                    u8"You must configure {}",
                    errorNamePathsOr(allNamePaths, false));
                break;
            }
            case DependencyMode::XOR:
                message = u8format(
                    u8"You must either configure {} or configure {}",
                    errorNamePathsOr(dependency->sources(), false),
                    errorNamePathsOr(dependency->targets(), false));
                break;
            case DependencyMode::XNOR:
                message = u8format(
                    u8"You must configure {} and configure {}, or none of them",
                    errorNamePathsOr(dependency->sources(), false),
                    errorNamePathsOr(dependency->targets(), false));
                break;
            default:
                message = u8"Unknown dependency mode";
                break;
            }
            throwValidationError(message, value->namePath(), value->location());
        }
    }
}


}
