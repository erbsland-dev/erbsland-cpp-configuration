// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../../../Error.hpp"
#include "../../../impl/vr/VersionMask.hpp"

#include <algorithm>
#include <ranges>
#include <vector>


namespace erbsland::conf::vr::builder {


/// Restricts a rule to specific versions.
struct Version : Attribute {
    explicit Version(std::vector<Integer> versions, const bool isNegated = false)
        : _versions{std::move(versions)}, _isNegated{isNegated} {
    }
    explicit Version(const std::initializer_list<Integer> versions, const bool isNegated = false)
        : _versions{versions}, _isNegated{isNegated} {
    }
    explicit Version(const Integer version, const bool isNegated = false)
        : _versions{version}, _isNegated{isNegated} {
    }

    void operator()(impl::Rule &rule) override {
        auto mask = toVersionMask(_versions);
        if (_isNegated) {
            mask = !mask;
        }
        rule.limitVersionMask(mask);
    }

    [[nodiscard]] static auto toVersionMask(const std::vector<Integer> &versions) -> impl::VersionMask {
        if (versions.empty()) {
            throw conf::Error{ErrorCategory::Validation, u8"The version list must not be empty"};
        }
        std::vector<Integer> uniqueVersions;
        uniqueVersions.reserve(versions.size());
        for (const auto version : versions) {
            if (version < 0) {
                throw conf::Error{ErrorCategory::Validation, u8"Versions must be non-negative integers"};
            }
            if (std::ranges::find(uniqueVersions, version) == uniqueVersions.end()) {
                uniqueVersions.push_back(version);
            }
        }
        return impl::VersionMask::fromIntegers(uniqueVersions);
    }

    std::vector<Integer> _versions;
    bool _isNegated{false};
};


}
