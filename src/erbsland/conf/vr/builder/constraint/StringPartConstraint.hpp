// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ConstraintAttribute.hpp"
#include "ConstraintOptions.hpp"

#include "../../../impl/utf8/U8Format.hpp"

#include <string>
#include <string_view>
#include <utility>
#include <vector>


namespace erbsland::conf::vr::builder {


/// Internal helper base for string-part constraints.
template<typename TImplConstraint>
struct StringPartConstraint : ConstraintAttribute {
    explicit StringPartConstraint(std::vector<String> values, ConstraintOptions options = {})
        : _values{std::move(values)}, _options{std::move(options)} {}

    explicit StringPartConstraint(const String &value, ConstraintOptions options = {})
        : _values{{value}}, _options{std::move(options)} {}
    explicit StringPartConstraint(const char *value, ConstraintOptions options = {})
        : _values{{String::fromCharString(std::string_view{value})}}, _options{std::move(options)} {}
    explicit StringPartConstraint(const std::string &value, ConstraintOptions options = {})
        : _values{{String::fromCharString(value)}}, _options{std::move(options)} {}
    explicit StringPartConstraint(const std::initializer_list<String> values, ConstraintOptions options = {})
        : _values{values}, _options{std::move(options)} {}
    explicit StringPartConstraint(const std::initializer_list<const char *> values, ConstraintOptions options = {})
        : _values{toTextList(values)}, _options{std::move(options)} {}

    void operator()(impl::Rule &rule) override {
        requireRuleTypeForConstraint(rule, _name, {vr::RuleType::Text});
        if (_values.empty()) {
            throwValidationError(impl::u8format(
                u8"The '{}' constraint must specify a single text value or a list of texts",
                _name));
        }
        auto constraint = std::make_shared<TImplConstraint>(_values);
        _options.addToRule(rule, constraint, _name);
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

protected:
    std::vector<String> _values;
    ConstraintOptions _options;
    String _name;
};


}
