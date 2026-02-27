// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../../../String.hpp"

#include <utility>


namespace erbsland::conf::vr::builder {


/// Sets the descriptive text of a rule.
struct Description : Attribute {
    explicit Description(String description) : _description{std::move(description)} {}
    void operator()(impl::Rule &rule) override {
        rule.setDescription(std::move(_description));
    }
    String _description;
};


}
