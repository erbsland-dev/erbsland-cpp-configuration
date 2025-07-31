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
    /// Create a value with children using an existing list.
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
    [[nodiscard]] auto hasValue(const NamePathLike &namePath) const noexcept -> bool override {
        return _children.hasValue(namePath);
    }
    [[nodiscard]] auto value(const NamePathLike &namePath) const noexcept -> conf::ValuePtr override {
        return _children.value(namePath);
    }
    [[nodiscard]] auto valueOrThrow(const NamePathLike &namePath) const -> conf::ValuePtr override {
        return _children.valueOrThrow(namePath, *this);
    }
    [[nodiscard]] auto begin() const noexcept -> ValueIterator override {
        return _children.begin();
    }
    [[nodiscard]] auto end() const noexcept -> ValueIterator override {
        return _children.end();
    }
    void addValue(const ValuePtr &childValue) override {
        _children.addValue(childValue);
    }

public: // implement impl::Value
    [[nodiscard]] auto valueImpl(const Name &name) const noexcept -> ValuePtr override {
        return _children.valueImpl(name);
    }

protected:
    ValueMap _children; ///< A map of child values.
};


}

