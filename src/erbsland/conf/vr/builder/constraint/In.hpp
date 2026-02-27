// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ConstraintAttribute.hpp"
#include "ConstraintOptions.hpp"

#include "../TypeTraits.hpp"

#include "../../../impl/utf8/U8Format.hpp"
#include "../../../impl/vr/InConstraint.hpp"

#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


namespace erbsland::conf::vr::builder {


/// Adds an inclusion constraint for a list of values.
struct In : ConstraintAttribute {
    using ValueList = std::variant<std::vector<Integer>, std::vector<Float>, std::vector<String>, std::vector<Bytes>>;

    explicit In(std::vector<Integer> values, ConstraintOptions options = {})
        : _values{std::move(values)}, _options{std::move(options)} {}
    explicit In(std::vector<Float> values, ConstraintOptions options = {})
        : _values{std::move(values)}, _options{std::move(options)} {}
    explicit In(std::vector<String> values, ConstraintOptions options = {})
        : _values{std::move(values)}, _options{std::move(options)} {}
    explicit In(std::vector<Bytes> values, ConstraintOptions options = {})
        : _values{std::move(values)}, _options{std::move(options)} {}

    explicit In(const std::initializer_list<Integer> values, ConstraintOptions options = {})
        : In(std::vector<Integer>{values}, std::move(options)) {}
    explicit In(const std::initializer_list<Float> values, ConstraintOptions options = {})
        : In(std::vector<Float>{values}, std::move(options)) {}
    explicit In(const std::initializer_list<String> values, ConstraintOptions options = {})
        : In(std::vector<String>{values}, std::move(options)) {}
    explicit In(const std::initializer_list<Bytes> values, ConstraintOptions options = {})
        : In(std::vector<Bytes>{values}, std::move(options)) {}
    explicit In(const std::initializer_list<const char *> values, ConstraintOptions options = {})
        : _values{toTextList(values)}, _options{std::move(options)} {}

    template<typename TValue>
    requires (IsInteger<TValue>)
    explicit In(const TValue value, ConstraintOptions options = {})
        : In(std::vector<Integer>{static_cast<Integer>(value)}, std::move(options)) {
    }
    template<typename TValue>
    requires (IsFloat<TValue>)
    explicit In(const TValue value, ConstraintOptions options = {})
        : In(std::vector<Float>{static_cast<Float>(value)}, std::move(options)) {
    }
    explicit In(const String &value, ConstraintOptions options = {}) : In(std::vector<String>{value}, std::move(options)) {}
    explicit In(const char *value, ConstraintOptions options = {})
        : In(std::vector<String>{String::fromCharString(std::string_view{value})}, std::move(options)) {}
    explicit In(const Bytes &value, ConstraintOptions options = {}) : In(std::vector<Bytes>{value}, std::move(options)) {}

    void operator()(impl::Rule &rule) override {
        auto constraint = std::visit(
            [&rule](const auto &values) -> impl::ConstraintPtr {
                using T = std::decay_t<decltype(values)>;
                if (values.empty()) {
                    throwValidationError(impl::u8format(
                        u8"The 'in' constraint must specify a single {} value or a list of {} values",
                        rule.type().toText(),
                        rule.type().toText()));
                }
                if constexpr (std::is_same_v<T, std::vector<Integer>>) {
                    requireRuleTypeForConstraint(rule, u8"in", {vr::RuleType::Integer});
                    if (impl::InIntegerConstraint::hasDuplicate(values, rule.caseSensitivity())) {
                        throwValidationError(u8"The 'in' list must not contain duplicate values");
                    }
                    return std::make_shared<impl::InIntegerConstraint>(values);
                } else if constexpr (std::is_same_v<T, std::vector<Float>>) {
                    requireRuleTypeForConstraint(rule, u8"in", {vr::RuleType::Float});
                    if (impl::InFloatConstraint::hasDuplicate(values, rule.caseSensitivity())) {
                        throwValidationError(u8"The 'in' list must not contain duplicate values");
                    }
                    return std::make_shared<impl::InFloatConstraint>(values);
                } else if constexpr (std::is_same_v<T, std::vector<String>>) {
                    requireRuleTypeForConstraint(rule, u8"in", {vr::RuleType::Text});
                    if (impl::InTextConstraint::hasDuplicate(values, rule.caseSensitivity())) {
                        throwValidationError(u8"The 'in' list must not contain duplicate values");
                    }
                    return std::make_shared<impl::InTextConstraint>(values);
                } else {
                    requireRuleTypeForConstraint(rule, u8"in", {vr::RuleType::Bytes});
                    if (impl::InBytesConstraint::hasDuplicate(values, rule.caseSensitivity())) {
                        throwValidationError(u8"The 'in' list must not contain duplicate values");
                    }
                    return std::make_shared<impl::InBytesConstraint>(values);
                }
            },
            _values);
        _options.addToRule(rule, constraint, u8"in");
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

    ValueList _values;
    ConstraintOptions _options;
};


}
