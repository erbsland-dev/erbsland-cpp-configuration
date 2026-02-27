// Copyright (c) 2024-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../char/CharClass.hpp"
#include "../utilities/ComparisonHelper.hpp"

#include "../../EscapeMode.hpp"

#include <string>


namespace erbsland::conf { class String; }


namespace erbsland::conf::impl {


/// A character that behaves like `char32_t`, but supports named characters.
///
/// The extensive use of test methods in this class is intentional: The main goal is to make the lexer code as
/// readable and maintainable as possible.
/// Calling test methods with easy-to-read names leads to simpler code,
/// and all the indirect nesting is resolved by the compiler.
///
/// @tested `CharTest`, `CharClassTest`
///
class Char {
public:
    /// The character names.
    ///
    enum Name : char32_t {
        // Special magic numbers.
        EndOfData = 0x00FFFFFFFU, ///< The value for the end-of-data mark
        Error = 0x00EEEEEEEU, ///< The value for an error mark,

        // Predefined Unicode code points
        ByteOrderMark = 0x0000FEFFU,

        // Named characters - Literal Names
        NewLine = U'\n',
        CarriageReturn = U'\r',
        Space = U' ',
        Tab = U'\t',
        Hash = U'#',
        At = U'@',
        DoubleQuote = U'"',
        SingleQuote = U'\'',
        Backtick = U'`',
        Slash = U'/',
        LessThan = U'<',
        GreaterThan = U'>',
        Underscore = U'_',
        Dollar = U'$',
        LcA = U'a',
        LcB = U'b',
        LcE = U'e',
        LcF = U'f',
        LcI = U'i',
        LcN = U'n',
        LcR = U'r',
        LcT = U't',
        LcU = U'u',
        LcX = U'x',
        LcZ = U'z',
        UcA = U'A',
        UcB = U'B',
        UcE = U'E',
        UcF = U'F',
        UcI = U'I',
        UcN = U'N',
        UcR = U'R',
        UcT = U'T',
        UcU = U'U',
        UcX = U'X',
        UcZ = U'Z',
        Digit0 = U'0',
        Digit1 = U'1',
        Digit9 = U'9',
        Colon = U':',
        Equal = U'=',
        Comma = U',',
        FullStop = U'.',
        Backslash = U'\\',
        OpenCBracket = U'{',
        ClosingCBracket = U'}',
        OpenSBracket = U'[',
        ClosingSBracket = U']',
        Plus = U'+',
        Minus = U'-',
        Asterisk = U'*',
        Micro = U'µ',
        QuestionMark = U'?',
        Pipe = U'|',

        // Character meanings (Aliases)
        CommentStart = Hash,
        DecimalPoint = FullStop,
        TimeSeparator = Colon,
        DateSeparator = Minus,
        NamePathSeparator = FullStop,
        ValueListSeparator = Comma,
        DigitSeparator = SingleQuote,
    };

public: // construction
    /// Create the end of data character.
    ///
    constexpr Char() noexcept : _unicode(static_cast<char32_t>(EndOfData)) {}

    /// Create an instance from the given enum value.
    ///
    /// @param name The value to convert.
    ///
    constexpr Char(const Name name) noexcept : _unicode{static_cast<char32_t>(name)} {} // NOLINT(*-explicit-constructor)

    /// Prevent unambiguous conversions from `char32_t`.
    ///
    /// @param unicode The value to convert.
    ///
    explicit constexpr Char(const char32_t unicode) noexcept : _unicode{unicode} {}

public: // operators
    /// Implicit conversion to `char32_t`.
    ///
    /// @return The value of this character.
    ///
    constexpr operator char32_t() const noexcept { return _unicode; } // NOLINT(*-explicit-constructor)

    // Comparison with Char, Char::Name and char32_t
    ERBSLAND_CONF_CONSTEXPR_COMPARE_MEMBER(const Char &other, _unicode, other._unicode);
    ERBSLAND_CONF_CONSTEXPR_COMPARE_MEMBER(const char32_t other, _unicode, other);
    ERBSLAND_CONF_COMPARE_FRIEND(const char32_t unicode, const Char &character, unicode, character._unicode);
    ERBSLAND_CONF_CONSTEXPR_COMPARE_MEMBER(const Name name, _unicode, static_cast<char32_t>(name));
    ERBSLAND_CONF_COMPARE_FRIEND(const Name name, const Char &character, static_cast<char32_t>(name), character._unicode);

    // Comparison with CharClass
    constexpr auto operator==(const CharClass charClass) const noexcept -> bool {
        return isClass(charClass);
    }
    friend auto operator==(const CharClass charClass, const Char &character) noexcept -> bool {
        return character.isClass(charClass);
    }
    constexpr auto operator!=(const CharClass charClass) const noexcept -> bool {
        return !isClass(charClass);
    }
    friend auto operator!=(const CharClass charClass, const Char &character) noexcept -> bool {
        return !character.isClass(charClass);
    }

public: // Accessors
    /// Get the value of this character.
    ///
    [[nodiscard]] constexpr auto raw() const noexcept -> char32_t { return _unicode; }

public: // Conversion
    /// Append this character as a UTF-8 sequence to a string.
    ///
    /// Invalid code points: Invalid code points are replaced with the code sequence "replacement character".
    ///
    /// @param str The string to append the UTF-8 sequence.
    ///
    void appendTo(std::u8string &str) const noexcept;

    /// @copydoc appendTo(std::u8string)
    void appendTo(String &str) const noexcept;

    /// @copydoc appendTo(std::u8string)
    void appendTo(std::string &str) const noexcept;

    /// Get the size of the UTF-8 sequence for this character.
    ///
    /// Invalid code points: Invalid code points are replaced with the code sequence "replacement character".
    /// 
    /// @return The size in bytes.
    ///
    [[nodiscard]] auto utf8Size() const noexcept -> std::size_t;

    /// Append a lowercase version of this character to the given string.
    ///
    void appendLowerCaseTo(String &str) const noexcept { toLowerCase().appendTo(str); }

    /// Append a regular name version of this character to the given string.
    ///
    void appendRegularNameTo(String &str) const noexcept { toRegularName().appendTo(str); }

    /// Append escaped.
    ///
    void appendEscaped(std::u8string &str, EscapeMode escapeMode) const noexcept;

    /// @copydoc appendEscaped(String, EscapeMode)
    void appendEscaped(String &str, EscapeMode escapeMode) const noexcept;

    /// Get the size of an escaped character in bytes.
    ///
    [[nodiscard]] auto escapedUtf8Size(EscapeMode escapeMode) const noexcept -> std::size_t;

    /// Convert uppercase ascii letters A-Z into lowercase letters a-z.
    ///
    [[nodiscard]] auto toLowerCase() const noexcept -> Char;

    /// Convert `A-Z` to `a-z` and space to `_`.
    ///
    [[nodiscard]] auto toRegularName() const noexcept -> Char;

    /// Convert a decimal digit to a numerical value.
    ///
    [[nodiscard]] auto toDecimalDigitValue() const noexcept -> uint8_t {
        if (_unicode >= Digit0 && _unicode <= Digit9) {
            return static_cast<uint8_t>(_unicode - Digit0);
        }
        return 0;
    }

    /// Convert a hex digit to a numerical value.
    ///
    [[nodiscard]] auto toHexDigitValue() const noexcept -> uint8_t {
        if (_unicode >= Digit0 && _unicode <= Digit9) {
            return static_cast<uint8_t>(_unicode - Digit0);
        }
        if (_unicode >= LcA && _unicode <= LcF) {
            return static_cast<uint8_t>(_unicode - LcA + 0xAU);
        }
        if (_unicode >= UcA && _unicode <= UcF) {
            return static_cast<uint8_t>(_unicode - UcA + 0xAU);
        }
        return 0;
    }

public: // Tests
    /// Test if the Unicode value is in the valid range.
    ///
    [[nodiscard]] constexpr auto isValidUnicode() const noexcept -> bool {
        return _unicode <= 0x10FFFFU && (_unicode < 0xD800U || _unicode > 0xDFFFU);
    }

    /// Test if the Unicode value is valid for an escape sequence.
    ///
    [[nodiscard]] constexpr auto isValidEscapeUnicode() const noexcept -> bool {
        return isValidUnicode() && _unicode != ByteOrderMark && _unicode != 0;
    }

    template <typename... Chars> requires (std::convertible_to<Chars, char32_t> && ...)
    [[nodiscard]] constexpr auto isChar(Chars... chars) const noexcept -> bool {
        return ((_unicode == chars) || ...);
    }
    [[nodiscard]] constexpr auto isChar(const CharClass charClass) const noexcept -> bool {
        return isClass(charClass);
    }
    template <typename T> requires std::convertible_to<T, char32_t>
    [[nodiscard]] constexpr auto isInRange(T first, T last) const noexcept {
        return _unicode >= first && _unicode <= last;
    }

public: // Helper methods.
    /// Basic numeric code point comparison.
    ///
    [[nodiscard]] static auto compare(const Char lhs, const Char rhs) noexcept -> std::strong_ordering {
        return lhs._unicode <=> rhs._unicode;
    }

    [[nodiscard]] static auto compareCaseInsensitive(const Char lhs, const Char rhs) noexcept -> std::strong_ordering {
        return lhs.toLowerCase() <=> rhs.toLowerCase();
    }

    [[nodiscard]] static auto compareName(const Char lhs, const Char rhs) noexcept -> std::strong_ordering {
        return lhs.toRegularName() <=> rhs.toRegularName();
    }

public:
    /// Test if this character is part of a character class.
    ///
    [[nodiscard]] constexpr auto isClass(const CharClass cls) const noexcept -> bool {
        switch (cls) {
        case CharClass::Spacing:
            return isChar(Tab, Space);
        case CharClass::LineBreak:
            return isChar(NewLine, CarriageReturn);
        case CharClass::Letter:
            return isInRange(LcA, LcZ) || isInRange(UcA, UcZ);
        case CharClass::NameStart:
            return isInRange(LcA, LcZ) || isInRange(UcA, UcZ) || isChar(At, DoubleQuote);
        case CharClass::LetterOrDigit:
            return isInRange(LcA, LcZ) || isInRange(UcA, UcZ) || isInRange(Digit0, Digit9);
        case CharClass::DecimalDigit:
            return isInRange(Digit0, Digit9);
        case CharClass::HexDigit:
            return isInRange(Digit0, Digit9) || isInRange(LcA, LcF) || isInRange(UcA, UcF);
        case CharClass::NameValueSeparator:
            return isChar(Colon, Equal);
        case CharClass::OpeningBracket:
            return isChar(DoubleQuote, Backtick, Slash, LessThan);
        case CharClass::EndOfLineStart:
            return isChar(Tab, Space, NewLine, CarriageReturn, CommentStart, EndOfData);
        case CharClass::LetterA:
            return isChar(LcA, UcA);
        case CharClass::LetterB:
            return isChar(LcB, UcB);
        case CharClass::LetterF:
            return isChar(LcF, UcF);
        case CharClass::LetterI:
            return isChar(LcI, UcI);
        case CharClass::LetterN:
            return isChar(LcN, UcN);
        case CharClass::LetterT:
            return isChar(LcT, UcT);
        case CharClass::LetterX:
            return isChar(LcX, UcX);
        case CharClass::LetterZ:
            return isChar(LcZ, UcZ);
        case CharClass::NumberStart:
            return isInRange(Digit0, Digit9) || isChar(Plus, Minus);
        case CharClass::TimeStart:
            return isInRange(Digit0, Digit9) || isChar(LcT, UcT);
        case CharClass::FloatLiteralStart:
            return isChar(Plus, Minus, LcN, UcN, LcI, UcI);
        case CharClass::ExponentStart:
            return isChar(LcE, UcE);
        case CharClass::BinaryDigit:
            return isChar(Digit0, Digit1);
        case CharClass::PlusOrMinus:
            return isChar(Plus, Minus);
        case CharClass::SectionNameStart:
            return isInRange(LcA, LcZ) || isInRange(UcA, UcZ) || isChar(DoubleQuote);
        case CharClass::FormatIdentifierChar:
            return isInRange(LcA, LcZ) || isInRange(UcA, UcZ) || isInRange(Digit0, Digit9) || isChar(Underscore, Minus);
        case CharClass::IntegerSuffixChar:
            return isInRange(LcA, LcZ) || isInRange(UcA, UcZ) || isChar(Micro);
        case CharClass::LineBreakOrEnd:
            return isChar(NewLine, CarriageReturn, EndOfData);
        case CharClass::ValidAfterValue:
            return isChar(Tab, Space, NewLine, CarriageReturn, CommentStart, EndOfData, ValueListSeparator);
        case CharClass::ValidLang:
            return isChar(Tab, NewLine, CarriageReturn) || !(isChar(Error) || isInRange(0x00U, 0x1FU) || isInRange(0x7FU, 0xA0U));
        case CharClass::SectionStart:
            return isChar(Minus, Asterisk, OpenSBracket);
        case CharClass::EscapedForText:
            return isInRange(0x00U, 0x1FU) // Control characters
                || isInRange(0x7FU, 0xA0U) // DEL and other formatting-like characters; includes non-breaking space
                || isChar(Backslash, DoubleQuote);
        case CharClass::EscapedForTextName:
            return isInRange(0x00U, 0x1FU) // Control characters
                || _unicode >= 0x7FU // Everything outside the 7-bit range.
                || isChar(Backslash, DoubleQuote, FullStop, Colon, Equal);
        case CharClass::EscapedForErrors:
            return isInRange(0x00U, 0x1FU) // Control characters
                || isInRange(0x7FU, 0xA0U) // DEL and other formatting-like characters; includes non-breaking space
                || isInRange(0x200BU, 0x200FU) // Zero-width and directional characters
                || isInRange(0x2028U, 0x2029U) // Line/Paragraph separators
                || isInRange(0xFFF9U, 0xFFFBU) // Interlinear annotation controls
                || isInRange(0x2066U, 0x2069U) // Bidirectional isolates and marks
                || isChar(Backslash, DoubleQuote)
                || _unicode == 0x061CU // Arabic Letter Mark
                || _unicode == 0xFEFFU; // Byte Order Mark
        case CharClass::FilePathSeparator:
            return isChar(Backslash, Slash);
        case CharClass::InvalidWindowsServerName:
            return isInRange(0x00U, 0x1FU) // Control characters
                || _unicode > 0x7FU // Any non-7-bit characters.
                || isChar(Asterisk, QuestionMark, Pipe, DoubleQuote, LessThan);
        default:
            return false;
        }
    }

private:
    [[nodiscard]] auto isEscapedForMode(EscapeMode escapeMode) const noexcept -> bool;

protected:
    char32_t _unicode{EndOfData};
};


}

