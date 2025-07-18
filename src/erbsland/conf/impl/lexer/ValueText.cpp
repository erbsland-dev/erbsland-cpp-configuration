// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ValueText.hpp"


#include "Core.hpp"
#include "Text.hpp"
#include "ValueMultiLine.hpp"

#include "../YieldMacros.hpp"

#include <utility>


namespace erbsland::conf::impl::lexer {


auto scanSingleLineText(TokenDecoder &decoder) -> std::optional<LexerToken> {
    if (!decoder.character().isChar(Char::DoubleQuote, Char::Backtick, Char::Slash)) {
        return {};
    }
    auto terminatingCharacter = decoder.character();
    decoder.next();
    String text;
    if (terminatingCharacter == Char::DoubleQuote) {
        parseText(decoder, text);
        return decoder.createToken(TokenType::Text, std::move(text));
    }
    if (terminatingCharacter == Char::Slash) {
        parseRegularExpression(decoder, text);
        return decoder.createToken(TokenType::RegEx, std::move(text));
    }
    parseCode(decoder, text);
    return decoder.createToken(TokenType::Code, std::move(text));
}


auto expectMultiLineText(TokenDecoder &decoder, const TokenType openTokenType) -> TokenGenerator {
    assert(openTokenType == TokenType::MultiLineTextOpen || openTokenType == TokenType::MultiLineCodeOpen || openTokenType == TokenType::MultiLineRegexOpen);
    // In the case of code, accept a language identifier, just after the opening sequence.
    if (openTokenType == TokenType::MultiLineCodeOpen) {
        if (auto languageIdentifier = scanFormatOrLanguageIdentifier(decoder, true); !languageIdentifier.empty()) {
            EL_YIELD_TOKEN(TokenType::MultiLineCodeLanguage, std::move(languageIdentifier));
            decoder.expectMore(u8"Unexpected end in multi-line code block.");
        }
    }
    // Process any text following the opening bracket sequence.
    EL_YIELD_FROM(expectMultiLineAfterOpen(decoder));
    // Next, process the text, line by line.
    // At the start of this while loop, the decoder should be at the indented continued line.
    while (decoder.character() != Char::EndOfData) {
        // Test if we get the closing bracket sequence.
        if (auto closeToken = scanMultiLineClose(decoder, openTokenType); closeToken.has_value()) {
            co_yield std::move(closeToken).value();
            co_return;
        }
        // Capture text, trailing spacing (+comment) and line-break.
        switch (openTokenType.raw()) {
        case TokenType::MultiLineTextOpen:
            EL_YIELD_FROM(parseMultiLineString(
                decoder,
                Char::Backslash,
                parseTextEscapeSequence,
                TokenType::MultiLineText));
            break;
        case TokenType::MultiLineCodeOpen:
            EL_YIELD_FROM(parseMultiLineString(
                decoder,
                {},
                {},
                TokenType::MultiLineCode));
            break;
        case TokenType::MultiLineRegexOpen:
            EL_YIELD_FROM(parseMultiLineString(
                decoder,
                Char::Backslash,
                parseRegularExpressionEscapeSequence,
                TokenType::MultiLineRegex));
            break;
        default:
            throw std::runtime_error("Unexpected open token type.");
        }
        // if the following line starts with spacing, expect the correct indentation pattern.
        if (decoder.character() == CharClass::Spacing) {
            EL_YIELD(expectAndCheckIndentation(decoder));
            decoder.expectMore(u8"Unexpected end in multi-line text, code-block or regular expression.");
        } else if (decoder.character() != CharClass::LineBreak) {
            decoder.throwSyntaxError(u8"Missing indentation in multi-line text.");
        }
    }
    // Unexpected, if the data ends, just after the opening sequence.
    decoder.throwUnexpectedEndOfDataError();
}


}


