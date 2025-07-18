// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../NamePath.hpp"
#include "../../ValueIterator.hpp"

#include <unordered_map>
#include <vector>


namespace erbsland::conf::impl {


/// A map of values.
///
class ValueMap {
public:
    using List = std::vector<ValuePtr>;
    using Map = std::unordered_map<Name, ValuePtr>;

public:
    /// Create a new value map, from a list of unnamed values.
    ///
    /// - Automatically builds the indexes and names the elements.
    ///
    /// @param valueList The value list to use to build the map.
    ///
    explicit ValueMap(List &&valueList);

    // defaults
    ValueMap() = default;

public:
    [[nodiscard]] auto empty() const noexcept -> bool;
    [[nodiscard]] auto size() const noexcept -> std::size_t;
    [[nodiscard]] auto hasValue(std::size_t index) const noexcept -> bool;
    [[nodiscard]] auto hasValue(const Name &name) const noexcept -> bool;
    [[nodiscard]] auto hasValue(const NamePath &namePath) const noexcept -> bool;
    [[nodiscard]] auto value(std::size_t index) const noexcept -> ValuePtr;
    [[nodiscard]] auto value(const Name &name) const noexcept -> ValuePtr;
    [[nodiscard]] auto value(const NamePath &namePath) const -> ValuePtr;
    [[nodiscard]] auto begin() const noexcept -> ValueIterator;
    [[nodiscard]] auto end() const noexcept -> ValueIterator;

public:
    void addValue(const ValuePtr &value);
    [[nodiscard]] auto valueList() const noexcept -> const List& { return _valueList; }
    [[nodiscard]] auto valueMap() const noexcept -> const Map& { return _valueMap; }

private:
    List _valueList; ///< The list with the values in order of their definition.
    Map _valueMap; ///< A map with the names of the values.
};



}

