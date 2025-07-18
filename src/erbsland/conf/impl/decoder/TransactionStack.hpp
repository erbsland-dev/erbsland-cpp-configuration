// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Transaction.hpp"


namespace erbsland::conf::impl {


/// A transaction stack.
///
/// @tested This is an integral part of `TokenDecoder` and `FastNameDecoder`.
///
using TransactionStack = std::vector<Transaction*>;


}


