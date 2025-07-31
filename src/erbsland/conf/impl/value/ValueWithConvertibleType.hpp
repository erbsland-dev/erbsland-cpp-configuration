// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Value.hpp"

#include "../../Date.hpp"
#include "../../DateTime.hpp"
#include "../../RegEx.hpp"
#include "../../Time.hpp"
#include "../../TimeDelta.hpp"


namespace erbsland::conf::impl {


/// A generic base class for types that have a `toText()` method.
///
/// @tested `ValueTest`
///
template<typename StorageType, ValueType::Enum tValueType>
class ValueWithConvertibleType : public Value {
public:
    template<typename FwdValue>
    explicit ValueWithConvertibleType(FwdValue value) : _value{std::forward<FwdValue>(value)} {}

public:
    [[nodiscard]] auto type() const noexcept -> ValueType override { return tValueType; }
    [[nodiscard]] auto toTextRepresentation() const noexcept -> String override { return _value.toText(); }

protected:
    StorageType _value;
};


/// The value implementation for the `DateTime` type.
///
/// @tested `ValueTest`
///
class DateTimeValue final : public ValueWithConvertibleType<DateTime, ValueType::DateTime> {
public:
    using ValueWithConvertibleType::ValueWithConvertibleType;
    [[nodiscard]] auto asDateTime() const noexcept -> DateTime override { return _value; }
    [[nodiscard]] auto asDateTimeOrThrow() const -> DateTime override { return _value; }
};


/// The value implementation for the `Date` type.
///
/// @tested `ValueTest`
///
class DateValue final : public ValueWithConvertibleType<Date, ValueType::Date> {
public:
    using ValueWithConvertibleType::ValueWithConvertibleType;
    [[nodiscard]] auto asDate() const noexcept -> Date override { return _value; }
    [[nodiscard]] auto asDateOrThrow() const -> Date override { return _value; }
};


/// The value implementation for the `Time` type.
///
/// @tested `ValueTest`
///
class TimeValue final : public ValueWithConvertibleType<Time, ValueType::Time> {
public:
    using ValueWithConvertibleType::ValueWithConvertibleType;
    [[nodiscard]] auto asTime() const noexcept -> Time override { return _value; }
    [[nodiscard]] auto asTimeOrThrow() const -> Time override { return _value; }
};


/// The value implementation for the `TimeDelta` type.
///
/// @tested `ValueTest`
///
class TimeDeltaValue final : public ValueWithConvertibleType<TimeDelta, ValueType::TimeDelta> {
public:
    using ValueWithConvertibleType::ValueWithConvertibleType;
    [[nodiscard]] auto asTimeDelta() const noexcept -> TimeDelta override { return _value; }
    [[nodiscard]] auto asTimeDeltaOrThrow() const -> TimeDelta override { return _value; }
};


/// The value implementation for the `RegEx` type.
///
/// @tested `ValueTest`
///
class RegExValue final : public ValueWithConvertibleType<RegEx, ValueType::RegEx> {
public:
    using ValueWithConvertibleType::ValueWithConvertibleType;
    [[nodiscard]] auto asRegEx() const noexcept -> RegEx override { return _value; }
    [[nodiscard]] auto asRegExOrThrow() const -> RegEx override { return _value; }
};


}

