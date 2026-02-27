// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ConstraintAttribute.hpp"
#include "ConstraintOptions.hpp"

#include "../../../impl/vr/CharsConstraint.hpp"

#include <string>
#include <string_view>
#include <utility>
#include <vector>


namespace erbsland::conf::vr::builder {


/// Adds a character-set constraint for text values.
struct Chars : ConstraintAttribute {
    explicit Chars(std::vector<String> values, ConstraintOptions options = {})
        : _values{std::move(values)}, _options{std::move(options)} {}
    explicit Chars(const String &value, ConstraintOptions options = {}) : _values{{value}}, _options{std::move(options)} {}
    explicit Chars(const char *value, ConstraintOptions options = {})
        : _values{{String::fromCharString(std::string_view{value})}}, _options{std::move(options)} {}
    explicit Chars(const std::string &value, ConstraintOptions options = {}) : _values{{String{value}}}, _options{std::move(options)} {}
    explicit Chars(const std::initializer_list<String> values, ConstraintOptions options = {})
        : _values{values}, _options{std::move(options)} {}
    explicit Chars(const std::initializer_list<const char *> values, ConstraintOptions options = {})
        : _values{toTextList(values)}, _options{std::move(options)} {}

    void operator()(impl::Rule &rule) override {
        requireRuleTypeForConstraint(rule, u8"chars", {vr::RuleType::Text});
        if (_values.empty()) {
            throwValidationError(
                u8"The 'chars' constraint must specify a single text value or a list of texts");
        }
        auto constraint = std::make_shared<impl::CharsConstraint>(_values);
        _options.addToRule(rule, constraint, u8"chars");
    }

private:
    [[nodiscard]] static auto toTextList(const std::initializer_list<const char *> values) -> std::vector<String> {
        std::vector<String> result;
        result.reserve(values.size());
        for (const auto value : values) {
            result.emplace_back(String::fromCharString(std::string_view{value}));
        }
        return result;
    }

    std::vector<String> _values;
    ConstraintOptions _options;
};


}
