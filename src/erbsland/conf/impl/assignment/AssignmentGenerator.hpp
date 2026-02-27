// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Assignment.hpp"

#include "../utilities/Generator.hpp"


namespace erbsland::conf::impl {


/// The generator of the assignment stream.
///
/// @notest This is tested via test for the `AssignmentStream` class.
///
using AssignmentGenerator = Generator<Assignment>;


}

