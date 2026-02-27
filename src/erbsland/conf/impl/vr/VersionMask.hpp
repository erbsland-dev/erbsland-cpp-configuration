// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../Integer.hpp"
#include "../../String.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <ranges>
#include <string>
#include <vector>


namespace erbsland::conf::impl {


/// A closed, inclusive version range [first, last].
///
/// Notes:
/// - Versions are modeled as non-negative integers (`Integer`).
/// - Endpoints are clamped to be >= 0. If constructed with reversed endpoints,
///   they are automatically ordered so that `fist <= last`.
/// - The data member name `fist` is a historical typo kept for ABI compatibility; it represents the first value.
class VersionRange final {
public:
    /// Create a zero version range 0-0.
    VersionRange() = default;
    /// Creates a version range with a single value n-n.
    /// @param value The single version value (>=0).
    constexpr explicit VersionRange(const Integer value) noexcept
    :
        first{clampVersion(value)},
        last{clampVersion(value)} {
    }
    /// Create a version name first-last.
    /// @param fist The first version of the range (>=0).
    /// @param last The last version of the range (>=0).
    constexpr VersionRange(const Integer fist, const Integer last) noexcept
    :
        first{lowerVersion(fist, last)},
        last{upperVersion(fist, last)} {
    }

    /// Creates a version range that covers all valid versions.
    constexpr static auto all() noexcept -> VersionRange {
        return VersionRange{0, std::numeric_limits<Integer>::max()};
    }

public: // tests
    [[nodiscard]] auto matches(const Integer version) const noexcept -> bool {
        return version >= first && version <= last;
    }

private:
    [[nodiscard]] constexpr static auto clampVersion(const Integer value) noexcept -> Integer {
        return std::max(value, Integer{0});
    }
    [[nodiscard]] constexpr static auto lowerVersion(const Integer first, const Integer last) noexcept -> Integer {
        return std::min(clampVersion(first), clampVersion(last));
    }
    [[nodiscard]] constexpr static auto upperVersion(const Integer first, const Integer last) noexcept -> Integer {
        return std::max(clampVersion(first), clampVersion(last));
    }
public:
    Integer first{};
    Integer last{};
};


/// A set-like mask over non-negative integer versions.
///
/// Semantics and invariants:
/// - A mask represents a finite union of closed, inclusive ranges.
/// - Public constructors/factories keep the internal representation normalized:
///   ranges are sorted by start, coalesced (overlapping or adjacent are merged),
///   and minimal. An empty mask has no ranges and matches nothing. The default
///   mask matches all versions (>= 0).
class VersionMask final {
public:
    /// Creates a mask that matches all possible versions (>= 0).
    VersionMask() = default;

    /// Creates a mask consisting of a single range.
    /// This constructor does not coalesce with other ranges (by design),
    /// but the provided `VersionRange` is already normalized on construction.
    constexpr VersionMask(const VersionRange &range) noexcept : _ranges{{range}} {} // NOLINT(*-explicit-constructor)

private:
    /// Internal constructor to create a mask from ranges.
    template<typename Fwd>
    requires (std::is_same_v<std::remove_cvref_t<Fwd>, std::vector<VersionRange>>)
    constexpr explicit VersionMask(Fwd &&ranges) noexcept : _ranges{std::forward<Fwd>(ranges)} {}

public:
    /// Create an empty mask that matches nothing.
    [[nodiscard]] static auto empty() noexcept -> VersionMask {
        return VersionMask{std::vector<VersionRange>{}};
    }

    /// Construct a mask from a vector of ranges.
    /// This will normalize the ranges before using them in the mask.
    ///
    template<typename Fwd>
    requires (std::is_same_v<std::remove_cvref_t<Fwd>, std::vector<VersionRange>>)
    [[nodiscard]] static auto fromRanges(Fwd &&values) noexcept -> VersionMask {
        return VersionMask{normalize(std::forward<Fwd>(values))};
    }
    [[nodiscard]] static auto fromRanges(const std::initializer_list<VersionRange> values) noexcept -> VersionMask {
        return VersionMask{normalize(std::vector<VersionRange>{values})};
    }

    /// Construct a mask that matches any of the given version integers (OR semantics).
    ///
    /// Notes:
    /// - Negative values are clamped to 0 by `VersionRange`.
    /// - Duplicates are removed, and adjacent numbers are merged into ranges.
    /// - Empty input creates an empty mask (matches nothing).
    ///
    [[nodiscard]] static auto fromIntegers(const std::vector<Integer> &values) noexcept -> VersionMask {
        return fromRanges(std::vector<VersionRange>{values.begin(), values.end()});
    }
    [[nodiscard]] static auto fromIntegers(const std::initializer_list<Integer> values) noexcept -> VersionMask {
        return fromRanges(std::vector<VersionRange>{values.begin(), values.end()});
    }

    /// Merge this mask with another one, using OR semantics.
    /// The result is a new, merged, and normalized mask.
    /// @param other The other mask for the merge.
    [[nodiscard]] auto unionWith(const VersionMask &other) const noexcept -> VersionMask {
        // Fast-path: if either matches all, the union is all
        if (isAny() || other.isAny()) {
            return {};
        }
        if (isEmpty()) {
            return other;
        }
        if (other.isEmpty()) {
            return *this;
        }
        // Collect all ranges and normalize at once
        std::vector<VersionRange> all;
        all.reserve(_ranges.size() + other._ranges.size());
        all.insert(all.end(), _ranges.begin(), _ranges.end());
        all.insert(all.end(), other._ranges.begin(), other._ranges.end());
        return VersionMask{normalize(std::move(all))};
    }
    /// Merge this mask with another mask one, using OR semantics.
    [[nodiscard]] auto operator|(const VersionMask &other) const noexcept -> VersionMask {
        return unionWith(other);
    }
    /// Merge this mask with another mask one, using OR semantics.
    auto operator|=(const VersionMask &other) noexcept -> VersionMask& {
        *this = unionWith(other);
        return *this;
    }

    /// Merge this mask with another one, using AND semantics.
    /// The result is a new, merged, and normalized mask.
    /// @param other The other mask for the merge.
    [[nodiscard]] auto intersectionWith(const VersionMask &other) const noexcept -> VersionMask {
        // Fast-paths: empty and any handling.
        if (isEmpty() || other.isEmpty()) {
            return empty();
        }
        if (isAny()) {
            return other;
        }
        if (other.isAny()) {
            return *this;
        }

        // Normalize both
        auto a = normalize(_ranges);
        auto b = normalize(other._ranges);

        // Two-pointer sweep to compute intersections
        auto result = empty();
        std::size_t i = 0;
        std::size_t j = 0;
        while (i < a.size() && j < b.size()) {
            const auto &ra = a[i];
            const auto &rb = b[j];
            const auto start = std::max(ra.first, rb.first);
            const auto end = std::min(ra.last, rb.last);
            if (start <= end) {
                // Overlap -> add intersection
                if (!result._ranges.empty() && start <= result._ranges.back().last + 1) {
                    // coalesce with previous if overlapping/adjacent
                    result._ranges.back().last = std::max(result._ranges.back().last, end);
                } else {
                    result._ranges.emplace_back(start, end);
                }
            }
            if (ra.last < rb.last) {
                ++i;
            } else {
                ++j;
            }
        }

        return result;
    }
    /// Merge this mask with another one, using AND semantics.
    [[nodiscard]] auto operator&(const VersionMask &other) const noexcept -> VersionMask {
        return intersectionWith(other);
    }
    /// Merge this mask with another one, using AND semantics.
    auto operator&=(const VersionMask &other) noexcept -> VersionMask& {
        *this = intersectionWith(other);
        return *this;
    }

    /// Complement this mask within the universe [0, maxInt()].
    /// Example: !(1-3, 7-10) == (0-0, 4-6, 11-max)
    [[nodiscard]] auto complement() const noexcept -> VersionMask {
        // Fast paths
        if (isAny()) {
            return empty();
        }
        if (isEmpty()) {
            return VersionMask{std::vector<VersionRange>{VersionRange::all()}};
        }

        // Work directly on the internal (already normalized) ranges
        std::vector<VersionRange> gaps;
        gaps.reserve(_ranges.size() + 1);

        const auto maxI = maxInt();

        // Gap before the first range
        const auto firstStart = _ranges.front().first;
        if (firstStart > 0) {
            gaps.emplace_back(0, firstStart - 1);
        }

        // Gaps between ranges
        for (size_t i = 1; i < _ranges.size(); ++i) {
            const auto &prev = _ranges[i - 1];
            const auto &cur = _ranges[i];
            // If prev ends at max, there is no further universe to cover
            if (prev.last == maxI) {
                break;
            }
            const auto gapStart = prev.last + 1; // safe, prev.last < maxI ensured above
            const auto gapEnd = cur.first - 1;   // cur.first >= prev.last + 2 due to normalization
            if (gapStart <= gapEnd) {
                gaps.emplace_back(gapStart, gapEnd);
            }
        }

        // Gap after the last range
        const auto &last = _ranges.back();
        if (last.last < maxI) {
            gaps.emplace_back(last.last + 1, maxI);
        }

        if (gaps.empty()) {
            return empty();
        }
        return VersionMask{std::move(gaps)};
    }

    /// Logical NOT operator returning the complement of this mask in the universe [0, max].
    [[nodiscard]] auto operator!() const noexcept -> VersionMask { return complement(); }

public: // accessors
    /// Access all coalesced ranges of this mask (sorted, minimal).
    [[nodiscard]] auto ranges() const noexcept -> const std::vector<VersionRange>& {
        return _ranges;
    }

public: // tests
    /// Test if this mask is empty.
    [[nodiscard]] auto isEmpty() const noexcept -> bool {
        return _ranges.empty();
    }
    /// Test if this mask matches all versions.
    [[nodiscard]] auto isAny() const noexcept -> bool {
        return _ranges.size() == 1 && _ranges.front().first == 0 && _ranges.front().last == maxInt();
    }
    /// Test if a version matches this mask.
    [[nodiscard]] auto matches(const Integer version) const noexcept -> bool {
        return std::ranges::any_of(_ranges, [version](const auto &range) {
            return range.matches(version);
        });
    }

public: // conversion
    /// Create a compact human-readable text representation.
    /// Examples: "5-6, 10, 14, 17-20", "<=10", ">=40", ">=0" (all), "<none>" (empty)
    [[nodiscard]] auto toText() const noexcept -> String {
        using std::numeric_limits;
        if (_ranges.empty()) {
            return String{u8"none"};
        }
        String result;
        for (const auto &range : _ranges) {
            if (!result.empty()) {
                result += u8", ";
            }
            const bool fromZero = (range.first == 0);
            const bool toMax = (range.last == maxInt());
            // Order of checks matters: handle full range and singleton first.
            if (fromZero && toMax) {
                // Universe
                result += u8"any";
            } else if (range.first == range.last) {
                // Singleton value (also covers 0-0 -> "0")
                result += std::to_string(range.first);
            } else if (fromZero) {
                // Up-to form
                result += u8"<=";
                result += std::to_string(range.last);
            } else if (toMax) {
                // From form
                result += u8">=";
                result += std::to_string(range.first);
            } else {
                // General interval
                result += std::to_string(range.first);
                result += "-";
                result += std::to_string(range.last);
            }
        }
        return result;
    }

private: // helper methods
    /// Get the maximum integer for a version.
    [[nodiscard]] static constexpr auto maxInt() noexcept -> Integer {
        return std::numeric_limits<Integer>::max();
    }

    /// Comparison operator to sort the ranges.
    [[nodiscard]] static constexpr auto lessStartThenEnd(const VersionRange& a, const VersionRange& b) noexcept -> bool {
        return (a.first < b.first) || (a.first == b.first && a.last < b.last);
    }

    /// Normalize a list of ranges.
    [[nodiscard]] static auto normalize(std::vector<VersionRange> ranges) noexcept -> std::vector<VersionRange> {
        if (ranges.empty()) {
            return ranges;
        }
        std::ranges::sort(ranges, lessStartThenEnd);
        std::vector<VersionRange> out;
        out.reserve(ranges.size());
        auto cur = ranges.front();
        for (size_t i = 1; i < ranges.size(); ++i) {
            const auto &range = ranges[i];
            const bool overlap = range.first <= cur.last;
            const bool adjacent = (cur.last < maxInt()) && (range.first == cur.last + 1);
            if (overlap || adjacent) {
                cur.last = std::max(cur.last, range.last);
            } else {
                out.push_back(cur);
                cur = range;
            }
        }
        out.push_back(cur);
        return out;
    }

private:
    std::vector<VersionRange> _ranges{VersionRange::all()};
};



}


