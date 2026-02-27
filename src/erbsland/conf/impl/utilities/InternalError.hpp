// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../Error.hpp"

#include <format>
#include <string_view>


namespace erbsland::conf::impl {


/// Throw an internal error.
/// This dedicated method exists as a convenient breakpoint for debugging.
/// @param message The error message.
template<typename Fwd>
requires std::is_constructible_v<std::string, Fwd>
[[noreturn]] void throwInternalError(Fwd &&message) {
    throw Error{ErrorCategory::Internal, std::forward<Fwd>(message)};
}


/// Require an expression to be true.
/// @param condition The condition that must be true.
inline void require(const bool condition) {
    if (!condition) {
        throwInternalError("Assertion failed");
    }
}

/// Require an expression to be true.
/// @param condition The condition that must be true.
/// @param message The message in case the condition is false.
inline void require(const bool condition, const std::string_view message) {
    if (!condition) {
        throwInternalError(message);
    }
}


// A test that is performed at runtime.
// We perform these tests at places where we want to ensure correctness for safety reasons.
#define ERBSLAND_CONF_REQUIRE_SAFETY(condition, message) ::erbsland::conf::impl::require(condition, message)
// A test that is only performed in debug builds.
// We perform these tests just to get better debugging information, but the program would fail safely otherwise.
#if defined(_DEBUG) || !defined(NDEBUG)
#define ERBSLAND_CONF_REQUIRE_DEBUG(condition, message) ::erbsland::conf::impl::require(condition, message)
#else
#define ERBSLAND_CONF_REQUIRE_DEBUG(condition, message)
#endif


}


