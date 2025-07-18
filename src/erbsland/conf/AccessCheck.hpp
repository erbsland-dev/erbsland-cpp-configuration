// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "AccessCheckResult.hpp"
#include "AccessSources.hpp"

#include <memory>


namespace erbsland::conf {


class AccessCheck;
using AccessCheckPtr = std::shared_ptr<AccessCheck>;


/// The interface to access check implementations.
///
class AccessCheck {
public:
    virtual ~AccessCheck() = default;

public:
    /// The check function is called for every source, including the initial source that is passed to the
    /// `parse()` function call. You can either grant or deny access to this source. If you deny the access to
    /// the source, the parser will throw an `Error` with `ErrorCategory::Access`.
    ///
    /// Instead of returning `AccessResult::Denied`, you can also throw an `Error` with `ErrorCategory::Access`.
    ///
    /// @param sources The sources that are verified.
    /// @return Return either `AccessResult::Granted` or `AccessResult::Denied`.
    /// @throws Error Alternatively, throw an `Error` with the `ErrorCategory::Access`.
    /// @notest This is part of `Parser`
    ///
    virtual auto check(const AccessSources &sources) -> AccessCheckResult = 0;
};


}

