// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../String.hpp"

#include <format>


namespace erbsland::conf::impl {


// @tested U8FormatTest


#if defined(ERBSLAND_U8_FORMAT_ENABLE)
// Allow to overwrite the detection.
#define ERBSLAND_HAS_U8_FORMAT
#elif defined(ERBSLAND_U8_FORMAT_DISABLE)
// Allow to overwrite the detection.
#undef ERBSLAND_HAS_U8_FORMAT
#elif defined(__clang__)
// At 2024-11-10: No clang compiler has stable support of std::format with u8 strings.
#undef ERBSLAND_HAS_U8_FORMAT
#elif defined(__GNUC__) || defined(__GNUG__)
// 2025-05-20: GCC 13.3 is supporting std::format, but there is no overload working with `char8_t`.
#undef ERBSLAND_HAS_U8_FORMAT
#elif defined(_MSC_VER)
// All MSVC compilers supporting C++20, also support std::format with u8 strings.
#define ERBSLAND_HAS_U8_FORMAT
#else
// When the compiler is not known, assume proper C++20 language support.
#define ERBSLAND_HAS_U8_FORMAT
#endif


template<typename... Args>
[[nodiscard]] inline auto u8format(const char *formatString, Args&&... args) -> String {
#ifdef ERBSLAND_HAS_U8_FORMAT
    auto result = std::format(reinterpret_cast<const char8_t*>(formatString), std::forward<Args>(args)...);
    return result;
#else
    auto formatResult = std::vformat(std::string_view{formatString}, std::make_format_args(args...));
    return {formatResult.begin(), formatResult.end()};
#endif
}

template<typename... Args>
[[nodiscard]] inline auto u8format(const char8_t *formatString, Args&&... args) -> String {
#ifdef ERBSLAND_HAS_U8_FORMAT
    auto result = std::format(formatString, std::forward<Args>(args)...);
    return result;
#else
    auto formatResult = std::vformat(std::string_view{reinterpret_cast<const char*>(formatString)},
                                     std::make_format_args(args...));
    return {formatResult.begin(), formatResult.end()};
#endif
}


}

