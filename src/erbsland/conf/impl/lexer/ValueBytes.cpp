// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ValueBytes.hpp"


#include "Core.hpp"
#include "ValueMultiLine.hpp"

#include "../YieldMacros.hpp"


namespace erbsland::conf::impl::lexer {



auto scanSingleLineFormatIdentifier(TokenDecoder &decoder) -> String {
    auto prefixTransaction = Transaction{decoder};
    auto formatIdentifier = scanFormatOrLanguageIdentifier(decoder, false);
    if (!formatIdentifier.empty() && decoder.character() == Char::Colon) {
        decoder.next(); // after the colon, the actual bytes start.
        prefixTransaction.commit();
        return formatIdentifier;
    }
    // Without colon at the end, this must be hex bytes.
    prefixTransaction.rollback();
    return {u8"hex"};
}


auto scanBytes(TokenDecoder &decoder) -> std::optional<LexerToken> {
    if (decoder.character() != Char::LessThan) {
        return std::nullopt;
    }
    decoder.next();
    decoder.expectMoreInLine(u8"Unexpected end in bytes value.");
    // Check for a format identifier after the opening angle bracket.
    auto formatIdentifier = scanSingleLineFormatIdentifier(decoder);
    if (formatIdentifier != u8"hex") {
        decoder.throwError(ErrorCategory::Unsupported, u8"Unknown bytes-data format.");
    }
    decoder.expectMoreInLine(u8"Unexpected end in bytes value.");
    auto bytes = Bytes{};
    while (decoder.character() != Char::GreaterThan) {
        decoder.expectMoreInLine(u8"Unexpected end in bytes value.");
        skipSpacing(decoder);
        if (decoder.character() == Char::GreaterThan) {
            break; // Valid end of bytes.
        }
        decoder.expectMoreInLine(u8"Unexpected end in bytes value.");
        if (decoder.character() != CharClass::HexDigit) {
            decoder.throwSyntaxError(u8"Expected first hex digit of a byte, got something else.");
        }
        auto value = static_cast<std::byte>(decoder.character().toHexDigitValue()) << 4;
        decoder.next();
        decoder.expectMoreInLine(u8"Unexpected end in bytes value.");
        if (decoder.character() != CharClass::HexDigit) {
            decoder.throwSyntaxError(u8"Expected second hex digit of a byte, got something else.");
        }
        value |= static_cast<std::byte>(decoder.character().toHexDigitValue());
        decoder.next();
        bytes.push_back(value);
    }
    decoder.next();
    return decoder.createToken(TokenType::Bytes, std::move(bytes));
}


auto parseMultiLineBytesHexLine(TokenDecoder &decoder) -> TokenGenerator {
    // Initial check so we avoid creating a Bytes object.
    if (!isAtMultiLineEnd(decoder, TokenType::MultiLineBytes)) {
        Bytes decodedBytes;
        // Carefully consume the text block by block, so we can skip trailing spacing.
        while (!isAtMultiLineEnd(decoder, TokenType::MultiLineBytes)) {
            skipSpacing(decoder);
            if (isAtMultiLineEnd(decoder, TokenType::MultiLineBytes)) {
                break;
            }
            if (decoder.character() != CharClass::HexDigit) {
                decoder.throwSyntaxError(u8"Expected first hex digit of a byte, got something else.");
            }
            auto value = static_cast<std::byte>(decoder.character().toHexDigitValue()) << 4;
            decoder.next();
            if (isAtMultiLineEnd(decoder, TokenType::MultiLineBytes)) {
                decoder.throwSyntaxError(u8"Expected second hex digit of a byte, not the end of the line.");
            }
            if (decoder.character() != CharClass::HexDigit) {
                decoder.throwSyntaxError(u8"Expected second hex digit of a byte, got something else.");
            }
            value |= static_cast<std::byte>(decoder.character().toHexDigitValue());
            decoder.next();
            decodedBytes.push_back(value);
        }
        EL_YIELD_TOKEN(TokenType::MultiLineBytes, std::move(decodedBytes));
    }
    // Read the end-of-line tokens (may include a comment if at #).
    EL_YIELD_FROM(expectEndOfLine(decoder, ExpectMore::No));
    // Do the check for more data after creating all tokens for the line.
    decoder.expectMore(u8"Unexpected end in a multi-line bytes-data.");
    co_return;
}


auto expectMultiLineBytes(TokenDecoder &decoder) -> TokenGenerator {
    // expect to be at the character just after the opening angle bracket.
    decoder.expectMore(u8"Unexpected end in bytes value.");
    if (auto formatIdentifier = scanFormatOrLanguageIdentifier(decoder, true); !formatIdentifier.empty()) {
        if (decoder.character() != CharClass::EndOfLineStart) {
            decoder.throwSyntaxError(u8"Unexpected characters in bytes-data format identifier.");
        }
        if (formatIdentifier != u8"hex") {
            decoder.throwError(ErrorCategory::Unsupported, u8"Unknown bytes-data format.");
        }
        EL_YIELD_TOKEN(TokenType::MultiLineBytesFormat, std::move(formatIdentifier));
    }
    if (decoder.character() != CharClass::EndOfLineStart) {
        decoder.throwSyntaxError(u8"Unexpected characters in bytes-data format identifier");
    }
    // Process any text following the opening bracket sequence.
    EL_YIELD_FROM(expectMultiLineAfterOpen(decoder));
    // Next, process the bytes data, line by line.
    // At the start of this while loop, the decoder should be at the indented continued line.
    while (decoder.character() != Char::EndOfData) {
        // Test if we get the closing bracket sequence.
        if (auto closeToken = scanMultiLineClose(decoder, TokenType::MultiLineBytesOpen); closeToken.has_value()) {
            co_yield std::move(closeToken).value();
            co_return;
        }
        EL_YIELD_FROM(parseMultiLineBytesHexLine(decoder));
        // if the following line starts with spacing, expect the correct indentation pattern.
        if (decoder.character() == CharClass::Spacing) {
            EL_YIELD(expectAndCheckIndentation(decoder));
            decoder.expectMore(u8"Unexpected end in multi-line byte-data.");
        } else if (decoder.character() != CharClass::LineBreak) {
            decoder.throwSyntaxError(u8"Missing indentation in multi-line byte-data.");
        }
    }
    // Unexpected, if the data ends, just after the opening sequence.
    decoder.throwUnexpectedEndOfDataError(u8"Unexpected end in multi-line byte-data.");
}


}


