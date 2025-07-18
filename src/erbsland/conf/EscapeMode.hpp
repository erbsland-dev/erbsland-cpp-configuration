// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>
#include <format>


namespace erbsland::conf {


/// Escaping modes.
///
/// @notest Tested via `Char` and `String`.
///
enum class EscapeMode : uint8_t {
    /// Escaping for double-quoted text.
    ///
    /// See reference documentation, chapter Text.
    /// Even allowed, the tab character is escaped as well.
    /// Escape characters U+0000-U+001F, `\\`, `"`, U+007F
    /// Use short formats for `\\\\`, `\\"`, `\\n`, `\\r`, `\\t`.
    /// Everything else as `\\u{x}`.
    ///
    Text,

    /// Full text name escaping.
    ///
    /// See reference documentation, chapter "Parser-Specific Usage of Text Names".
    /// Also mentioned in the specification for test adapters.
    /// Escape characters U+0000-U+001F, `\`, `"`, `.`, `=`, U+007F-...
    /// Escape all characters in `\u{X}` format.
    ///
    FullTextName,

    /// Full test adapter escaping.
    ///
    FullTestAdapter = FullTextName,

    /// Escape for error output and log messages.
    ///
    /// Escapes all Unicode code points that may disrupt the display or have unexpected side effects.
    /// Escapes all control codes.
    /// Escapes backslash and double-quote.
    /// Use short formats for `\\\\`, `\\"`, `\\n`, `\\r`, `\\t`.
    /// Everything else as `\\u{x}`.
    ///
    ErrorText,
};


/// Convert the `EscapeMode` into text.
///
/// @notest Only used in development builds.
///
[[nodiscard]] constexpr auto toStringView(const EscapeMode mode) noexcept -> std::string_view {
    switch (mode) {
    case EscapeMode::Text:
        return "Text";
    case EscapeMode::FullTextName:
        return "FullTextName";
    case EscapeMode::ErrorText:
        return "ErrorText";
    }
    return {};
}


}


template <>
struct std::formatter<erbsland::conf::EscapeMode> : std::formatter<std::string_view> {
    auto format(const erbsland::conf::EscapeMode &mode, format_context& ctx) const {
        return std::formatter<std::string_view>::format(toStringView(mode), ctx);
    }
};

