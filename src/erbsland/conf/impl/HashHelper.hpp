// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstddef>
#include <functional>


namespace erbsland::conf::impl {


/// Combine a hash with the hash of another value.
///
/// @tested HashHelperTest
///
template <typename T>
void hashCombine(std::size_t& seed, const T& value) {
    seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


}

