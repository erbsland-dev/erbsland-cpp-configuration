// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <type_traits>


namespace erbsland::conf::impl {


template <class>
inline constexpr bool always_false_v = false;


template<class T>
inline constexpr bool value_get_pass_default_by_ref_v =
    std::is_same_v<T, String> ||
    std::is_same_v<T, Bytes> ||
    std::is_same_v<T, RegEx> ||
    std::is_same_v<T, Time> ||
    std::is_same_v<T, Date> ||
    std::is_same_v<T, DateTime> ||
    std::is_same_v<T, TimeDelta> ||
    std::is_same_v<T, std::string> ||
    std::is_same_v<T, std::u8string>;


template<class T>
using value_get_default_param_t =
    std::conditional_t<value_get_pass_default_by_ref_v<T>, const T&, T>;


}

