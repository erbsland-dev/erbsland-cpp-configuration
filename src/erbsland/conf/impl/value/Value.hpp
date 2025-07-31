// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Container.hpp"

#include "../Definitions.hpp"

#include <utility>
#include <vector>


namespace erbsland::conf::impl {


class Value;
using ValuePtr = std::shared_ptr<Value>;
using ConstValuePtr = std::shared_ptr<const Value>;


/// Internal implementation of the public `conf::Value` interface.
///
/// The parser creates instances of this class and its derived types while building the value tree for a
/// configuration document.
///
/// @tested `ValueTest`
///
class Value : public conf::Value, public Container {
public:
    // defaults
    Value() = default;
    ~Value() override = default;

    // disable copy and assign.
    Value(const Value &) = delete;
    auto operator=(const Value &) -> Value& = delete;
    Value(Value &&) = delete;
    auto operator=(Value &&) -> Value& = delete;

public:
    [[nodiscard]] auto name() const noexcept -> Name override;
    [[nodiscard]] auto namePath() const noexcept -> NamePath override;
    [[nodiscard]] auto hasParent() const noexcept -> bool override;
    [[nodiscard]] auto parent() const noexcept -> conf::ValuePtr override;
    [[nodiscard]] auto size() const noexcept -> size_t override { return 0; }
    [[nodiscard]] auto hasValue(const NamePathLike &namePath) const noexcept -> bool override { return false; }
    [[nodiscard]] auto value(const NamePathLike &namePath) const noexcept -> conf::ValuePtr override {
        return {};
    }
    [[nodiscard]] auto valueOrThrow(const NamePathLike &namePath) const -> conf::ValuePtr override {
        throwValueNotFound(*this, namePath);
    }
    [[nodiscard]] auto begin() const noexcept -> ValueIterator override { return {}; }
    [[nodiscard]] auto end() const noexcept -> ValueIterator override { return {}; }
    [[nodiscard]] auto hasLocation() const noexcept -> bool override;
    [[nodiscard]] auto location() const noexcept -> Location override;
    void setLocation(const Location &newLocation) noexcept override;

    // empty defaults
    [[nodiscard]] auto asInteger() const noexcept -> int64_t override { return 0LL; }
    [[nodiscard]] auto asBoolean() const noexcept -> bool override { return false; }
    [[nodiscard]] auto asFloat() const noexcept -> double override { return 0.0; }
    [[nodiscard]] auto asText() const noexcept -> String override { return {}; }
    [[nodiscard]] auto asDate() const noexcept -> Date override { return {}; }
    [[nodiscard]] auto asTime() const noexcept -> Time override { return {}; }
    [[nodiscard]] auto asDateTime() const noexcept -> DateTime override { return {}; }
    [[nodiscard]] auto asBytes() const noexcept -> Bytes override { return {}; }
    [[nodiscard]] auto asTimeDelta() const noexcept -> TimeDelta override { return {}; }
    [[nodiscard]] auto asRegEx() const noexcept -> RegEx override { return {}; }
    [[nodiscard]] auto asValueList() const noexcept -> conf::ValueList override { return {}; }
    [[nodiscard]] auto asIntegerOrThrow() const -> int64_t override {
        throwAsTypeMismatch(*this, ValueType::Integer);
    }
    [[nodiscard]] auto asBooleanOrThrow() const -> bool override {
        throwAsTypeMismatch(*this, ValueType::Boolean);
    }
    [[nodiscard]] auto asFloatOrThrow() const -> double override {
        throwAsTypeMismatch(*this, ValueType::Float);
    }
    [[nodiscard]] auto asTextOrThrow() const -> String override {
        throwAsTypeMismatch(*this, ValueType::Text);
    }
    [[nodiscard]] auto asDateOrThrow() const -> Date override {
        throwAsTypeMismatch(*this, ValueType::Date);
    }
    [[nodiscard]] auto asTimeOrThrow() const -> Time override {
        throwAsTypeMismatch(*this, ValueType::Time);
    }
    [[nodiscard]] auto asDateTimeOrThrow() const -> DateTime override {
        throwAsTypeMismatch(*this, ValueType::DateTime);
    }
    [[nodiscard]] auto asBytesOrThrow() const -> Bytes override {
        throwAsTypeMismatch(*this, ValueType::Bytes);
    }
    [[nodiscard]] auto asTimeDeltaOrThrow() const -> TimeDelta override {
        throwAsTypeMismatch(*this, ValueType::TimeDelta);
    }
    [[nodiscard]] auto asRegExOrThrow() const -> RegEx override {
        throwAsTypeMismatch(*this, ValueType::RegEx);
    }
    [[nodiscard]] auto asValueListOrThrow() const -> ValueList override {
        throwAsTypeMismatch(*this, ValueType::ValueList);
    }
    [[nodiscard]] auto toTextRepresentation() const noexcept -> String override {
        return {};
    }

public: // modification
    /// Set the name for this value.
    ///
    template<typename Fwd>
    void setName(Fwd &&name) noexcept { _name = std::forward<Fwd>(name); }

    /// Transform a value type into another.
    ///
    /// @param targetType The target type for the transformation.
    ///
    virtual void transform(ValueType targetType) {
        throw std::runtime_error("Conversion not possible.");
    }

public: // factory methods
    /// @{
    /// Factory method to create a value.
    ///
    [[nodiscard]] static auto createInteger(Integer value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createBoolean(bool value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createFloat(Float value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createText(const String &value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createText(String &&value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createDate(const Date &value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createTime(const Time &value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createDateTime(const DateTime &value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createBytes(const Bytes &value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createBytes(Bytes &&value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createTimeDelta(const TimeDelta &value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createRegEx(const RegEx &value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createRegEx(RegEx &&value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createValueList(std::vector<ValuePtr> &&valueList) noexcept -> ValuePtr;

    [[nodiscard]] static auto createSectionList() noexcept -> ValuePtr;
    [[nodiscard]] static auto createIntermediateSection() noexcept -> ValuePtr;
    [[nodiscard]] static auto createSectionWithNames() noexcept -> ValuePtr;
    [[nodiscard]] static auto createSectionWithTexts() noexcept -> ValuePtr;
    /// @}

public: // implement `Container`
    void setParent(const conf::ValuePtr &parent) override;
    void addValue(const ValuePtr &childValue) override;

public: // helper methods.
    /// Fast name-based access for child-values.
    ///
    [[nodiscard]] virtual auto valueImpl(const Name &name) const noexcept -> ValuePtr {
        return nullptr;
    }

    [[noreturn]] static void throwAsTypeMismatch(
        const conf::Value &thisValue,
        ValueType expectedType);

    template<typename MessageFwd>
    [[noreturn]] static void throwErrorWithPath(
        const ErrorCategory errorCategory,
        MessageFwd &&message,
        const conf::Value &thisValue,
        const NamePathLike &namePath) {

        auto path = thisValue.namePath();
        path.append(toNamePath(namePath));
        throw Error(
            errorCategory,
            std::forward<MessageFwd>(message),
            std::move(path));
    }

    [[noreturn]] static void throwValueNotFound(
        const conf::Value &thisValue,
        const NamePathLike &namePath);

    [[noreturn]] static void throwTypeMismatch(
        const conf::Value &thisValue,
        ValueType expectedType,
        ValueType actualType,
        const NamePathLike &namePath);

    template<ValueType::Enum tValueType>
    [[nodiscard]] static auto getterOrThrow(
        const conf::Value &thisValue,
        const NamePathLike &namePath) -> conf::ValuePtr {

        const auto valuePtr = thisValue.value(namePath);
        if (valuePtr == nullptr) {
            throwValueNotFound(thisValue, namePath);
        }
        if (valuePtr->type() != tValueType) {
            throwTypeMismatch(thisValue, tValueType, valuePtr->type(), namePath);
        }
        return valuePtr;
    }

    template<typename ReturnType, ValueType::Enum tValueType>
    [[nodiscard]] static auto valueGetterOrThrow(
        const conf::Value &thisValue,
        const NamePathLike &namePath) -> ReturnType {

        const auto valuePtr = getterOrThrow<tValueType>(thisValue, namePath);
        return valuePtr->template asType<ReturnType>();
    }

    template<ValueType::Enum tValueType>
    [[nodiscard]] static auto sectionGetter(
        const conf::Value &thisValue,
        const NamePathLike &namePath) noexcept -> conf::ValuePtr {

        auto valuePtr = thisValue.value(namePath);
        if (valuePtr == nullptr) {
            return nullptr;
        }
        if (valuePtr->type() != tValueType) {
            return nullptr;
        }
        return valuePtr;
    }

    template<typename T>
    [[nodiscard]] static auto valueGetter(
        const conf::Value &thisValue,
        const NamePathLike &namePath,
        const T &defaultValue) noexcept -> T {

        const auto valuePtr = thisValue.value(namePath);
        if (valuePtr == nullptr) {
            return defaultValue;
        }
        if (valuePtr->type() != ValueType::from<T>()) {
            return defaultValue;
        }
        return valuePtr->asType<T>();
    }

protected:
    Name _name; ///< The name of the value.
    std::size_t _index{}; ///< The index in the parent container.
    std::weak_ptr<conf::Value> _parent; ///< The parent value.
    Location _location; ///< The location of this value.
};


}

