// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "StringPartConstraint.hpp"

#include "../../../impl/vr/StringPartConstraint.hpp"

#include <string>
#include <utility>
#include <vector>


namespace erbsland::conf::vr::builder {


/// Adds an ends-with text constraint.
struct Ends final : StringPartConstraint<impl::EndsConstraint> {
    explicit Ends(std::vector<String> values, ConstraintOptions options = {})
        : StringPartConstraint(std::move(values), std::move(options)) {
        _name = u8"ends";
    }
    explicit Ends(const String &value, ConstraintOptions options = {}) : StringPartConstraint(value, std::move(options)) {
        _name = u8"ends";
    }
    explicit Ends(const char *value, ConstraintOptions options = {}) : StringPartConstraint(value, std::move(options)) {
        _name = u8"ends";
    }
    explicit Ends(const std::string &value, ConstraintOptions options = {}) : StringPartConstraint(value, std::move(options)) {
        _name = u8"ends";
    }
    explicit Ends(const std::initializer_list<String> values, ConstraintOptions options = {})
        : StringPartConstraint(values, std::move(options)) {
        _name = u8"ends";
    }
    explicit Ends(const std::initializer_list<const char *> values, ConstraintOptions options = {})
        : StringPartConstraint(values, std::move(options)) {
        _name = u8"ends";
    }
};


}
