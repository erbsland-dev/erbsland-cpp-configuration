// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ConstraintAttribute.hpp"
#include "ConstraintOptions.hpp"

#include "../TypeTraits.hpp"

#include "../../../impl/vr/MinMaxConstraint.hpp"

#include <type_traits>
#include <utility>
#include <variant>


namespace erbsland::conf::vr::builder {


/// Adds a minimum boundary constraint for numeric or temporal values.
struct Minimum : ConstraintAttribute {
    using Value = std::variant<Integer, Float, Date, DateTime, std::pair<Integer, Integer>>;

    template<typename TValue>
    requires (IsInteger<TValue>)
    explicit Minimum(const TValue value, ConstraintOptions options = {})
        : _value{static_cast<Integer>(value)}, _options{std::move(options)} {
    }
    template<typename TValue>
    requires (IsFloat<TValue>)
    explicit Minimum(const TValue value, ConstraintOptions options = {})
        : _value{static_cast<Float>(value)}, _options{std::move(options)} {
    }
    explicit Minimum(const Date &value, ConstraintOptions options = {}) : _value{value}, _options{std::move(options)} {}
    explicit Minimum(const DateTime &value, ConstraintOptions options = {}) : _value{value}, _options{std::move(options)} {}
    explicit Minimum(const std::pair<Integer, Integer> value, ConstraintOptions options = {})
        : _value{value}, _options{std::move(options)} {}
    Minimum(const Integer first, const Integer second, ConstraintOptions options = {})
        : _value{std::pair<Integer, Integer>{first, second}}, _options{std::move(options)} {}

    void operator()(impl::Rule &rule) override {
        auto constraint = std::visit(
            [&rule](const auto &value) -> impl::ConstraintPtr {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, Integer>) {
                    requireRuleTypeForConstraint(rule, u8"minimum", {
                        vr::RuleType::Integer,
                        vr::RuleType::Text,
                        vr::RuleType::Bytes,
                        vr::RuleType::ValueList,
                        vr::RuleType::Section,
                        vr::RuleType::SectionList,
                        vr::RuleType::SectionWithTexts});
                    return std::make_shared<impl::MinMaxIntegerConstraint>(impl::MinMaxConstraint::Min, value);
                } else if constexpr (std::is_same_v<T, Float>) {
                    requireRuleTypeForConstraint(rule, u8"minimum", {vr::RuleType::Float});
                    return std::make_shared<impl::MinMaxFloatConstraint>(impl::MinMaxConstraint::Min, value);
                } else if constexpr (std::is_same_v<T, Date>) {
                    requireRuleTypeForConstraint(rule, u8"minimum", {vr::RuleType::Date});
                    return std::make_shared<impl::MinMaxDateConstraint>(impl::MinMaxConstraint::Min, value);
                } else if constexpr (std::is_same_v<T, DateTime>) {
                    requireRuleTypeForConstraint(rule, u8"minimum", {vr::RuleType::DateTime});
                    return std::make_shared<impl::MinMaxDateTimeConstraint>(impl::MinMaxConstraint::Min, value);
                } else {
                    requireRuleTypeForConstraint(rule, u8"minimum", {vr::RuleType::ValueMatrix});
                    return std::make_shared<impl::MinMaxMatrixConstraint>(
                        impl::MinMaxConstraint::Min, value.first, value.second);
                }
            },
            _value);
        _options.addToRule(rule, constraint, u8"minimum");
    }

    Value _value;
    ConstraintOptions _options;
};


}
