// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ValueMultiLine.hpp"


#include "Core.hpp"

#include "../utilities/YieldMacros.hpp"


namespace erbsland::conf::impl::lexer {


auto scanRepeatingCharacters(TokenDecoder &decoder, const char32_t expectedChar, const bool isOpen) -> std::optional<LexerToken> {
    // Avoid a transaction if there is no need.
    if (decoder.character() != expectedChar) {
        return std::nullopt;
    }
    // Count until we get three characters.
    int expectedCharCount = 0;
    auto transaction = Transaction{decoder};
    while (decoder.character() == expectedChar) {
        decoder.next();
        ++expectedCharCount;
        if (expectedCharCount == 3) {
            transaction.commit();
            if (isOpen) {
                return decoder.createToken(TokenType::fromMultiLineOpen(expectedChar));
            }
            return decoder.createToken(TokenType::fromMultiLineClose(expectedChar));
        }
    }
    return std::nullopt; // Too few.
}



auto scanMultiLineOpen(TokenDecoder &decoder) -> std::optional<LexerToken> {
    return scanRepeatingCharacters(decoder, decoder.character(), true);
}


auto scanMultiLineClose(TokenDecoder &decoder, TokenType openTokenType) -> std::optional<LexerToken> {
    Char expectedChar;
    switch (openTokenType.raw()) {
        case TokenType::MultiLineTextOpen:
            expectedChar = Char::DoubleQuote;
            break;
        case TokenType::MultiLineCodeOpen:
            expectedChar = Char::Backtick;
            break;
        case TokenType::MultiLineRegexOpen:
            expectedChar = Char::Slash;
            break;
        case TokenType::MultiLineBytesOpen:
            expectedChar = Char::GreaterThan;
            break;
        default:
            throw std::runtime_error("Unexpected open token type.");
    }
    return scanRepeatingCharacters(decoder, expectedChar, false);
}


auto expectMultiLineAfterOpen(TokenDecoder &decoder) -> TokenGenerator {
    EL_YIELD_FROM(expectEndOfLine(decoder, ExpectMore::Yes));
    decoder.expectMore(u8"Unexpected end in multi-line expression.");
    // Now we are at the start of a new line.
    if (decoder.character() == CharClass::Spacing) {
        EL_YIELD(expectAndCheckIndentation(decoder));
    } else if (decoder.character() != CharClass::LineBreak) {
        decoder.throwSyntaxError(u8"Expected continued text or data, but got something else.");
    }
    // Special case with an empty line after the opening bracket.
    // Don't consume this linebreak, pass it down to the multi-line-text logic.
    co_return;
}


auto isAtMultiLineEnd(const TokenDecoder &decoder, const TokenType tokenType) -> bool {
    const auto areCommentsAllowed = (tokenType == TokenType::MultiLineRegex || tokenType == TokenType::MultiLineBytes);
    return decoder.character() == CharClass::LineBreakOrEnd || (areCommentsAllowed && decoder.character() == Char::CommentStart);
}


}

