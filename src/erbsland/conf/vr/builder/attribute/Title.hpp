// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../../../String.hpp"

#include <utility>


namespace erbsland::conf::vr::builder {


/// Sets the user-facing title of a rule.
struct Title : Attribute {
    explicit Title(String title) : _title{std::move(title)} {}
    void operator()(impl::Rule &rule) override {
        rule.setTitle(std::move(_title));
    }
    String _title;
};


}
