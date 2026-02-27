// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ConstraintAttribute.hpp"
#include "ConstraintOptions.hpp"

#include "../../../impl/vr/MatchesConstraint.hpp"

#include <string>
#include <string_view>
#include <utility>


namespace erbsland::conf::vr::builder {


/// Adds a regular-expression constraint for text values.
struct Matches : ConstraintAttribute {
    explicit Matches(const String &pattern, const bool isVerbose = false, ConstraintOptions options = {})
        : _pattern{pattern}, _isVerbose{isVerbose}, _options{std::move(options)} {}
    explicit Matches(const char *pattern, const bool isVerbose = false, ConstraintOptions options = {})
        : _pattern{String::fromCharString(std::string_view{pattern})},
          _isVerbose{isVerbose},
          _options{std::move(options)} {
    }
    explicit Matches(const std::string &pattern, const bool isVerbose = false, ConstraintOptions options = {})
        : _pattern{String::fromCharString(pattern)}, _isVerbose{isVerbose}, _options{std::move(options)} {}
    explicit Matches(const RegEx &pattern, ConstraintOptions options = {})
        : _pattern{pattern.toText()},
          _isVerbose{pattern.isMultiLine()},
          _options{std::move(options)} {
    }

    void operator()(impl::Rule &rule) override {
        requireRuleTypeForConstraint(rule, u8"matches", {vr::RuleType::Text});
        if (_pattern.empty()) {
            throwValidationError(u8"The regular expression in 'matches' constraint cannot be empty");
        }
        auto constraint = std::make_shared<impl::MatchesConstraint>(_pattern, _isVerbose);
        _options.addToRule(rule, constraint, u8"matches");
    }

    String _pattern;
    bool _isVerbose{false};
    ConstraintOptions _options;
};


}
