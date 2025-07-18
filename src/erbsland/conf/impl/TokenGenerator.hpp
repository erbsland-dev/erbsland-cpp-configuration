// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Generator.hpp"
#include "LexerToken.hpp"


namespace erbsland::conf::impl {


/// A token generator
///
/// @tested Via `TokenDecoder` class.
///
using TokenGenerator = Generator<LexerToken>;


}


