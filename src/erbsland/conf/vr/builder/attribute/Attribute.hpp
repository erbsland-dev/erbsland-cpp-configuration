// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../../Error.hpp"
#include "../../../impl/vr/Rule.hpp"


namespace erbsland::conf::vr::builder {


/// Base interface for all rule builder attributes.
struct Attribute {
    virtual ~Attribute() = default;
    virtual void operator()(impl::Rule &rule) = 0;

protected:
    [[noreturn]] static void throwValidationError(const String &message) {
        throw conf::Error{ErrorCategory::Validation, message};
    }
};


}
