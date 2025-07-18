// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../TokenGenerator.hpp"

#include "../decoder/TokenDecoder.hpp"


namespace erbsland::conf::impl::lexer {


// @tested via Lexer tests


/// An escape function.
///
/// - Takes the target string as only argument.
/// - When called, the escape character is the current character.
/// - Can throw exceptions to indicate syntax errors.
/// - Must leave with the current character is the character following the escape sequence.
///
using EscapeFn = std::function<void (Decoder&, String&)>;

/// Generic function to parse a string.
///
/// - Expects that the current character is the first character in the string.
/// - Raises an error if the data ends, before the terminator character.
/// - Leaves with the current character *after* the terminator.
///
/// @param decoder The decoder to use.
/// @param target The target string where to store the characters.
/// @param terminator The terminating character for this string.
/// @param escapeChar An escape character (use zero if there is no escape character).
/// @param escapeFn The function to handle escape characters.
///
void parseString(
    Decoder &decoder,
    String &target,
    char32_t terminator,
    char32_t escapeChar,
    const EscapeFn &escapeFn);

/// Generic function to parse a multi-line string.
///
/// - Parses a string up to the last character, that is no trailing spacing.
/// - Throws an exception on an early end-of-data situation.
///
/// @param decoder The decoder to use.
/// @param escapeChar An escape character (use zero if there is no escape character).
/// @param escapeFn The function to handle escape characters.
/// @param tokenType The type of token to return.
///
[[nodiscard]] auto parseMultiLineString(
    TokenDecoder &decoder,
    char32_t escapeChar,
    const EscapeFn &escapeFn,
    TokenType tokenType) -> TokenGenerator;

/// Parse regular single line text.
///
/// @param decoder The decoder to use.
/// @param target The string where the parsed text is appended to.
/// @throws Error For any syntax errors in the parsed text.
///
void parseText(Decoder &decoder, String &target);

/// Parse the escape sequence after the backslash character.
///
/// @param decoder The decoder to use.
/// @param target The string where the parsed text is appended to.
/// @throws Error For any syntax errors in the parsed text.
///
void parseTextEscapeSequence(Decoder &decoder, String &target);

/// Parse a single line regular expression text.
///
/// @param decoder The decoder to use.
/// @param target The string where the parsed text is appended to.
/// @throws Error For any syntax errors in the parsed text.
///
void parseRegularExpression(Decoder &decoder, String &target);

/// Parse the escape sequence after the backslash character.
///
/// @param decoder The decoder to use.
/// @param target The string where the parsed text is appended to.
/// @throws Error For any syntax errors in the parsed text.
///
void parseRegularExpressionEscapeSequence(Decoder &decoder, String &target);

/// Parse a single line code text.
///
void parseCode(Decoder &decoder, String &target);


}

