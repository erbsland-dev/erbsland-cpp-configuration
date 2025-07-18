// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Value.hpp"
#include "ValueMap.hpp"

#include "../utf8/U8Format.hpp"


namespace erbsland::conf::impl {


/// A generic base class for value containers.
///
/// @tested `ValueTest`
///
class ValueWithChildren : public Value {
public:
    /// Create a value with children, using an existing list.
    ///
    /// @param children The existing list of values.
    ///
    explicit ValueWithChildren(ValueMap &&children) : _children(std::move(children)) {}

    // defaults
    ValueWithChildren() = default;

public: // implement `Value`
    [[nodiscard]] auto size() const noexcept -> size_t override {
        return _children.size();
    }
    [[nodiscard]] auto hasValue(std::size_t index) const noexcept -> bool override {
        return _children.hasValue(index);
    }
    [[nodiscard]] auto hasValue(const Name &name) const noexcept -> bool override {
        return _children.hasValue(name);
    }
    [[nodiscard]] auto hasValue(const NamePath &namePath) const noexcept -> bool override {
        return _children.hasValue(namePath);
    }
    [[nodiscard]] auto value(const std::size_t index) const noexcept -> conf::ValuePtr override {
        return _children.value(index);
    }
    [[nodiscard]] auto value(const Name &name) const noexcept -> conf::ValuePtr override {
        return _children.value(name);
    }
    [[nodiscard]] auto value(const NamePath &namePath) const noexcept -> conf::ValuePtr override {
        return _children.value(namePath);
    }
    [[nodiscard]] auto begin() const noexcept -> ValueIterator override {
        return _children.begin();
    }
    [[nodiscard]] auto end() const noexcept -> ValueIterator override {
        return _children.end();
    }
    [[nodiscard]] auto toTestText() const noexcept -> String override {
        return u8format("{}()", type());
    }

    void addValue(const ValuePtr &childValue) override {
        _children.addValue(childValue);
    }

protected:
    ValueMap _children; ///< A map of child values.
};


}

