// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../../../impl/utf8/U8Format.hpp"


namespace erbsland::conf::vr::builder {


/// Marks a rule value as secret.
struct IsSecret : Attribute {
    explicit IsSecret(const bool isSecret = true) : _isSecret{isSecret} {}
    void operator()(impl::Rule &rule) override {
        if (_isSecret && !rule.type().isScalar()) {
            throwValidationError(impl::u8format(
                u8"The 'is_secret' marker can only be used for scalar value types. Found {} type",
                rule.type().toText()));
        }
        rule.setSecret(_isSecret);
    }
    bool _isSecret{true};
};


}
