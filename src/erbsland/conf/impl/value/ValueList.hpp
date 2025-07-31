// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ValueWithChildren.hpp"


namespace erbsland::conf::impl {


/// Implementation of value for value lists.
///
/// @tested `ValueTest`
///
class ValueList final : public ValueWithChildren {
public:
    explicit ValueList(std::vector<ValuePtr> &&valueList) noexcept
        : ValueWithChildren(ValueMap{std::move(valueList)}) {
    }
    [[nodiscard]] auto type() const noexcept -> ValueType override {
        return ValueType::ValueList;
    }
    [[nodiscard]] auto asValueList() const noexcept -> conf::ValueList override {
        return {_children.begin(), _children.end()};
    }
    [[nodiscard]] auto asValueListOrThrow() const -> conf::ValueList override {
        return {_children.begin(), _children.end()};
    }

public:
    /// Initialize the child values.
    ///
    void initializeChildren() {
        _children.setParent(shared_from_this());
    }
};


}

