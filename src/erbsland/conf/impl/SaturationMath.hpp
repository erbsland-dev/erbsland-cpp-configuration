// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <limits>
#include <type_traits>


#ifdef _MSC_VER
#include <safeint.h>
#endif


namespace erbsland::conf::impl {


// Methods copied from ErbslandCore
// @tested SaturationMathTest – verifies the arithmetic overflow helpers.


/// Test if an addition will overflow.
///
/// @tparam T The type for the operands.
/// @param a The first value of the operation to test.
/// @param b The second value of the operation to test.
/// @return `true` if an overflow will occur, `false` otherwise.
///
template<typename T>
auto willAddOverflow(T a, T b) noexcept -> bool {
    static_assert(std::is_integral_v<T>, "This function is only implemented for integer types.");
    // Use reliable built-in functions or stop compilation.
#if __GNUC__ || __clang__
#if __has_builtin(__builtin_add_overflow)
    T result;
    return __builtin_add_overflow(a, b, &result);
#else
#error "Please use a current version of the GCC or clang compiler."
#endif
#elif _MSC_VER
    T result;
    return !msl::utilities::SafeAdd(a, b, result);
#else
#error "Missing saturating add implementation for the compiler you use."
#endif
}


/// Test if a multiplication will overflow.
///
/// @tparam T The type for the operands.
/// @param a The first value of the operation to test.
/// @param b The second value of the operation to test.
/// @return `true` if an overflow will occur, `false` otherwise.
///
template<typename T>
auto willMultiplyOverflow(T a, T b) -> bool {
    static_assert(std::is_integral_v<T>, "This function is only implemented for integer types.");
    // Use reliable built-in functions or stop compilation.
#if __GNUC__ || __clang__
#if __has_builtin(__builtin_add_overflow)
    T result;
    return __builtin_mul_overflow(a, b, &result);
#else
#error "Please use a current version of the GCC or clang compiler."
#endif
#elif _MSC_VER
    T result;
    return !msl::utilities::SafeMultiply(a, b, result);
#else
#error "Missing saturating subtract implementation for the compiler you use."
#endif
}



}


