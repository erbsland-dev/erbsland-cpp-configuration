// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::conf {


/// The result of an access check.
///
/// @notest This is part of `Parser`
///
enum class AccessCheckResult : uint8_t {
    /// If the access is granted.
    ///
    Granted,

    /// If the access is denied.
    ///
    Denied,
};


}

