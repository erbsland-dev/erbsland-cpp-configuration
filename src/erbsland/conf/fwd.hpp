// Copyright © 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <cstdint>
#include <memory>
#include <type_traits>


namespace erbsland::conf {


class Value;
using ValuePtr = std::shared_ptr<Value>;
using ConstValuePtr = std::shared_ptr<const Value>;
class Source;
using SourcePtr = std::shared_ptr<Source>;
using SourcePtrConst = std::shared_ptr<const Source>;


}


