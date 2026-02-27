// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "StringPartConstraint.hpp"

#include "../../../impl/vr/StringPartConstraint.hpp"

#include <string>
#include <utility>
#include <vector>


namespace erbsland::conf::vr::builder {


/// Adds a contains text constraint.
struct Contains final : StringPartConstraint<impl::ContainsConstraint> {
    explicit Contains(std::vector<String> values, ConstraintOptions options = {})
        : StringPartConstraint(std::move(values), std::move(options)) {
        _name = u8"contains";
    }
    explicit Contains(const String &value, ConstraintOptions options = {}) : StringPartConstraint(value, std::move(options)) {
        _name = u8"contains";
    }
    explicit Contains(const char *value, ConstraintOptions options = {})
        : StringPartConstraint(value, std::move(options)) {
        _name = u8"contains";
    }
    explicit Contains(const std::string &value, ConstraintOptions options = {})
        : StringPartConstraint(value, std::move(options)) {
        _name = u8"contains";
    }
    explicit Contains(const std::initializer_list<String> values, ConstraintOptions options = {})
        : StringPartConstraint(values, std::move(options)) {
        _name = u8"contains";
    }
    explicit Contains(const std::initializer_list<const char *> values, ConstraintOptions options = {})
        : StringPartConstraint(values, std::move(options)) {
        _name = u8"contains";
    }
};


}
