// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Name.hpp"
#include "Number.hpp"
#include "Text.hpp"

#include "../decoder/FastNameDecoder.hpp"

#include "../../Name.hpp"

#include <cstddef>
#include <limits>
#include <span>


namespace erbsland::conf::impl {


/// A minimalistic decoder for decoding names and name paths.
///
class NameLexer {
public:
    /// Create a new name lexer using the given text reference.
    ///
    /// @param text A reference to the text to parse.
    ///
    explicit NameLexer(const std::span<const char8_t> text) noexcept : _decoder{text} {}

    // defaults
    ~NameLexer() = default;
    NameLexer(const NameLexer &) = delete;
    auto operator=(const NameLexer &) -> NameLexer& = delete;

public:
    void initialize() {
        if (_decoder.buffer().size() > limits::maxLineLength) {
            _decoder.throwLimitExceededError(u8"A name path must not exceed 4kb.");
        }
        _decoder.initialize();
    }

    [[nodiscard]] auto hasNext() const noexcept -> bool {
        return _decoder.character() != Char::EndOfData;
    }

    [[nodiscard]] auto next() -> Name {
        skipSpacing();
        bool readSeparator = false;
        if (_decoder.character() == Char::EndOfData) {
            return {}; // Coverage: This prevents misuse and is not used for correct operation.
        }
        if (_decoder.character() == Char::NamePathSeparator) {
            if (!_afterFirstElement) {
                _decoder.throwSyntaxError(u8"The name path must not start with a separator.");
            }
            _decoder.next();
            skipSpacing(); // Ignore spacing after the separator.
            if (_decoder.character() == Char::EndOfData) {
                _decoder.throwUnexpectedEndOfDataError(u8"Name path must not end with a separator.");
            }
            readSeparator = true;
        }
        if (_decoder.character() == CharClass::Letter || _decoder.character() == Char::At) {
            return expectRegularName();
        }
        if (_decoder.character() == Char::DoubleQuote) {
            return expectTextNameOrIndex();
        }
        if (_decoder.character() == Char::OpenSBracket) {
            if (readSeparator) {
                _decoder.throwSyntaxError(u8"An index must not be preceded by a separator.");
            }
            return expectIndex();
        }
        if (_decoder.character() == CharClass::DecimalDigit) {
            _decoder.throwSyntaxError(u8"Regular names must not start with a digit.");
        }
        if (_decoder.character() == Char::Underscore) {
            _decoder.throwSyntaxError(u8"Regular names must not start with an underscore.");
        }
        if (_decoder.character() == Char::NamePathSeparator) {
            _decoder.throwSyntaxError(u8"Name path must not contain multiple subsequent separators.");
        }
        _decoder.throwSyntaxError(u8"Expected regular name, text name or index but got something else.");
    }

private:
    void expectNameSeparatorOrEnd() {
        skipSpacing();
        if (!(_decoder.character() == Char::NamePathSeparator || _decoder.character() == Char::EndOfData)) {
            _decoder.throwSyntaxError(
                u8"Unexpected character after the last element. Expected name separator or the end of the path.");
        }
    }

    void expectNameSeparatorIndexOrEnd() {
        skipSpacing();
        if (!(_decoder.character() == Char::NamePathSeparator ||
            _decoder.character() == Char::OpenSBracket ||
            _decoder.character() == Char::EndOfData)) {

            _decoder.throwSyntaxError(
                u8"Unexpected character after the last element. Expected name separator or the end of the path.");
        }
    }

    [[nodiscard]] auto expectGenericIndex() -> std::size_t {
        _decoder.next(); // Skip the opening bracket.
        skipSpacing();
        auto result = lexer::parseNumber(
            _decoder,
            NumberBase::Decimal,
            lexer::Sign::Positive,
            lexer::NumberSeparators::Yes);
        skipSpacing();
        if (_decoder.character() != Char::ClosingSBracket) {
            _decoder.throwSyntaxError(u8"An index must end with a closing bracket.");
        }
        _decoder.next();
        if (result.value() < 0) {
            _decoder.throwSyntaxError(u8"Index values must not be negative.");
        }
        const auto indexValue = static_cast<std::uint64_t>(result.value());
        if (indexValue > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
            _decoder.throwLimitExceededError(u8"The index value is too large.");
        }
        return static_cast<std::size_t>(indexValue);
    }

    [[nodiscard]] auto expectRegularName() -> Name {
        auto [isMetaName, name] = lexer::expectRegularOrMetaName(_decoder, lexer::AcceptedNameEnd::NamePath);
        expectNameSeparatorIndexOrEnd();
        _afterFirstElement = true;
        return Name{NameType::Regular, Name::Storage{std::move(name)}, PrivateTag{}};
    }

    [[nodiscard]] auto expectTextNameOrIndex() -> Name {
        _decoder.next(); // Skip the opening quote.
        String text;
        lexer::parseText(_decoder, text);
        if (text.empty()) { // empty string? must be a text index.
            if (_decoder.character() != Char::OpenSBracket) {
                _decoder.throwSyntaxError(
                    u8"Empty text names are not allowed, unless followed by an index.");
            }
            auto index = expectGenericIndex();
            expectNameSeparatorOrEnd();
            _afterFirstElement = true;
            return Name::createTextIndex(index);
        }
        expectNameSeparatorOrEnd();
        _afterFirstElement = true;
        return Name::createText(std::move(text));
    }

    [[nodiscard]] auto expectIndex() -> Name {
        auto index = expectGenericIndex();
        expectNameSeparatorIndexOrEnd();
        _afterFirstElement = true;
        return Name::createIndex(index);
    }

    void skipSpacing() {
        while (_decoder.character() == CharClass::Spacing) {
            _decoder.next();
        }
    }

private:
    bool _afterFirstElement = false;
    FastNameDecoder _decoder;
};


}


