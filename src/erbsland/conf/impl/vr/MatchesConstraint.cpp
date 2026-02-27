// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "MatchesConstraint.hpp"


#include "ValidationError.hpp"


namespace erbsland::conf::impl {


MatchesConstraint::MatchesConstraint(
    [[maybe_unused]] const String &pattern,
    [[maybe_unused]] const bool isVerbose) {
#ifdef ERBSLAND_CONF_VR_RE_STD
    try {
        _regex = std::regex(pattern.toCharString());
    } catch (const std::regex_error &error) {
        throwValidationError(u8format(u8"Invalid regular expression: {}", error.what()));
    }
#else
#ifdef ERBSLAND_CONF_VR_RE_ERBSLAND
    try {
        re::Flags flags = re::Flags{};
        if (isVerbose) {
            flags |= re::Flag::Verbose;
        }
        _regex = re::RegEx::compile(pattern.toCharString(), flags);
    } catch (const re::Error &error) {
        throwValidationError(u8format(u8"Invalid regular expression: {}", error));
    }
#else
    throwValidationError(u8"The 'matches' constraint was disabled in this build");
#endif
#endif
}


void MatchesConstraint::validateText(
    [[maybe_unused]] const ValidationContext &context,
    [[maybe_unused]] const String &value) const {
#ifdef ERBSLAND_CONF_VR_RE_STD
    if (!std::regex_search(value.toCharString(), _regex)) {
        throwValidationError("The text does not match an expected pattern");
    }
#else
#ifdef ERBSLAND_CONF_VR_RE_ERBSLAND
    try {
        re::MatchPtr match;
#ifdef ERBSLAND_RE_USE_U8STRING
        match = _regex->findFirst(value.raw());
#else
        match = _regex->findFirst(value.toCharString());
#endif
        if (match == nullptr) {
            throwValidationError("The text does not match an expected pattern");
        }
    } catch (const re::Error &error) {
        throwValidationError(u8format("The text could not be validated because of an error: {}", error));
    }
#else
    // ignore if disabled.
#endif
#endif
}


auto handleMatchesConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    if (context.rule->type() != vr::RuleType::Text) {
        throwValidationError(u8"The 'matches' constraint can only be used on text rules");
    }
    if (context.node->type() != ValueType::RegEx) {
        throwValidationError(u8"The 'matches' constraint requires a regular expression value");
    }
    const auto regexValue = context.node->asRegEx();
    const auto pattern = regexValue.toText();
    if (pattern.empty()) {
        throwValidationError(u8"The regular expression in 'matches' constraint cannot be empty");
    }
    return std::make_shared<MatchesConstraint>(pattern, regexValue.isMultiLine());
}


}

