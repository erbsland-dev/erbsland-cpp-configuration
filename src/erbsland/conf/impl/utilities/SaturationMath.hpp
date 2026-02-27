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


/// Convert an integer type into another one but make sure the result will not overflow.
///
/// If e.g. the unsigned 16bit value 0x2000 is casted to a unsigned 8bit value, the result is 0xff, which is the
/// maximum possible for the target type. If a signed 8bit value -10 is cases to a unsigned 8bit value, the result
/// is zero, because this is the smallest possible value for the type.
///
/// @tparam TargetType The target type for the cast.
/// @tparam SourceType The source type to cast.
/// @param value The source value to cast.
/// @return A value as target type.
///
template<typename TargetType, typename SourceType>
constexpr auto saturatingCast(SourceType value) noexcept -> TargetType {
    static_assert(std::is_integral_v<TargetType>);
    static_assert(std::is_integral_v<SourceType>);

    // This code is sadly not very readable. Yet making it constexpr is a huge and measurable gain.
    // The surrounding `if constexpr` structure narrows the conversions down to the actual required checks
    // for a given source and target type. In the end, the code for each case consists of a single `return` statement.
    if constexpr (std::is_same_v<TargetType, SourceType>) {
        // no cast required.
        return value;
    } else if constexpr (std::is_signed_v<SourceType> != std::is_signed_v<TargetType>) {
        if constexpr (std::is_unsigned_v<TargetType>) {
            // source is signed, target unsigned.
            if constexpr (sizeof(TargetType) < sizeof(SourceType)) {
                return
                    (value < 0) ?
                    0 :
                    (
                        (value > static_cast<SourceType>(std::numeric_limits<TargetType>::max())) ?
                        std::numeric_limits<TargetType>::max() :
                        static_cast<TargetType>(value)
                    );
            } else {
                return (value < 0) ? 0 : static_cast<TargetType>(value);
            }
        } else {
            // source is unsigned, target signed.
            if constexpr (sizeof(TargetType) <= sizeof(SourceType)) {
                return
                    (value > static_cast<SourceType>(std::numeric_limits<TargetType>::max())) ?
                    std::numeric_limits<TargetType>::max() :
                    static_cast<TargetType>(value);
            } else {
                return static_cast<TargetType>(value);
            }
        }
    } else if constexpr (sizeof(TargetType) < sizeof(SourceType)) {
        // target is smaller than source.
        if constexpr (std::is_signed_v<TargetType>) {
            return
                (value < static_cast<SourceType>(std::numeric_limits<TargetType>::min())) ?
                std::numeric_limits<TargetType>::min() :
                (
                    (value > static_cast<SourceType>(std::numeric_limits<TargetType>::max())) ?
                    std::numeric_limits<TargetType>::max() :
                    static_cast<TargetType>(value)
                );
        } else {
            return
                (value > static_cast<SourceType>(std::numeric_limits<TargetType>::max())) ?
                std::numeric_limits<TargetType>::max() :
                static_cast<TargetType>(value);
        }
    } else {
        // target is larger than source.
        return static_cast<TargetType>(value);
    }
}


/// Check if a saturating cast will overflow.
///
/// @tparam TargetType The target type for the cast.
/// @tparam SourceType The source type to cast.
/// @param value The source value to check for an overflow.
/// @return `true` if the case would overflow and change `value`.
///
template<typename TargetType, typename SourceType>
constexpr auto willSaturatingCastOverflow(SourceType value) noexcept -> bool {
    static_assert(std::is_integral_v<TargetType>);
    static_assert(std::is_integral_v<SourceType>);

    if constexpr (std::is_signed_v<SourceType> != std::is_signed_v<TargetType>) {
        if constexpr (std::is_unsigned_v<TargetType>) { // source is signed, target unsigned.
            if constexpr (sizeof(SourceType) > sizeof(TargetType)) {
                return value < 0 || value > static_cast<SourceType>(std::numeric_limits<TargetType>::max());
            } else {
                return value < 0;
            }
        } else { // source is unsigned, target signed.
            if constexpr (sizeof(SourceType) >= sizeof(TargetType)) {
                return value > static_cast<SourceType>(std::numeric_limits<TargetType>::max());
            } else {
                return false; // if target is larger than source, it will never overflow.
            }
        }
    } else {
        if constexpr (sizeof(SourceType) > sizeof(TargetType)) {
            if constexpr (std::is_signed_v<TargetType>) {
                return (value < static_cast<SourceType>(std::numeric_limits<TargetType>::min())) ||
                    (value > static_cast<SourceType>(std::numeric_limits<TargetType>::max()));
            } else {
                return (value > static_cast<SourceType>(std::numeric_limits<TargetType>::max()));
            }
        } else {
            return false; // if target is larger or equal than source, it will never overflow.
        }
    }
}


}


