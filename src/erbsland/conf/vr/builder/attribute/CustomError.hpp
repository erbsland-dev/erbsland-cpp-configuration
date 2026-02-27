// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../../../String.hpp"

#include <utility>


namespace erbsland::conf::vr::builder {


/// Sets the default validation error message for a rule.
struct CustomError : Attribute {
    explicit CustomError(String errorMessage) : _errorMessage{std::move(errorMessage)} {}
    void operator()(impl::Rule &rule) override {
        rule.setErrorMessage(std::move(_errorMessage));
    }
    String _errorMessage;
};


}
