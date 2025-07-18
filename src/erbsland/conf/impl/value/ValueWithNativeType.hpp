// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Value.hpp"

#include "../utf8/U8Format.hpp"


/// A generic template to implement value for native types.
///
/// @tested `ValueTest`
///
namespace erbsland::conf::impl {
template<typename StorageType, ValueType::Enum tValueType>
class ValueWithNativeType : public Value {
public:
    template<typename Fwd>
    explicit ValueWithNativeType(Fwd value) : _value{std::forward<Fwd>(value)} {}

public:
    [[nodiscard]] auto type() const noexcept -> ValueType override { return tValueType; }
    [[nodiscard]] auto toTextRepresentation() const noexcept -> String override {
        if constexpr (std::is_same_v<StorageType, String>) {
            return _value;
        } else {
            return impl::u8format("{}", _value);
        }
    }

protected:
    StorageType _value;
};


/// The value implementation for the `Boolean` type.
///
/// @tested `ValueTest`
///
class BooleanValue final : public ValueWithNativeType<bool, ValueType::Boolean> {
public:
    using ValueWithNativeType::ValueWithNativeType;
    [[nodiscard]] auto toBoolean() const noexcept -> bool override { return _value; }
    [[nodiscard]] auto toTestText() const noexcept -> String override {
        return u8format(u8"{}({})", type(), _value);
    }
};


/// The value implementation for the `Float` type.
///
/// @tested `ValueTest`
///
class FloatValue final : public ValueWithNativeType<Float, ValueType::Float> {
public:
    using ValueWithNativeType::ValueWithNativeType;
    [[nodiscard]] auto toFloat() const noexcept -> Float override { return _value; }
    [[nodiscard]] auto toTestText() const noexcept -> String override {
        return u8format(u8"{}({})", type(), _value);
    }
};


/// The value implementation for the `Integer` type.
///
/// @tested `ValueTest`
///
class IntegerValue final : public ValueWithNativeType<Integer, ValueType::Integer> {
public:
    using ValueWithNativeType::ValueWithNativeType;
    [[nodiscard]] auto toInteger() const noexcept -> Integer override { return _value; }
    [[nodiscard]] auto toTestText() const noexcept -> String override {
        return u8format(u8"{}({})", type(), _value);
    }
};


/// The value implementation for the `Text` type.
///
/// @tested `ValueTest`
///
class TextValue final : public ValueWithNativeType<String, ValueType::Text> {
public:
    using ValueWithNativeType::ValueWithNativeType;
    [[nodiscard]] auto toText() const noexcept -> String override { return _value; }
    [[nodiscard]] auto toTestText() const noexcept -> String override {
        return u8format(u8"{}(\"{}\")", type(), _value.toEscaped(EscapeMode::FullTestAdapter));
    }
};


/// The value implementation for the `RegEx` type.
///
/// @tested `ValueTest`
///
class RegExValue final : public ValueWithNativeType<String, ValueType::RegEx> {
public:
    using ValueWithNativeType::ValueWithNativeType;
    [[nodiscard]] auto toRegEx() const noexcept -> String override { return _value; }
    [[nodiscard]] auto toTestText() const noexcept -> String override {
        return u8format(u8"{}(\"{}\")", type(), _value.toEscaped(EscapeMode::FullTestAdapter));
    }
};


}

