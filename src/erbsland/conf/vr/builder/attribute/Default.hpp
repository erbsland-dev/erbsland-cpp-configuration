// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../DefaultValueFactory.hpp"

#include "../../../impl/utf8/U8Format.hpp"

#include <string>
#include <vector>


namespace erbsland::conf::vr::builder {


/// Assigns a default value to a rule.
struct Default : Attribute {
    explicit Default(const impl::ValuePtr &value) : _value{value} {}
    explicit Default(const Integer value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const bool value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const Float value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const String &value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const char *value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const std::string &value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const Date &value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const Time &value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const DateTime &value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const Bytes &value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const TimeDelta &value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const RegEx &value) : _value{detail::createDefaultValue(value)} {}
    explicit Default(const std::vector<Integer> &values) : _value{detail::createDefaultValue(values)} {}
    explicit Default(const std::vector<bool> &values) : _value{detail::createDefaultValue(values)} {}
    explicit Default(const std::vector<Float> &values) : _value{detail::createDefaultValue(values)} {}
    explicit Default(const std::vector<String> &values) : _value{detail::createDefaultValue(values)} {}
    explicit Default(const std::vector<Bytes> &values) : _value{detail::createDefaultValue(values)} {}
    explicit Default(const std::vector<std::vector<Integer>> &values) : _value{detail::createDefaultValue(values)} {}
    explicit Default(const std::vector<std::vector<Float>> &values) : _value{detail::createDefaultValue(values)} {}

    void operator()(impl::Rule &rule) override {
        if (!rule.type().acceptsDefaults()) {
            throwValidationError(impl::u8format(
                u8"A default value cannot be used for '{}' node rules",
                rule.type().toText()));
        }
        if (!rule.type().matchesValueType(_value->type())) {
            throwValidationError(impl::u8format(
                u8"The default value of a node-rules definition must match its type. Expected {}, but got {}",
                rule.type().expectedValueTypeText(),
                _value->type().toValueDescription(true)));
        }
        rule.setDefaultValue(_value);
    }

    impl::ValuePtr _value;
};


}
