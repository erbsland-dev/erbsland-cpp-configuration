// Copyright (c) 2024-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../char/Char.hpp"

#include "../../String.hpp"

#include <cstdint>
#include <format>


namespace erbsland::conf::impl {


/// The type of token.
///
/// @tested TokenTypeTest
///
class TokenType final {
public:
    /// Enum with all types.
    ///
    enum Value : uint8_t {
        EndOfData,                   ///< The end of the data.               (NoContent)
        LineBreak,                   ///< A line-break.                      (NoContent)
        Spacing,                     ///< A block of spacing.                (NoContent)
        Indentation,                 ///< A block with indentation.          (NoContent)
        Comment,                     ///< A comment.                         (NoContent)
        RegularName,                 ///< A regular name                     (String)
        TextName,                    ///< A text name                        (String) *without* double quotes
        MetaName,                    ///< A metaname                         (String) including leading @
        NameValueSeparator,          ///< A value separator `:`              (NoContent)
        ValueListSeparator,          ///< A value list separator `,`         (NoContent)
        MultiLineValueListSeparator, ///< A multi-line value separator `*`   (NoContent)
        NamePathSeparator,           ///< A name path separator. `.`         (NoContent)
        Integer,                     ///< An integer literal                 (Integer)
        Boolean,                     ///< A boolean literal                  (bool)
        Float,                       ///< A floating point literal           (Float)
        Text,                        ///< A single line text                 (String)
        MultiLineTextOpen,           ///< The start of a multi-line text.    (NoContent)
        MultiLineTextClose,          ///< The end of a multi-line text.      (NoContent)
        MultiLineText,               ///< A line of multi-line text.         (String, no linebreak)
        Code,                        ///< A single line code.                (String)
        MultiLineCodeOpen,           ///< The start of multi-line code.      (NoContent)
        MultiLineCodeLanguage,       ///< The language identifier.           (String) = language name.
        MultiLineCodeClose,          ///< The end of multi-line code.        (NoContent)
        MultiLineCode,               ///< A line of multi-line code.         (String, no linebreak)
        RegEx,                       ///< A single line regex.               (String)
        MultiLineRegexOpen,          ///< The start of multi-line regex.     (NoContent)
        MultiLineRegexClose,         ///< The end of multi-line regex.       (NoContent)
        MultiLineRegex,              ///< A line of multi-line regex.        (String, no linebreak, no comment)
        Bytes,                       ///< A single line block of bytes.      (Bytes)
        MultiLineBytesOpen,          ///< The start of multi-line bytes.     (NoContent)
        MultiLineBytesFormat,        ///< The format multi-line bytes.       (NoContent)
        MultiLineBytesClose,         ///< The end of multi-line bytes.       (NoContent)
        MultiLineBytes,              ///< A line of multi-line bytes         (Bytes)
        Date,                        ///< A date                             (Date)
        Time,                        ///< A time                             (Time)
        DateTime,                    ///< A date/time                        (DateTime)
        TimeDelta,                   ///< A time-delta                       (TimeDelta)
        SectionMapOpen,              ///< The start of a section map.        (NoContent)
        SectionMapClose,             ///< The end of a section map.          (NoContent)
        SectionListOpen,             ///< The start of a section list block. (NoContent)
        SectionListClose,            ///< The end of a section list block.   (NoContent)
        Error                        ///< Error block, for relaxed lexing.   (String) = error message.
    };

public: // construction
    TokenType() = default; // Create an error type.
    TokenType(const Value value) noexcept : _value{value} {} // NOLINT(*-explicit-constructor)

public: // operators
    constexpr auto operator==(const TokenType &other) const noexcept -> bool { return _value == other._value; }
    constexpr auto operator==(const Value other) const noexcept -> bool { return _value == other; }
    constexpr auto operator!=(const TokenType &other) const noexcept -> bool { return _value != other._value; }
    constexpr auto operator!=(const Value other) const noexcept -> bool { return _value != other; }

public: // accessors
    [[nodiscard]] constexpr auto raw() const noexcept -> Value { return _value; }

public: // helper
    [[nodiscard]] constexpr static auto fromMultiLineOpen(const char32_t character) noexcept -> TokenType {
        switch (character) {
        case Char::DoubleQuote:
            return MultiLineTextOpen;
        case Char::Backtick:
            return MultiLineCodeOpen;
        case Char::Slash:
            return MultiLineRegexOpen;
        case Char::LessThan:
            return MultiLineBytesOpen;
        default:
            return EndOfData;
        }
    }

    [[nodiscard]] constexpr static auto fromMultiLineClose(const char32_t character) noexcept -> TokenType {
        switch (character) {
        case Char::DoubleQuote:
            return MultiLineTextClose;
        case Char::Backtick:
            return MultiLineCodeClose;
        case Char::Slash:
            return MultiLineRegexClose;
        case Char::GreaterThan:
            return MultiLineBytesClose;
        default:
            return EndOfData;
        }
    }

private:
    Value _value{Error};
};



[[nodiscard]] constexpr auto toStringView(const TokenType tokenType) noexcept -> std::string_view {
    switch (tokenType.raw()) {
    case TokenType::EndOfData: return "EndOfData";
    case TokenType::LineBreak: return "LineBreak";
    case TokenType::Spacing: return "Spacing";
    case TokenType::Indentation: return "Indentation";
    case TokenType::Comment: return "Comment";
    case TokenType::RegularName: return "RegularName";
    case TokenType::TextName: return "TextName";
    case TokenType::MetaName: return "MetaName";
    case TokenType::NameValueSeparator: return "NameValueSeparator";
    case TokenType::ValueListSeparator: return "ValueListSeparator";
    case TokenType::MultiLineValueListSeparator: return "MultiLineValueListSeparator";
    case TokenType::NamePathSeparator: return "NamePathSeparator";
    case TokenType::Integer: return "Integer";
    case TokenType::Boolean: return "Boolean";
    case TokenType::Float: return "Float";
    case TokenType::Text: return "Text";
    case TokenType::MultiLineTextOpen: return "MultiLineTextOpen";
    case TokenType::MultiLineTextClose: return "MultiLineTextClose";
    case TokenType::MultiLineText: return "MultiLineText";
    case TokenType::Code: return "Code";
    case TokenType::MultiLineCodeOpen: return "MultiLineCodeOpen";
    case TokenType::MultiLineCodeLanguage: return "MultiLineCodeLanguage";
    case TokenType::MultiLineCodeClose: return "MultiLineCodeClose";
    case TokenType::MultiLineCode: return "MultiLineCode";
    case TokenType::RegEx: return "RegEx";
    case TokenType::MultiLineRegexOpen: return "MultiLineRegexOpen";
    case TokenType::MultiLineRegexClose: return "MultiLineRegexClose";
    case TokenType::MultiLineRegex: return "MultiLineRegex";
    case TokenType::Bytes: return "Bytes";
    case TokenType::MultiLineBytesOpen: return "MultiLineBytesOpen";
    case TokenType::MultiLineBytesFormat: return "MultiLineBytesFormat";
    case TokenType::MultiLineBytesClose: return "MultiLineBytesClose";
    case TokenType::MultiLineBytes: return "MultiLineBytes";
    case TokenType::Date: return "Date";
    case TokenType::Time: return "Time";
    case TokenType::DateTime: return "DateTime";
    case TokenType::TimeDelta: return "TimeDelta";
    case TokenType::SectionMapOpen: return "SectionMapOpen";
    case TokenType::SectionMapClose: return "SectionMapClose";
    case TokenType::SectionListOpen: return "SectionListOpen";
    case TokenType::SectionListClose: return "SectionListClose";
    case TokenType::Error: return "Error";
    }
    return {};
}


}


template<>
struct std::formatter<erbsland::conf::impl::TokenType> : std::formatter<std::string_view> {
    auto format(erbsland::conf::impl::TokenType tokenType, format_context& ctx) const {
        return std::formatter<std::string_view>::format(toStringView(tokenType), ctx);
    }
};
