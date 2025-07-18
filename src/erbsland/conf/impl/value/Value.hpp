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
    [[nodiscard]] auto hasValue(std::size_t index) const noexcept -> bool override { return false; }
    [[nodiscard]] auto hasValue(const Name &name) const noexcept -> bool override { return false; }
    [[nodiscard]] auto hasValue(const NamePath &namePath) const noexcept -> bool override { return false; }
    [[nodiscard]] auto value(std::size_t index) const noexcept -> conf::ValuePtr override { return {}; }
    [[nodiscard]] auto value(const Name &name) const noexcept -> conf::ValuePtr override { return {}; }
    [[nodiscard]] auto value(const NamePath &namePath) const noexcept -> conf::ValuePtr override { return {}; }
    [[nodiscard]] auto begin() const noexcept -> ValueIterator override { return {}; }
    [[nodiscard]] auto end() const noexcept -> ValueIterator override { return {}; }
    [[nodiscard]] auto hasLocation() const noexcept -> bool override;
    [[nodiscard]] auto location() const noexcept -> Location override;
    void setLocation(const Location &newLocation) noexcept override;

    // empty defaults
    [[nodiscard]] auto toInteger() const noexcept -> int64_t override { return 0LL; }
    [[nodiscard]] auto toBoolean() const noexcept -> bool override { return false; }
    [[nodiscard]] auto toFloat() const noexcept -> double override { return 0.0; }
    [[nodiscard]] auto toText() const noexcept -> String override { return {}; }
    [[nodiscard]] auto toDate() const noexcept -> Date override { return {}; }
    [[nodiscard]] auto toTime() const noexcept -> Time override { return {}; }
    [[nodiscard]] auto toDateTime() const noexcept -> DateTime override { return {}; }
    [[nodiscard]] auto toBytes() const noexcept -> Bytes override { return {}; }
    [[nodiscard]] auto toTimeDelta() const noexcept -> TimeDelta override { return {}; }
    [[nodiscard]] auto toRegEx() const noexcept -> String override { return {}; }
    [[nodiscard]] auto toTextRepresentation() const noexcept -> String override { return {}; }
    [[nodiscard]] auto toList() const noexcept -> conf::ValueList override;

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
    [[nodiscard]] static auto createRegEx(const String &value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createRegEx(String &&value) noexcept -> ValuePtr;
    [[nodiscard]] static auto createValueList(std::vector<ValuePtr> &&valueList) noexcept -> ValuePtr;

    [[nodiscard]] static auto createSectionList() noexcept -> ValuePtr;
    [[nodiscard]] static auto createIntermediateSection() noexcept -> ValuePtr;
    [[nodiscard]] static auto createSectionWithNames() noexcept -> ValuePtr;
    [[nodiscard]] static auto createSectionWithTexts() noexcept -> ValuePtr;
    /// @}

public: // implement `Container`
    void setParent(const conf::ValuePtr &parent) override;
    void addValue(const ValuePtr &childValue) override;

protected:
    Name _name; ///< The name of the value.
    std::size_t _index{}; ///< The index in the parent container.
    std::weak_ptr<conf::Value> _parent; ///< The parent value.
    Location _location; ///< The location of this value.
};


}

