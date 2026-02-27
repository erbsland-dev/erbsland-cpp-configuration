// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../../../Error.hpp"
#include "../../../impl/vr/VersionMask.hpp"


namespace erbsland::conf::vr::builder {


/// Restricts a rule to versions smaller than or equal to a maximum.
struct MaximumVersion : Attribute {
    explicit MaximumVersion(const Integer version, const bool isNegated = false)
        : _version{version}, _isNegated{isNegated} {}
    void operator()(impl::Rule &rule) override {
        if (_version < 0) {
            throw conf::Error{ErrorCategory::Validation, u8"The maximum version must be non-negative"};
        }
        auto mask = impl::VersionMask::fromRanges({impl::VersionRange{0, _version}});
        if (_isNegated) {
            mask = !mask;
        }
        rule.limitVersionMask(mask);
    }
    Integer _version;
    bool _isNegated{false};
};


}
