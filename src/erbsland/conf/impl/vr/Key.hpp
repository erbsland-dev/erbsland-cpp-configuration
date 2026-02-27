// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../CaseSensitivity.hpp"
#include "../../String.hpp"
#include "../../StringList.hpp"

#include <vector>


namespace erbsland::conf::impl {


/// A key that consists of one or multiple elements.
class Key {
public:
    /// Create a key with multiple elements.
    /// @param elements The elements of the key.
    explicit Key(StringList elements) : _elements{std::move(elements)} {}
    /// Create a key with a single element.
    /// @param oneElement The single element of the key.
    explicit Key(String oneElement) {
        _elements.emplace_back(std::move(oneElement));
    }

    // defaults
    Key() = default;
    ~Key() = default;
    Key(const Key &) = default;
    Key(Key &&) noexcept = default;
    auto operator=(const Key &) -> Key& = default;
    auto operator=(Key &&) noexcept -> Key& = default;

public:
    /// Compare all elements of two keys.
    /// @param other The other key to compare.
    /// @param caseSensitivity The case sensitivity to use for comparison.
    /// @return True if all elements are equal, false otherwise.
    [[nodiscard]] auto isEqual(const Key &other, CaseSensitivity caseSensitivity) const noexcept -> bool;
    /// Compare a single element of two keys.
    /// @param other The other key to compare.
    /// @param caseSensitivity The case sensitivity to use for comparison.
    /// @param index The element index to compare.
    /// If the index is out of bounds for any of the compared keys, an empty string is compared.
    /// @return True if the elements are equal, false otherwise.
    [[nodiscard]] auto isEqual(const Key &other, CaseSensitivity caseSensitivity, std::size_t index) const noexcept -> bool;
    /// Access all elements of this key.
    [[nodiscard]] auto elements() const noexcept -> const StringList&;
    /// Access a single element of this key.
    /// @return The element at the given index or an empty string if the index is out of bounds.
    [[nodiscard]] auto element(std::size_t index) const noexcept -> const String&;
    /// Convert this key to a text representation.
    /// For the text representation, all elements of the key are joined with a comma character.
    [[nodiscard]] auto toText() const noexcept -> String;
    /// Get the size (number of elements) of this key.
    [[nodiscard]] auto size() const noexcept -> std::size_t;
    /// Get a hash for this key.
    [[nodiscard]] auto hash(CaseSensitivity caseSensitivity) const noexcept -> std::size_t;
    /// Get the hash for a single element
    [[nodiscard]] static auto elementHash(const String &element, CaseSensitivity caseSensitivity) noexcept -> std::size_t;

private:
    StringList _elements; ///< The elements of this key.
};


}


template <>
struct std::formatter<erbsland::conf::impl::Key> : std::formatter<std::string> {
    auto format(const erbsland::conf::impl::Key &key, format_context& ctx) const {
        return std::formatter<std::string>::format(key.toText().toCharString(), ctx);
    }
};

