// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Text.hpp"


#include "Core.hpp"
#include "Number.hpp"
#include "ValueMultiLine.hpp"

#include "../utilities/YieldMacros.hpp"


namespace erbsland::conf::impl::lexer {


void parseString(
    Decoder &decoder,
    String &target,
    const char32_t terminator,
    const char32_t escapeChar,
    const EscapeFn &escapeFn) {

    while (decoder.character() != Char::EndOfData) {
        decoder.checkForErrorAndThrowIt();
        if (decoder.character() == CharClass::LineBreak) {
            decoder.throwSyntaxError(u8"Unexpected line break in text or code-block.");
        }
        if (decoder.character() == terminator) {
            decoder.next();
            return;
        }
        if (decoder.character() == escapeChar) {
            decoder.next();
            decoder.expectMore(u8"Unexpected end in an escape sequence.");
            escapeFn(decoder, target);
        } else {
            decoder.character().appendTo(target);
            decoder.next();
        }
    }
    decoder.throwUnexpectedEndOfDataError();
}


auto parseMultiLineString(
    TokenDecoder &decoder,
    const char32_t escapeChar,
    EscapeFn escapeFn,
    const TokenType tokenType) -> TokenGenerator {

    // Initial check if the line starts with the end marker, so we avoid creating a transaction and capture string.
    if (!isAtMultiLineEnd(decoder, tokenType)) {
        String decodedText;
        // Carefully consume the text block by block, so we can skip trailing spacing.
        while (!isAtMultiLineEnd(decoder, tokenType)) {
            // Consume anything that is not space, or the end of the line.
            while (decoder.character() != CharClass::Spacing && decoder.character() != CharClass::LineBreakOrEnd) {
                decoder.checkForErrorAndThrowIt();
                if (decoder.character() == escapeChar) {
                    decoder.next();
                    escapeFn(decoder, decodedText);
                } else {
                    decoder.character().appendTo(decodedText);
                    decoder.next();
                }
            }
            // If the line ends here, commit everything consumed so far.
            if (decoder.character() == CharClass::LineBreakOrEnd) {
                break;
            }
            // At this point we are in spacing territory, always expect that we read the trailing space of the line.
            auto trailingSpaceTransaction = Transaction{decoder};
            while (decoder.character() == CharClass::Spacing) {
                decoder.next();
            }
            if (isAtMultiLineEnd(decoder, tokenType)) {
                // If we reached the end of the line, while consuming spaces. We have to roll back this section,
                // as this is the trailing portion that is not part of the actual text.
                trailingSpaceTransaction.rollback();
                break;
            }
            decodedText.append(trailingSpaceTransaction.capturedString());
            trailingSpaceTransaction.commit();
        }
        EL_YIELD_TOKEN(tokenType, std::move(decodedText));
    }
    // Read the end-of-line tokens (may include a comment if at #).
    EL_YIELD_FROM(expectEndOfLine(decoder, ExpectMore::No));
    // Do the check for more data after creating all tokens for the line.
    decoder.expectMore(u8"Unexpected end in a multi-line text, code-block or regular expression.");
    co_return;
}


void parseText(Decoder &decoder, String &target) {
    parseString(
        decoder,
        target,
        Char::DoubleQuote,
        Char::Backslash,
        parseTextEscapeSequence);
}


/// Parse the Unicode escape sequence, after `\u` or `\U`.
///
/// @param decoder The decoder.
/// @param target The target to add the parsed Unicode character.
///
void parseUnicodeEscapeSequence(Decoder &decoder, String &target) {
    decoder.expectMore(u8"Unexpected end in a Unicode escape sequence.");
    Char character{};
    if (decoder.character() == Char::OpenCBracket) { // dynamic length hex.
        decoder.next();
        decoder.expect(CharClass::HexDigit, u8"Expected a hex digit after the opening bracket.");
        const auto numberResult = parseNumber(
            decoder,
            NumberBase::Hexadecimal,
            Sign::Positive,
            NumberSeparators::No);
        // TODO: Check if the return value can be used directly.
        const auto number = numberResult.value();
        const auto digits = numberResult.digitCount();
        decoder.expectMore(u8"Unexpected end in a Unicode escape sequence.");
        if (digits > 8) {
            decoder.throwSyntaxError(u8"Hex escape sequence is too long.");
        }
        decoder.expectAndNext(Char::ClosingCBracket, u8"Expected a closing bracket after the hexadecimal number.");
        character = Char{static_cast<char32_t>(number)};
    } else if (decoder.character() == CharClass::HexDigit) { // fixed length hex.
        const auto numberResult = parseNumber(
            decoder,
            NumberBase::Hexadecimal,
            Sign::Positive,
            NumberSeparators::No,
            4);
        // TODO: Check if the return value can be used directly.
        const auto number = numberResult.value();
        decoder.expectMore(u8"Unexpected end in a Unicode escape sequence.");
        if (number < 0) {
            decoder.throwSyntaxError(u8"Hex escape sequence requires four digits.");
        }
        character = Char{static_cast<char32_t>(number)};
    } else {
        decoder.throwSyntaxError(u8"Expected a hex digit or an opening bracket.");
    }
    if (!character.isValidEscapeUnicode()) {
        decoder.throwSyntaxError(u8"Invalid unicode value in escape sequence.");
    }
    character.appendTo(target);
}


void parseTextEscapeSequence(Decoder &decoder, String &target) {
    // Do end checks before calling `next()`, because of open transactions.
    decoder.expectMore(u8"Unexpected end in an escape sequence.");
    if (decoder.character() == CharClass::LineBreak) {
        decoder.throwSyntaxError(u8"Unexpected line break in escape sequence.");
    }
    // Get the escaped character first and consume it.
    const auto escapedChar = decoder.character();
    decoder.checkForErrorAndThrowIt();
    decoder.next();
    // Next, decide how to handle the escaped character.
    switch (escapedChar) {
    case Char::Backslash: target.append(u8'\\'); break;
    case Char::DoubleQuote: target.append(u8'"'); break;
    case Char::Dollar: target.append(u8'$'); break;
    case Char::LcT: case Char::UcT: target.append(u8'\t'); break;
    case Char::LcN: case Char::UcN: target.append(u8'\n'); break;
    case Char::LcR: case Char::UcR: target.append(u8'\r'); break;
    case Char::LcU: case Char::UcU:
        parseUnicodeEscapeSequence(decoder, target);
        break;
    default:
        decoder.throwSyntaxError(u8"Unexpected character in escape sequence.");
    }
}


void parseRegularExpression(Decoder &decoder, String &target) {
    parseString(
        decoder,
        target,
        Char::Slash,
        Char::Backslash,
        parseRegularExpressionEscapeSequence);
}


void parseRegularExpressionEscapeSequence(Decoder &decoder, String &target) {
    if (decoder.character() == Char::Slash) {
        target.append(u8'/');
    } else if (decoder.character() != Char::Error) {
        target.append(u8'\\');
        decoder.character().appendTo(target);
    } else {
        // This will throw any captured encoding or control-character exception.
        decoder.throwSyntaxError(u8"Unexpected character in escape sequence.");
    }
    decoder.next();
}


void parseCode(Decoder &decoder, String &target) {
    parseString(
        decoder,
        target,
        Char::Backtick,
        0, // no escape.
        nullptr);
}


}

