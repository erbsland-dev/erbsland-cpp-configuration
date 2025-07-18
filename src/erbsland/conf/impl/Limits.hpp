// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstddef>


namespace erbsland::conf::limits {


// @tested DefaultsTest


/// The maximum size of a configuration document in bytes.
///
constexpr std::size_t maxDocumentSize = 100'000'000; // 100MB

/// The maximum length of lines in bytes.
///
constexpr std::size_t maxLineLength = 4000;

/// The maximum length of a regular name in characters.
///
constexpr std::size_t maxNameLength = 100;

/// The maximum length of text/code/bytes that is embedded in configuration files.
///
constexpr std::size_t maxTextLength = 10'000'000;

/// The maximum number of decimal digits.
///
constexpr std::size_t maxDecimalDigits = 19;

/// The maximum number of hexadecimal digits.
///
constexpr std::size_t maxHexadecimalDigits = 16;

/// The maximum number of binary digits.
///
constexpr std::size_t maxBinaryDigits = 64;

/// The maximum number of name elements in a name path.
///
constexpr std::size_t maxNamePathLength = 10;

/// The maximum depth of nested documents.
///
constexpr std::size_t maxDocumentNesting = 5;

/// The maximum sources accepted as a result of an include directive.
///
constexpr std::size_t maxIncludeSources = 100;


}

