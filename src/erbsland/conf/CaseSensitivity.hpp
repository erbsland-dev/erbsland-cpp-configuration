// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>
#include <format>


namespace erbsland::conf {


/// Case sensitivity for character-wise comparisons.
///
/// This enum controls whether Unicode code-point comparisons are performed case-sensitively or using a simple
/// ASCII case-insensitive rule.
///
/// @note Case-insensitive comparison uses ASCII-only mapping and does not implement full Unicode case folding.
///
/// @see String::characterCompare
/// @see String::startsWith
/// @see String::endsWith
///
enum class CaseSensitivity : uint8_t {
    CaseSensitive,     ///< Compare with exact code-point values.
    CaseInsensitive,   ///< Compare using ASCII lowercase mapping.
};


[[nodiscard]] inline auto toString(const CaseSensitivity caseSensitivity) -> std::string_view {
    switch (caseSensitivity) {
        case CaseSensitivity::CaseSensitive: return "case-sensitive";
        case CaseSensitivity::CaseInsensitive: return "case-insensitive";
    }
    return {};
}


}


template <>
struct std::formatter<erbsland::conf::CaseSensitivity> : std::formatter<std::string_view> {
    auto format(const erbsland::conf::CaseSensitivity caseSensitivity, format_context& ctx) const {
        return std::formatter<std::string_view>::format(erbsland::conf::toString(caseSensitivity), ctx);
    }
};
