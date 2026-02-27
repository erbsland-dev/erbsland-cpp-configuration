// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "CharsConstraint.hpp"


#include "ValidationContext.hpp"
#include "ValidationError.hpp"

#include "../utf8/U8Decoder.hpp"
#include "../utf8/U8StringView.hpp"

#include "../../Error.hpp"
#include "../../EscapeMode.hpp"

#include <algorithm>
#include <unordered_set>


namespace erbsland::conf::impl {


void CharsConstraint::validateText(const ValidationContext &context, const String &value) const {
    std::size_t index = 0;
    U8StringView{value}.forEachChar([&](const Char character) -> void {
        const auto inRanges = _charRanges.contains(character);
        const auto isInvalid = isNegated() ? inRanges : !inRanges;
        if (isInvalid) {
            if (context.rule != nullptr && context.rule->isSecret()) {
                throwValidationError(u8format(
                    u8"The text contains a forbidden character at position {} in a secret value",
                    index));
            }
            String chText;
            character.appendEscaped(chText, EscapeMode::ErrorText);
            throwValidationError(u8format(
                u8"The text contains a forbidden character at position {}: \"{}\"",
                index,
                chText));
        }
        ++index;
    });
}


auto CharsConstraint::parseTextRanges(const std::vector<String> &texts) -> CharRanges {
    CharRanges result;

    for (const auto &text : texts) {
        const auto cps = U8Decoder{text}.decodeAllToVector();

        if (cps.size() >= 2 && cps.front() == U'(' && cps.back() == U')') {
            parseParenRange(cps, text, result);
        } else if (cps.size() >= 2 && cps.front() == U'[' && cps.back() == U']') {
            parseBracketList(cps, result);
        } else if (!tryAppendNamedRange(text, namedRanges(), result)) {
            throwValidationError(
                u8format(u8"Unknown named character range: \"{}\"", text.toEscaped(EscapeMode::ErrorText)));
        }
    }

    return result;
}


auto CharsConstraint::namedRanges() noexcept -> const std::vector<NamedRange>& {
    static const auto namedRanges = std::vector<NamedRange>{
        {String{u8"letters"}, {
            {Char::LcA, Char::LcZ},
            {Char::UcA, Char::UcZ},
        }},
        {String{u8"digits"}, {
            {Char::Digit0, Char::Digit9},
        }},
        {String{u8"control"}, {
            {Char{0x0000}, Char{0x001f}},
            {Char{0x007f}, Char{0x00a0}}
        }},
        {String{u8"linebreak"}, {
            {Char::NewLine, Char::NewLine},
            {Char::CarriageReturn, Char::CarriageReturn},
        }},
        {String{u8"spacing"}, {
            {Char::Tab, Char::Tab},
            {Char::Space, Char::Space},
        }}
    };
    return namedRanges;
}


void CharsConstraint::parseParenRange(const std::vector<Char> &cps, const String &rawText, CharRanges &out) {
    // Must be exactly 5 code points: '(', start, '-', end, ')'
    if (cps.size() != 5 || !(cps[2] == U'-')) {
        throwValidationError(
            u8format(u8"Invalid character range syntax: \"{}\"", rawText.toEscaped(EscapeMode::ErrorText)));
    }
    const Char start = cps[1];
    const Char end = cps[3];
    if (!(start < end)) {
        throwValidationError(u8format(
            u8"Invalid character range: start (U+{:04X}) must be lower than end (U+{:04X})",
            static_cast<unsigned>(start.raw()),
            static_cast<unsigned>(end.raw())));
    }
    out.add(start, end);
}


void CharsConstraint::parseBracketList(const std::vector<Char> &cps, CharRanges &out) {
    std::vector<Char> seen;
    for (std::size_t i = 1; i + 1 < cps.size(); ++i) {
        const Char c = cps[i];
        if (std::ranges::find(seen, c) != seen.end()) {
            String dup;
            c.appendEscaped(dup, EscapeMode::ErrorText);
            throwValidationError(u8format(u8"The character list contains a duplicate character: '{}'", dup));
        }
        seen.push_back(c);
        out.addSingle(c);
    }
}


auto CharsConstraint::tryAppendNamedRange(
    const String &text, const std::vector<std::pair<String, CharRanges>> &named, CharRanges &out) -> bool {
    for (const auto &[name, ranges] : named) {
        if (name == text) {
            out.extend(ranges);
            return true;
        }
    }
    return false;
}


auto handleCharsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr {
    const auto &node = context.node;
    const auto &rule = context.rule;
    if (rule->type() == vr::RuleType::Text) {
        const auto textValues = node->asList<String>();
        if (textValues.empty()) {
            throwValidationError(u8format(
                u8"The '{}' constraint must specify a single text value or a list of texts",
                node->name()));
        }
        return std::make_shared<CharsConstraint>(textValues);
    }
    throwValidationError(u8format(
        u8"The '{}' constraint is not supported for '{}' rules",
        node->name(),
        rule->type().toText()));
}


}
