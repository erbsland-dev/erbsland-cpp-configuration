// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Char.hpp"


#include "utf8/U8Format.hpp"

#include "../String.hpp"

#include <cassert>


namespace erbsland::conf::impl {


template<typename Buffer, typename CharType>
void appendCharToBuffer(Buffer &buffer, int32_t unicode) noexcept {
    if (unicode <= 0x0000007FU) {
        buffer.push_back(static_cast<CharType>(unicode));
        return;
    }
    if (unicode <= 0x000007FFU) {
        buffer.push_back(static_cast<CharType>(((unicode >> 6) & 0b00011111U) | 0b11000000U));
        buffer.push_back(static_cast<CharType>((unicode        & 0b00111111U) | 0b10000000U));
        return;
    }
    if (unicode <= 0x0000FFFFU) {
        buffer.push_back(static_cast<CharType>(((unicode >> 12) & 0b00001111U) | 0b11100000U));
        buffer.push_back(static_cast<CharType>(((unicode >> 6)  & 0b00111111U) | 0b10000000U));
        buffer.push_back(static_cast<CharType>((unicode         & 0b00111111U) | 0b10000000U));
        return;
    }
    if (unicode <= 0x0010FFFFU) {
        buffer.push_back(static_cast<CharType>(((unicode >> 18) & 0b00000111U) | 0b11110000U));
        buffer.push_back(static_cast<CharType>(((unicode >> 12) & 0b00111111U) | 0b10000000U));
        buffer.push_back(static_cast<CharType>(((unicode >> 6)  & 0b00111111U) | 0b10000000U));
        buffer.push_back(static_cast<CharType>((unicode         & 0b00111111U) | 0b10000000U));
        return;
    }
    // Placeholder for Unicode characters that are out of the valid range.
    buffer.push_back(static_cast<CharType>(0xefU));
    buffer.push_back(static_cast<CharType>(0xbfU));
    buffer.push_back(static_cast<CharType>(0xbdU));
}


void Char::appendTo(std::u8string &str) const noexcept {
    appendCharToBuffer<std::u8string, char8_t>(str, _unicode);
}


void Char::appendTo(String &str) const noexcept {
    appendTo(str.raw());
}


void Char::appendTo(std::string &str) const noexcept {
    appendCharToBuffer<std::string, char>(str, _unicode);
}


auto Char::utf8Size() const noexcept -> std::size_t {
    if (_unicode <= 0x0000007FU) {
        return 1;
    }
    if (_unicode <= 0x000007FFU) {
        return 2;
    }
    if (_unicode <= 0x0000FFFFU) {
        return 3;
    }
    if (_unicode <= 0x0010FFFFU) {
        return 4;
    }
    return 3; // If the Unicode character isn't value, a 3-byte sequence is returned.
}


void Char::appendEscaped(std::u8string &str, const EscapeMode escapeMode) const noexcept {
    if (!isValidEscapeUnicode()) {
        return;
    }
    if (!isEscapedForMode(escapeMode)) {
        appendTo(str);
        return;
    }
    if (escapeMode != EscapeMode::FullTextName) {
        switch (*this) {
        case Backslash: str.append(u8"\\\\"); return;
        case DoubleQuote: str.append(u8"\\\""); return;
        case NewLine: str.append(u8"\\n"); return;
        case CarriageReturn: str.append(u8"\\r"); return;
        case Tab: str.append(u8"\\t"); return;
        default: break;
        }
    }
    str.append(u8format("\\u{{{:x}}}", static_cast<uint32_t>(_unicode)).raw());
}


void Char::appendEscaped(String &str, const EscapeMode escapeMode) const noexcept {
    appendEscaped(str.raw(), escapeMode);
}


auto Char::escapedUtf8Size(const EscapeMode escapeMode) const noexcept -> std::size_t {
    if (!isValidEscapeUnicode()) {
        return 0;
    }
    if (!isEscapedForMode(escapeMode)) {
        return utf8Size();
    }
    if (escapeMode != EscapeMode::FullTextName) {
        const auto isShort = *this == Backslash || *this == DoubleQuote || *this == NewLine ||
            *this == CarriageReturn || *this == Tab;
        if (isShort) {
            return 2;
        }
    }
    return std::max(1, (std::bit_width(static_cast<uint32_t>(_unicode)) + 3) / 4) + 4;
}


auto Char::toLowerCase() const noexcept -> Char {
    if (_unicode >= static_cast<char32_t>(UcA) && _unicode <= static_cast<char32_t>(UcZ)) {
        const auto newValue = static_cast<char32_t>(
            _unicode + (static_cast<char32_t>(LcA) - static_cast<char32_t>(UcA)));
        return Char{newValue};
    }
    return Char{_unicode};
}


auto Char::toRegularName() const noexcept -> Char {
    if (_unicode == Space) {
        return Char{Underscore};
    }
    return toLowerCase();
}


auto Char::isEscapedForMode(EscapeMode escapeMode) const noexcept -> bool {
    switch (escapeMode) {
    case EscapeMode::Text:
        return *this == CharClass::EscapedForText;
    case EscapeMode::FullTextName:
        return *this == CharClass::EscapedForTextName;
    case EscapeMode::ErrorText:
        return *this == CharClass::EscapedForErrors;
    default:
        break;
    }
    return false;
}


}

