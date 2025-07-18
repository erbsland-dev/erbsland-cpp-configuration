// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Value.hpp"

#include "../../Bytes.hpp"


namespace erbsland::conf::impl {


/// The value implementation for bytes values.
///
/// @tested `ValueTest`
///
class BytesValue final : public Value {
public:
    template<typename FwdData>
    explicit BytesValue(FwdData value) noexcept : _value{std::forward<FwdData>(value)} {}

public:
    [[nodiscard]] auto type() const noexcept -> ValueType override { return ValueType::Bytes; }
    [[nodiscard]] auto toBytes() const noexcept -> Bytes override { return _value; }
    [[nodiscard]] auto toTextRepresentation() const noexcept -> String override;
    [[nodiscard]] auto toTestText() const noexcept -> String override;

private:
    Bytes _value;
};


}

