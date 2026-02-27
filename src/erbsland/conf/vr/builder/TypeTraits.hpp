// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../Date.hpp"
#include "../../DateTime.hpp"
#include "../../String.hpp"

#include <string>
#include <type_traits>


namespace erbsland::conf::vr::builder {

template<typename ValueType>
auto constexpr IsInteger = std::is_integral_v<ValueType> && !std::is_same_v<ValueType, bool>;

template<typename ValueType>
auto constexpr IsFloat = std::is_floating_point_v<ValueType>;

template<typename ValueType>
auto constexpr IsBoolean = std::is_same_v<ValueType, bool>;

template<typename ValueType>
auto constexpr IsDate = std::is_same_v<ValueType, Date>;

template<typename ValueType>
auto constexpr IsDateTime = std::is_same_v<ValueType, DateTime>;

template<typename ValueType>
auto constexpr IsString = std::is_same_v<ValueType, String>;

template<typename ValueType>
auto constexpr IsCString = std::is_same_v<ValueType, const char *>;

template<typename ValueType>
auto constexpr IsStdString = std::is_same_v<ValueType, std::string>;

template<typename ValueType>
auto constexpr IsStringLike = IsString<ValueType> || IsCString<ValueType> || IsStdString<ValueType>;

template<typename ValueType>
auto constexpr IsIntegerPair = std::is_same_v<ValueType, std::pair<Integer, Integer>>;


}
