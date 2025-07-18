// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Content.hpp"
#include "InternalView.hpp"
#include "TokenType.hpp"

#include <cassert>


namespace erbsland::conf::impl {


/// A single lexer token.
///
/// @tested LexerTokenTest
///
class LexerToken final {
public: // construction
    /// Create a new lexer token.
    ///
    /// @param type The token type.
    /// @param begin The first character of the token.
    /// @param end After the last character of the token.
    /// @param rawText The raw text that was used to parse the token.
    /// @param value The converted value of the token.
    ///
    template <typename T>
    constexpr LexerToken(
        const TokenType type,
        const Position begin,
        const Position end,
        String &&rawText,
        T &&value = NoContent{}) noexcept
    :
        _type{type},
        _begin{begin},
        _end{end},
        _rawText{std::move(rawText)},
        _content{std::forward<T>(value)} {
    }

    /// Create a simple lexer token of a given type with no content.
    ///
    /// Meant to explicitly create end-of-data and error tokens.
    ///
    /// @param type Either `TokenType::EndOfData` or `TokenType::EndOfData`
    ///
    explicit constexpr LexerToken(const TokenType type) noexcept : _type{type} {
        assert(type == TokenType::EndOfData || type == TokenType::Error);
    }

    // defaults
    LexerToken() = default; // Implicitly created an end-of-data token.
    ~LexerToken() = default;
    LexerToken(const LexerToken &) = default;
    LexerToken(LexerToken &&) = default;
    auto operator=(const LexerToken &) -> LexerToken& = default;
    auto operator=(LexerToken &&) -> LexerToken& = default;

public: // accessors
    [[nodiscard]] constexpr auto type() const noexcept -> TokenType { return _type; }
    [[nodiscard]] constexpr auto begin() const noexcept -> Position { return _begin; }
    [[nodiscard]] constexpr auto end() const noexcept -> Position { return _end; }
    [[nodiscard]] constexpr auto rawText() const noexcept -> const String& { return _rawText; }
    [[nodiscard]] constexpr auto content() const noexcept -> const Content& { return _content; }

public:
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const LexerToken &token) noexcept -> InternalViewPtr;
#endif

private:
    TokenType _type{TokenType::EndOfData};
    Position _begin;
    Position _end;
    String _rawText;
    Content _content{NoContent{}};
};


}

