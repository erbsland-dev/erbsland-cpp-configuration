// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../TokenGenerator.hpp"

#include "../decoder/TokenDecoder.hpp"


namespace erbsland::conf::impl::lexer {


// @tested via Lexer tests


/// Scan the character stream for a single line text/code/regex value.
///
/// @param decoder The decoder.
/// @return A `Text`, `Code` or `RegEx` token if the value was found, nothing if something else was found.
///
[[nodiscard]] auto scanSingleLineText(TokenDecoder &decoder) -> std::optional<LexerToken>;

/// Expect and read multi-line text.
///
/// Expects that the opening bracket token was parsed and the decoder is now at the character
/// just after the opening bracket. Works with regular text, code and regular expressions.
///
/// @param decoder The decoder.
/// @param openTokenType The token of the open bracket to choose the parsing mode.
///     Must be either `MultiLineTextStart`, `MultiLineCodeStart` or `MultiLineRegExStart`.
/// @return A token generator.
///
[[nodiscard]] auto expectMultiLineText(TokenDecoder &decoder, TokenType openTokenType) -> TokenGenerator;


}



