// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../Error.hpp"
#include "../../Integer.hpp"
#include "../../vr/RuleType.hpp"

#include <set>


namespace erbsland::conf::impl {


class Rule;
using RulePtr = std::shared_ptr<Rule>;
class Value;
using ValuePtr = std::shared_ptr<Value>;


/// Throw a validation error.
template<typename Msg, typename... Args>
[[noreturn]] void throwValidationError(Msg &&message, Args&&... args) {
    throw Error(
        ErrorCategory::Validation,
        std::forward<Msg>(message),
        std::forward<Args>(args)...);
}

/// Create a text with possible types.
[[nodiscard]] auto expectedRuleTypesText(const std::vector<vr::RuleType> &ruleTypes) -> String;

/// Create an error message string with the expected value types from a rule.
/// @param rule The rule.
/// @param version The version of the document.
/// @return The textual representation of the expected value type.
[[nodiscard]] auto expectedValueTypeText(const RulePtr &rule, Integer version) -> String;

/// Throw an error message when we got a value of an unexpected type.
/// @param rule The rule.
/// @param value The value with the unexpected type.
/// @param version The version of the document.
[[noreturn]] void throwExpectedVsActual(const RulePtr &rule, const ValuePtr &value, Integer version);


}

