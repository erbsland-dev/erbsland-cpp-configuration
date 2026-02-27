// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../Bytes.hpp"
#include "../../Date.hpp"
#include "../../DateTime.hpp"
#include "../../Float.hpp"
#include "../../Integer.hpp"
#include "../../String.hpp"
#include "../../Time.hpp"
#include "../../TimeDelta.hpp"

#include <variant>


namespace erbsland::conf::impl {


/// A placeholder type to signal that the token has no value.
///
struct NoContent {};

/// A variant used to store the contents of a value.
///
using Content = std::variant<NoContent, Integer, bool, Float, String, Date, Time, DateTime, Bytes, TimeDelta>;


}

