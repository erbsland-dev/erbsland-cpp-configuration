// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ConstraintAttribute.hpp"
#include "ConstraintOptions.hpp"

#include "../NamePathHelper.hpp"

#include "../../../impl/vr/KeyConstraint.hpp"

#include <utility>
#include <vector>


namespace erbsland::conf::vr::builder {


/// Adds a key-reference constraint to a named key index.
struct Key : ConstraintAttribute {
    explicit Key(const NamePathLike &reference, ConstraintOptions options = {})
        : _references{{reference}}, _options{std::move(options)} {}
    explicit Key(std::vector<NamePathLike> references, ConstraintOptions options = {})
        : _references{std::move(references)}, _options{std::move(options)} {}
    explicit Key(const std::initializer_list<NamePathLike> references, ConstraintOptions options = {})
        : _references{references}, _options{std::move(options)} {}

    void operator()(impl::Rule &rule) override {
        requireRuleTypeForConstraint(rule, u8"key", {vr::RuleType::Text, vr::RuleType::Integer});
        auto references = detail::parseNamePathList(_references);
        auto constraint = std::make_shared<impl::KeyConstraint>(std::move(references));
        _options.addToRule(rule, constraint, u8"key");
    }

    std::vector<NamePathLike> _references;
    ConstraintOptions _options;
};


}
