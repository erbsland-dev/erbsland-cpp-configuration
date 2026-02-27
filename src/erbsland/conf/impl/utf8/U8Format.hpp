// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../String.hpp"

#include <format>


namespace erbsland::conf::impl {


// @tested U8FormatTest


template<typename... Args>
[[nodiscard]] inline auto u8format(const char *formatString, Args&&... args) -> String {
    // TODO: Validate if this is working on all platforms
    auto formatResult = std::vformat(
        std::string_view{formatString},
        std::make_format_args(args...));
    return {formatResult.begin(), formatResult.end()};
}

template<typename... Args>
[[nodiscard]] inline auto u8format(const char8_t *formatString, Args&&... args) -> String {
    // TODO: Validate if this is working on all platforms
    auto formatResult = std::vformat(
        std::string_view{reinterpret_cast<const char*>(formatString)},
        std::make_format_args(args...));
    return {formatResult.begin(), formatResult.end()};
}


}

