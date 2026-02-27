// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Char.hpp"

#include "../../String.hpp"

#include <initializer_list>
#include <vector>


namespace erbsland::conf::impl {


class CharRange {
public:
    // Constructors
    constexpr CharRange(Char first, Char last) noexcept : _first(first), _last(last) {}

    // Factory to create a single-character range
    static constexpr auto single(const Char c) noexcept -> CharRange { return CharRange{c, c}; }

    // Accessors
    [[nodiscard]] constexpr auto first() const noexcept -> Char { return _first; }
    [[nodiscard]] constexpr auto last() const noexcept -> Char { return _last; }

    // Test if this range contains the given character
    [[nodiscard]] constexpr auto contains(const Char c) const noexcept -> bool {
        return !(c < _first) && !(_last < c);
    }

private:
    Char _first;
    Char _last;
};


// A tiny helper around a vector of ranges to keep logic readable
class CharRanges {
public:
    CharRanges() = default;
    CharRanges(std::initializer_list<CharRange> init) : _ranges(init) {}

    void add(const CharRange &range) { _ranges.push_back(range); }
    void add(const Char first, const Char last) { _ranges.emplace_back(first, last); }
    void addSingle(const Char character) { _ranges.push_back(CharRange::single(character)); }
    void extend(const CharRanges &other) {
        _ranges.insert(_ranges.end(), other._ranges.begin(), other._ranges.end());
    }

    [[nodiscard]] auto contains(const Char character) const noexcept -> bool {
        return std::ranges::any_of(_ranges, [character](const auto &range) -> bool { return range.contains(character); });
    }

    // iteration support for range-based for
    [[nodiscard]] auto begin() const { return _ranges.begin(); }
    [[nodiscard]] auto end() const { return _ranges.end(); }

private:
    std::vector<CharRange> _ranges;
};


}
