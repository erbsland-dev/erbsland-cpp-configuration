// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ConstraintAttribute.hpp"
#include "ConstraintOptions.hpp"

#include "../TypeTraits.hpp"

#include "../../../impl/vr/EqualsConstraint.hpp"

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>


namespace erbsland::conf::vr::builder {


/// Adds an equality constraint for scalar values or matrix size.
struct Equals : ConstraintAttribute {
    using Value = std::variant<Integer, bool, Float, String, Bytes, std::pair<Integer, Integer>>;

    template<typename TValue>
    requires (IsInteger<TValue>)
    explicit Equals(const TValue value, ConstraintOptions options = {})
        : _value{static_cast<Integer>(value)}, _options{std::move(options)} {
    }
    explicit Equals(const bool value, ConstraintOptions options = {}) : _value{value}, _options{std::move(options)} {}
    template<typename TValue>
    requires (IsFloat<TValue>)
    explicit Equals(const TValue value, ConstraintOptions options = {})
        : _value{static_cast<Float>(value)}, _options{std::move(options)} {
    }
    explicit Equals(const String &value, ConstraintOptions options = {}) : _value{value}, _options{std::move(options)} {}
    explicit Equals(const char *value, ConstraintOptions options = {})
        : _value{String::fromCharString(std::string_view{value})}, _options{std::move(options)} {}
    explicit Equals(const std::string &value, ConstraintOptions options = {})
        : _value{String::fromCharString(value)}, _options{std::move(options)} {}
    explicit Equals(const Bytes &value, ConstraintOptions options = {}) : _value{value}, _options{std::move(options)} {}
    explicit Equals(const std::pair<Integer, Integer> value, ConstraintOptions options = {})
        : _value{value}, _options{std::move(options)} {}
    Equals(const Integer first, const Integer second, ConstraintOptions options = {})
        : _value{std::pair<Integer, Integer>{first, second}}, _options{std::move(options)} {}

    void operator()(impl::Rule &rule) override {
        auto constraint = std::visit(
            [&rule](const auto &value) -> impl::ConstraintPtr {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, Integer>) {
                    requireRuleTypeForConstraint(rule, u8"equals", {
                        vr::RuleType::Integer,
                        vr::RuleType::Text,
                        vr::RuleType::Bytes,
                        vr::RuleType::ValueList,
                        vr::RuleType::Section,
                        vr::RuleType::SectionList,
                        vr::RuleType::SectionWithTexts});
                    return std::make_shared<impl::EqualsIntegerConstraint>(value);
                } else if constexpr (std::is_same_v<T, bool>) {
                    requireRuleTypeForConstraint(rule, u8"equals", {vr::RuleType::Boolean});
                    return std::make_shared<impl::EqualsBooleanConstraint>(value);
                } else if constexpr (std::is_same_v<T, Float>) {
                    requireRuleTypeForConstraint(rule, u8"equals", {vr::RuleType::Float});
                    return std::make_shared<impl::EqualsFloatConstraint>(value);
                } else if constexpr (std::is_same_v<T, String>) {
                    requireRuleTypeForConstraint(rule, u8"equals", {vr::RuleType::Text});
                    return std::make_shared<impl::EqualsTextConstraint>(value);
                } else if constexpr (std::is_same_v<T, Bytes>) {
                    requireRuleTypeForConstraint(rule, u8"equals", {vr::RuleType::Bytes});
                    return std::make_shared<impl::EqualsBytesConstraint>(value);
                } else {
                    requireRuleTypeForConstraint(rule, u8"equals", {vr::RuleType::ValueMatrix});
                    return std::make_shared<impl::EqualsMatrixConstraint>(value.first, value.second);
                }
            },
            _value);
        _options.addToRule(rule, constraint, u8"equals");
    }

    Value _value;
    ConstraintOptions _options;
};


}
