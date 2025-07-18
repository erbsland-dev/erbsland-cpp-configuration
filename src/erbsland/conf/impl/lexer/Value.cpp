// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Value.hpp"


#include "Core.hpp"
#include "Name.hpp"
#include "ValueBytes.hpp"
#include "ValueDateTime.hpp"
#include "ValueFloat.hpp"
#include "ValueInteger.hpp"
#include "ValueLiteral.hpp"
#include "ValueMultiLine.hpp"
#include "ValueText.hpp"

#include "../YieldMacros.hpp"


namespace erbsland::conf::impl::lexer {


auto expectNameAndValue(TokenDecoder &decoder) -> TokenGenerator {
    decoder.clearIndentationPattern(); // Clear the indentation pattern at the start of a name/value line.
    if (decoder.character() == CharClass::Letter || decoder.character() == Char::At) {
        EL_YIELD(expectRegularOrMetaNameToken(decoder));
    } else {
        // assumes this must be a text name; otherwise this method was called from the wrong context.
        if(decoder.character() != Char::DoubleQuote) {
            decoder.throwInternalError(u8"Function 'expectNameAndValue' called from the wrong context.");
        }
        EL_YIELD(expectTextName(decoder));
    }
    EL_YIELD_OPTIONAL(scanForSpacing(decoder));
    decoder.expectAndNext(CharClass::NameValueSeparator,
        u8"Expected a value separator after the name, but got something else.");
    EL_YIELD_TOKEN(TokenType::NameValueSeparator);
    EL_YIELD_OPTIONAL(scanForSpacing(decoder));
    if (decoder.character() == Char::CommentStart || decoder.character() == CharClass::LineBreak) {
        EL_YIELD_FROM(expectEndOfLine(decoder, ExpectMore::Yes)); // The Value is defined on the next line.
        decoder.expectMore(u8"Expected a value on the next line.");
        EL_YIELD(expectAndCheckIndentation(decoder));
        EL_YIELD_FROM(expectValueOrValueList(decoder, NextLine::Yes, MultiLineAllowed::Yes));
    } else if (decoder.character() == Char::EndOfData) {
        decoder.throwUnexpectedEndOfDataError(u8"Expected a value after the name separator.");
    } else {
        EL_YIELD_FROM(expectValueOrValueList(decoder, NextLine::No, MultiLineAllowed::Yes));
    }
    co_return;
}


auto expectMultiLineValueList(TokenDecoder &decoder) -> TokenGenerator {
    if (decoder.character() != Char::Asterisk) {
        decoder.throwInternalError(u8"Called 'expectMultiLineValueList' in the wrong state.");
    }
    decoder.next();
    EL_YIELD_TOKEN(TokenType::MultiLineValueListSeparator);
    EL_YIELD_OPTIONAL(scanForSpacing(decoder));
    decoder.expectMore(u8"Unexpected end in multi-line value list. Expected a value.");
    EL_YIELD_FROM(expectSingleLineValueOrValueList(decoder));
    // At this point, we are on the following line.
    if (decoder.character() == Char::EndOfData) {
        co_return; // This is a valid end of the document.
    }
    if (decoder.character() != CharClass::Spacing) {
        co_return; // If there is something else following the list, it is a valid end of the list.
    }
    if (!decoder.hasIndentationPattern()) {
        decoder.throwInternalError(u8"Expected to have an indentation patten at this point.");
    }
    // If the next line starts with spacing, it is potentially a continuation of the value list.
    while (decoder.character() == CharClass::Spacing) {
        auto transaction = Transaction{decoder};
        while (decoder.character() == CharClass::Spacing) {
            decoder.next();
        }
        if (decoder.character() == CharClass::EndOfLineStart) {
            // This is a valid empty line. Therefore, also a valid end of the list.
            transaction.rollback();
            co_return;
        }
        if (transaction.capturedString() != decoder.indentationPattern()) {
            EL_YIELD_TOKEN(TokenType::Indentation); // Consume the spacing as an indentation token.
            transaction.commit();
            decoder.throwError(ErrorCategory::Indentation,
                u8"The indentation pattern does not match the one on the previous line.");
        }
        if (decoder.character() != Char::Asterisk) {
            EL_YIELD_TOKEN(TokenType::Indentation); // Consume the spacing as an indentation token.
            transaction.commit();
            decoder.throwSyntaxError(
                u8"Expected the asterisk for a value list continuation, but got something else.");
        }
        transaction.commit();
        EL_YIELD_TOKEN(TokenType::Indentation); // Consume the spacing.
        decoder.next();
        EL_YIELD_TOKEN(TokenType::MultiLineValueListSeparator); // Consume the asterisk.
        EL_YIELD_OPTIONAL(scanForSpacing(decoder));
        decoder.expectMore(u8"Unexpected end in multi-line value list. Expected a value.");
        EL_YIELD_FROM(expectSingleLineValueOrValueList(decoder));
    }
    co_return;
}


auto expectValueOrValueList(
    TokenDecoder &decoder,
    const NextLine nextLine,
    const MultiLineAllowed multiLineAllowed) -> TokenGenerator {

    if (nextLine == NextLine::Yes && decoder.character() == Char::Asterisk) {
        EL_YIELD_FROM(expectMultiLineValueList(decoder));
        co_return;
    }
    // Check for multi-line values at this point.
    if (multiLineAllowed == MultiLineAllowed::Yes && decoder.character() == CharClass::OpeningBracket) {
        if (auto multiLineOpenToken = scanMultiLineOpen(decoder)) {
            const auto tokenType = multiLineOpenToken.value().type();
            co_yield std::move(multiLineOpenToken).value();
            switch (tokenType.raw()) {
            case TokenType::MultiLineTextOpen:
                EL_YIELD_FROM(expectMultiLineText(decoder, tokenType));
                break;
            case TokenType::MultiLineCodeOpen:
                EL_YIELD_FROM(expectMultiLineText(decoder, tokenType));
                break;
            case TokenType::MultiLineRegexOpen:
                EL_YIELD_FROM(expectMultiLineText(decoder, tokenType));
                break;
            case TokenType::MultiLineBytesOpen:
                EL_YIELD_FROM(expectMultiLineBytes(decoder));
                break;
            default:
                throw std::runtime_error("Unexpected token type after opening bracket.");
            }
            co_return;
        }
    }
    EL_YIELD_FROM(expectSingleLineValueOrValueList(decoder));
    co_return;
}


auto expectSingleLineValueOrValueList(TokenDecoder &decoder) -> TokenGenerator {
    EL_YIELD(expectSingleLineValue(decoder));
    EL_YIELD_OPTIONAL(scanForSpacing(decoder));
    while (decoder.character() == Char::ValueListSeparator) { // Is this a list?
        decoder.next();
        EL_YIELD_TOKEN(TokenType::ValueListSeparator);
        EL_YIELD_OPTIONAL(scanForSpacing(decoder));
        if (decoder.character() == CharClass::LineBreakOrEnd) {
            decoder.throwSyntaxOrUnexpectedEndError(u8"Expected another value after the value list separator.");
        }
        EL_YIELD(expectSingleLineValue(decoder));
        EL_YIELD_OPTIONAL(scanForSpacing(decoder));
    }
    decoder.expect(CharClass::EndOfLineStart, u8"Expected end of line or a value separator, but got something else.");
    EL_YIELD_FROM(expectEndOfLine(decoder, ExpectMore::No));
    co_return;
}


auto expectSingleLineValue(TokenDecoder &decoder) -> LexerToken {
    // The ORDER of the following scan functions is IMPORTANT!
    const auto valueScannerFunctions = {
        &scanLiteralFloat, // test for literal floats first.
        &scanLiteral, // test other literals. Throws error if value starts with letter and does not match.
        &scanDateOrDateTime, // test a date `2026-01-01`, or date time `2026-01-01t12:00`
        &scanTime, // test for a single time `10:00:32z`
        &scanFloatFractionOnly, // test for floats, like `.1928`
        &scanFloatWithWholePart, // test for floats, like `283.1293`
        &scanIntegerOrTimeDelta, // test for `123` or `123 days`
        &scanSingleLineText, // test for "text", `code` or /regex/
        &scanBytes, // test for bytes blocks like `<c8 14>`
    };
    for (const auto &scannerFunction : valueScannerFunctions) {
        if (auto optToken = (*scannerFunction)(decoder)) {
            return std::move(optToken).value();
        }
    }
    decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a value, but got something else.");
}


}

