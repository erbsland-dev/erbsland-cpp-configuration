// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../../../CaseSensitivity.hpp"


namespace erbsland::conf::vr::builder {


/// Sets the case sensitivity used by the rule.
struct CaseSensitive : Attribute {
    explicit CaseSensitive(const CaseSensitivity caseSensitivity = CaseSensitivity::CaseSensitive)
        : _caseSensitivity{caseSensitivity} {
    }
    void operator()(impl::Rule &rule) override {
        rule.setCaseSensitivity(_caseSensitivity);
    }
    CaseSensitivity _caseSensitivity{CaseSensitivity::CaseSensitive};
};


}
