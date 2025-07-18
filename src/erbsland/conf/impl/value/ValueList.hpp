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
    [[nodiscard]] auto toList() const noexcept -> conf::ValueList override {
        return {_children.begin(), _children.end()};
    }

public:
    /// Initialize the child values.
    ///
    /// - Set the parent to this value list.
    /// - Set the name with the index of the value in this list.
    ///
    void initializeChildren() {
        const auto thisPtr = shared_from_this();
        std::size_t index = 0;
        for (const auto &value : _children.valueList()) {
            value->setParent(thisPtr);
            value->setName(Name::createIndex(index));
            index += 1;
        }
    }
};


}

