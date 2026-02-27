// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "LexerToken.hpp"

#include "../utilities/Generator.hpp"


namespace erbsland::conf::impl {


/// A token generator
///
/// @tested Via `TokenDecoder` class.
///
using TokenGenerator = Generator<LexerToken>;


}


