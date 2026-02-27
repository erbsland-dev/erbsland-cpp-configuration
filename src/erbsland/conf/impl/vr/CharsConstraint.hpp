// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Constraint.hpp"
#include "ConstraintHandlerContext.hpp"

#include "../char/CharRanges.hpp"

#include <vector>


namespace erbsland::conf::impl {


class CharsConstraint final : public Constraint {
    using NamedRange = std::pair<String, CharRanges>;

public:
    explicit CharsConstraint(const std::vector<String> &expectedValue) {
        _charRanges = parseTextRanges(expectedValue);
        setType(vr::ConstraintType::Chars);
    }

protected:
    void validateText(const ValidationContext &context, const String &value) const override;

private:
    /// Parse text ranges into character ranges.
    static auto parseTextRanges(const std::vector<String> &texts) -> CharRanges;

    /// Access the list of predefined character ranges.
    static auto namedRanges() noexcept -> const std::vector<NamedRange>&;

    /// Parse a parenthesized range expression: (a-z)
    static void parseParenRange(const std::vector<Char> &cps, const String &rawText, CharRanges &out);

    /// Parse a bracket list: [abc] — each inner character must be unique
    static void parseBracketList(const std::vector<Char> &cps, CharRanges &out);

    /// Try to match a named range; returns true if a known name was appended to out
    [[nodiscard]] static auto tryAppendNamedRange(
        const String &text,
        const std::vector<std::pair<String, CharRanges>> &named,
        CharRanges &out) -> bool;


private:
    CharRanges _charRanges;
};


auto handleCharsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr;


}

