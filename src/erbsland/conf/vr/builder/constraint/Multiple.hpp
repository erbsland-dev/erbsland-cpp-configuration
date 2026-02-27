// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ConstraintAttribute.hpp"
#include "ConstraintOptions.hpp"

#include "../TypeTraits.hpp"

#include "../../../impl/vr/MultipleConstraint.hpp"

#include <cmath>
#include <limits>
#include <type_traits>
#include <utility>
#include <variant>


namespace erbsland::conf::vr::builder {


/// Adds a multiple-of constraint for scalar values or matrix size.
struct Multiple : ConstraintAttribute {
    using Value = std::variant<Integer, Float, std::pair<Integer, Integer>>;

    template<typename TValue>
    requires (IsInteger<TValue>)
    explicit Multiple(const TValue value, ConstraintOptions options = {})
        : _value{static_cast<Integer>(value)}, _options{std::move(options)} {
    }
    template<typename TValue>
    requires (IsFloat<TValue>)
    explicit Multiple(const TValue value, ConstraintOptions options = {})
        : _value{static_cast<Float>(value)}, _options{std::move(options)} {
    }
    explicit Multiple(const std::pair<Integer, Integer> value, ConstraintOptions options = {})
        : _value{value}, _options{std::move(options)} {}
    Multiple(const Integer rows, const Integer columns, ConstraintOptions options = {})
        : _value{std::pair<Integer, Integer>{rows, columns}}, _options{std::move(options)} {}

    void operator()(impl::Rule &rule) override {
        auto constraint = std::visit(
            [&rule](const auto &value) -> impl::ConstraintPtr {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, Integer>) {
                    requireRuleTypeForConstraint(rule, u8"multiple", {
                        vr::RuleType::Integer,
                        vr::RuleType::Text,
                        vr::RuleType::Bytes,
                        vr::RuleType::ValueList,
                        vr::RuleType::Section,
                        vr::RuleType::SectionList,
                        vr::RuleType::SectionWithTexts});
                    if (value == 0) {
                        throwValidationError(u8"The 'multiple' divisor must not be zero");
                    }
                    return std::make_shared<impl::MultipleIntegerConstraint>(value);
                } else if constexpr (std::is_same_v<T, Float>) {
                    requireRuleTypeForConstraint(rule, u8"multiple", {vr::RuleType::Float});
                    if (std::abs(value) <= std::numeric_limits<Float>::epsilon()) {
                        throwValidationError(u8"The 'multiple' divisor must not be zero");
                    }
                    return std::make_shared<impl::MultipleFloatConstraint>(value);
                } else {
                    requireRuleTypeForConstraint(rule, u8"multiple", {vr::RuleType::ValueMatrix});
                    if (value.first == 0 || value.second == 0) {
                        throwValidationError(u8"The 'multiple' divisors must not be zero");
                    }
                    return std::make_shared<impl::MultipleMatrixConstraint>(value.first, value.second);
                }
            },
            _value);
        _options.addToRule(rule, constraint, u8"multiple");
    }

    Value _value;
    ConstraintOptions _options;
};


}
