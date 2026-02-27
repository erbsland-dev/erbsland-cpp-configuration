// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../../../Error.hpp"
#include "../../../impl/vr/VersionMask.hpp"

#include <limits>


namespace erbsland::conf::vr::builder {


/// Restricts a rule to versions greater than or equal to a minimum.
struct MinimumVersion : Attribute {
    explicit MinimumVersion(const Integer version, const bool isNegated = false)
        : _version{version}, _isNegated{isNegated} {}
    void operator()(impl::Rule &rule) override {
        if (_version < 0) {
            throw conf::Error{ErrorCategory::Validation, u8"The minimum version must be non-negative"};
        }
        auto mask = impl::VersionMask::fromRanges({impl::VersionRange{
            _version, std::numeric_limits<Integer>::max()}});
        if (_isNegated) {
            mask = !mask;
        }
        rule.limitVersionMask(mask);
    }
    Integer _version;
    bool _isNegated{false};
};


}
