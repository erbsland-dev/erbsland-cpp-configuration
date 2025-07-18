// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "impl/Definitions.hpp"

#include <iterator>
#include <memory>
#include <vector>


namespace erbsland::conf {


class Value;
using ValuePtr = std::shared_ptr<Value>;
namespace impl {
    class Value;
    using ValuePtr = std::shared_ptr<Value>;
}


/// Const iterator for the `Value` class.
///
/// This is a simple wrapper around the iterator of the internally used container.
///
/// @tested Tested via `Value` class.
///
class ValueIterator final {
    using WrappedIterator = std::vector<impl::ValuePtr>::const_iterator;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const ValuePtr;
    using difference_type = std::ptrdiff_t;
    using pointer = const ValuePtr;
    using reference = const ValuePtr;

public:
    /// Create an iterator wrapping the given internal iterator.
    ///
    /// @param it The iterator to wrap.
    constexpr explicit ValueIterator(WrappedIterator it) : _current{it} {}

    // defaults
    ValueIterator() = default;
    ~ValueIterator() = default;
    ValueIterator(const ValueIterator&) = default;
    ValueIterator(ValueIterator&&) noexcept = default;
    auto operator=(const ValueIterator&) -> ValueIterator& = default;
    auto operator=(ValueIterator&&) noexcept -> ValueIterator& = default;

public: // operators
    /// Dereference operator.
    /// @return A shared pointer to the current value.
    [[nodiscard]] auto operator*() const noexcept -> reference;

    /// Member access operator.
    /// @return A pointer to the current value.
    [[nodiscard]] auto operator->() const noexcept -> pointer;

    /// Prefix increment. Advances the iterator to the next element.
    auto operator++() noexcept -> ValueIterator&;

    /// Postfix increment. Advances the iterator and returns the previous state.
    auto operator++(int) noexcept -> ValueIterator;

    /// Equality comparison.
    [[nodiscard]] auto operator==(const ValueIterator& other) const noexcept -> bool;

    /// Inequality comparison.
    [[nodiscard]] auto operator!=(const ValueIterator& other) const noexcept -> bool;

protected:
    WrappedIterator _current;
};


}

