// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Core.hpp"


#include "../YieldMacros.hpp"


namespace erbsland::conf::impl::lexer {


auto expectEndOfLine(TokenDecoder &decoder, ExpectMore expectMore) -> TokenGenerator {
    if (decoder.character() == Char::EndOfData) {
        if (expectMore == ExpectMore::Yes) {
            decoder.throwUnexpectedEndOfDataError(u8"Expected the data to be continued on the next line.");
        }
        co_return; // The line end can align with the end of the data.
    }
    // Spacing at the end of a line is allowed
    EL_YIELD_OPTIONAL(scanForSpacing(decoder));
    if (decoder.character() == Char::EndOfData) {
        if (expectMore == ExpectMore::Yes) {
            decoder.throwUnexpectedEndOfDataError(u8"Expected the data to be continued on the next line.");
        }
        co_return; // The line end can align with the end of the data.
    }
    // After the spacing, a comment is allowed.
    if (decoder.character() == Char::CommentStart) {
        EL_YIELD(expectComment(decoder));
    }
    if (decoder.character() == Char::EndOfData) {
        if (expectMore == ExpectMore::Yes) {
            decoder.throwUnexpectedEndOfDataError(u8"Expected the data to be continued on the next line.");
        }
        co_return; // The line end can align with the end of the data.
    }
    // At this point, a line-break is expected, or the data must end.
    decoder.expect(CharClass::LineBreak, u8"Expected the end of the line, but got something else.");
    EL_YIELD(expectLinebreak(decoder));
    co_return;
}


auto expectLinebreak(TokenDecoder &decoder) -> LexerToken {
    // In the case of a CR, expect a next character that is a newline.
    if (decoder.character() == Char::CarriageReturn) {
        decoder.next();
        decoder.expect(Char::NewLine, u8"Expected a newline after a carriage return.");
    } else {
        decoder.expect(Char::NewLine, u8"Expected a carriage return or newline.");
    }
    return decoder.createEndOfLineToken();
}


auto scanForSpacing(TokenDecoder &decoder) -> std::optional<LexerToken> {
    if (decoder.character() == CharClass::Spacing) {
        return expectSpacing(decoder);
    }
    return std::nullopt;
}


auto expectSpacing(TokenDecoder &decoder) -> LexerToken {
    decoder.expect(CharClass::Spacing, u8"Expected spacing, but got something else.");
    while (decoder.character() == CharClass::Spacing) {
        decoder.next();
    }
    return decoder.createToken(TokenType::Spacing);
}


void skipSpacing(TokenDecoder &decoder) {
    while (decoder.character() == CharClass::Spacing) {
        decoder.next();
    }
    decoder.checkForErrorAndThrowIt();
}


auto expectAndCheckIndentation(TokenDecoder &decoder) -> LexerToken {
    decoder.expect(CharClass::Spacing, u8"Expected indentation, but got something else.");
    // If there is already a pattern set, verify it and only read the pattern characters.
    if (decoder.hasIndentationPattern()) {
        for (auto patternChar : decoder.indentationPattern()) {
            if (decoder.character() != static_cast<char32_t>(patternChar)) {
                decoder.throwError(ErrorCategory::Indentation,
                                   u8"The indentation pattern on a continued line does not match the previous one.");
            }
            decoder.next();
        }
        return decoder.createToken(TokenType::Indentation);
    }
    // if no indentation pattern is defined, read and set one.
    while (decoder.character() == CharClass::Spacing) {
        decoder.next();
    }
    decoder.checkForErrorAndThrowIt();
    auto token = decoder.createToken(TokenType::Indentation);
    decoder.setIndentationPattern(token.rawText());
    return token;
}


auto expectComment(TokenDecoder &decoder) -> LexerToken {
    decoder.expectAndNext(Char::Hash, u8"Expected end of line or a comment, but got something else.");
    while (decoder.character() != CharClass::LineBreakOrEnd) {
        decoder.checkForErrorAndThrowIt();
        decoder.next();
    }
    decoder.checkForErrorAndThrowIt();
    return decoder.createToken(TokenType::Comment);
}


auto scanFormatOrLanguageIdentifier(TokenDecoder &decoder, const bool throwOnLength) -> String {
    if (decoder.character() != CharClass::Letter) {
        return {};
    }
    String identifier;
    decoder.character().appendLowerCaseTo(identifier);
    decoder.next();
    while (decoder.character() == CharClass::FormatIdentifierChar) {
        if (identifier.size() >= 16) { // as we only capture 7-bit chars, we can rely on the size.
            if (throwOnLength) {
                decoder.throwError(ErrorCategory::LimitExceeded, u8"Language or format identifier too long.");
            }
            return {};
        }
        decoder.character().appendLowerCaseTo(identifier);
        decoder.next();
    }
    if (decoder.character() == Char::EndOfData) {
        decoder.throwUnexpectedEndOfDataError(u8"Unexpected end after format or language identifier.");
    }
    return identifier;
}


}

