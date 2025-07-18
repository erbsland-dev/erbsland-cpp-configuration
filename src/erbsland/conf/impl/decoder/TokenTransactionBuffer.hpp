// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "DecodedChar.hpp"

#include <vector>


namespace erbsland::conf::impl {


/// A transaction buffer.
///
/// @tested This is an integral part of `TokenDecoder`.
///
using TokenTransactionBuffer = std::vector<DecodedChar>;


}

