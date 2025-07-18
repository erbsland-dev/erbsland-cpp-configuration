// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


namespace erbsland::conf::impl {


// These macros are defined to improve the readability of the lexer code by reducing repetitive elements.
// @notest These are tested via various `Lexer` methods.

#undef EL_YIELD
#undef EL_YIELD_FROM
#undef EL_OPTIONAL_YIELD


/// Yield the given token and make try to move it, not copied.
///
#define EL_YIELD(fn_or_token) co_yield std::move(fn_or_token);

/// Create a token by calling `decoder.createToken(...)` and yield it.
///
#define EL_YIELD_TOKEN(...) co_yield std::move(decoder.createToken(__VA_ARGS__));

/// Yield all tokens from the given generator function.
///
/// Expects a function returning `TokenGenerator`.
///
#define EL_YIELD_FROM(fn) for (auto&& token : fn) { co_yield token; }

/// Yield an optional token from a scan function.
///
/// Expects a function returning `std::optional<Token>`.
///
#define EL_YIELD_OPTIONAL(fn) if (auto result = fn; result.has_value()) { co_yield std::move(result).value(); }


}

