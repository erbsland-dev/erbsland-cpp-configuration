// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::conf::impl {


/// Character classes to simplify lexer tests.
///
/// @notest Tested with the `Char` tests.
///
enum class CharClass : uint8_t {
    Spacing,                ///< Space or tab.
    LineBreak,              ///< Carriage return or newline.
    NameStart,              ///< Start of a name (a-z, @ or double quote).
    Letter,                 ///< A letter (a-z).
    LetterOrDigit,          ///< Either a letter (a-z) or digit (0-9).
    DecimalDigit,           ///< A decimal digit (0-9).
    HexDigit,               ///< A hexadecimal digit (0-9, a-f, A-F).
    NameValueSeparator,     ///< A value separator (either : or =).
    OpeningBracket,         ///< Any opening bracket of the language (", `, /, <).
    SectionStart,           ///< Any character that may start a section (- *, [).
    EndOfLineStart,         ///< Either a space, hash (comment) or line-break character.
    LetterA,                ///< The latter A, case-insensitive
    LetterB,                ///< The latter B, case-insensitive
    LetterF,                ///< The latter F, case-insensitive
    LetterI,                ///< The latter I, case-insensitive
    LetterN,                ///< The latter N, case-insensitive
    LetterT,                ///< The latter T, case-insensitive
    LetterX,                ///< The latter X, case-insensitive
    LetterZ,                ///< The latter Z, case-insensitive
    NumberStart,            ///< Digit `0`-`9` or `+`/`-`
    TimeStart,              ///< Digit `0`-`9` or `t`/`T`
    FloatLiteralStart,      ///< `+`, `-`, `i`/`I` or `n`/`N`
    ExponentStart,          ///< The letter `e`/`E`
    BinaryDigit,            ///< Digit `0` or `1`
    PlusOrMinus,            ///< `+` or `-`
    SectionNameStart,       ///< `a-z`, `A-Z` or `"`
    FormatIdentifierChar,   ///< `a-z`, `A-Z`, `0-9`, `_` or `-`
    IntegerSuffixChar,      ///< `a-z`, `A-Z`, or `µ`
    LineBreakOrEnd,         ///< new-line, carriage-return or end-of-data
    ValidAfterValue,        ///< tab, space, new-line, carriage-return, `#`, end-of-data, `,`
    ValidLang,              ///< All valid characters for a configuration document.
    EscapedForText,         ///< Characters that should be escaped in text.
    EscapedForTextName,     ///< Characters that should be escaped in text-names.
    EscapedForErrors,       ///< Characters that aren't safe for log and error messages.
    FilePathSeparator,      ///< `/` or `\`
    InvalidWindowsServerName, ///< All invalid characters for a Windows server name in a UNC path.
};


}

