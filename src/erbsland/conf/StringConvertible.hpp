// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "String.hpp"


namespace erbsland::conf {


/// Concept to check if a type is convertible to one of the supported string types.
template <typename T>
concept StringConvertible = std::is_convertible_v<T, std::string> || std::is_convertible_v<T, std::u8string> ||
    std::is_convertible_v<T, std::string_view> || std::is_convertible_v<T, std::u8string_view>;

/// Concept to check if a type is a string, or convertible to a string.
template <typename T>
concept StringLike = StringConvertible<T> || std::is_same_v<T, String>;


}

