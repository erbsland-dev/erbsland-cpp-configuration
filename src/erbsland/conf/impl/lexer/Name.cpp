// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Name.hpp"


#include "Text.hpp"

#include "../YieldMacros.hpp"


namespace erbsland::conf::impl::lexer {


auto expectRegularOrMetaName(Decoder &decoder, const AcceptedNameEnd acceptedNameEnd) -> NameResult {
    bool isMetaName = false;

    auto nameTransaction = Transaction{decoder};
    if (decoder.character() == Char::At) {
        decoder.next();
        isMetaName = true;
        decoder.expect(CharClass::Letter, u8"Unexpected character in meta name after at-character.");
    }
    // Allow reading one more than the maximum numbers of allowed characters to detect names that are too long.
    while (std::cmp_less_equal(nameTransaction.capturedSize(), limits::maxNameLength)) {
        if (decoder.character() == Char::Space) {
            // Use a transaction, as we don't know if the name continues after the space.
            auto spaceTransaction = Transaction{decoder};
            decoder.next();
            if (decoder.character() != CharClass::LetterOrDigit) {
                break; // In case something else follows the space, this must be the end of the name.
            }
            spaceTransaction.commit(); // Ok, the name continues.
        } else if (decoder.character() == Char::Tab) {
            break; // Other than a space, a tab is for sure the end of the name.
        } else if (decoder.character() == Char::Underscore) {
            decoder.next();
            if (std::cmp_greater(nameTransaction.capturedSize(), limits::maxNameLength)) {
                break; // stop if the underscore makes the name too long.
            }
            if (decoder.character() != CharClass::LetterOrDigit) {
                if (decoder.character() == CharClass::LineBreakOrEnd || decoder.character() == CharClass::NameValueSeparator || decoder.character() == CharClass::Spacing) {
                    decoder.throwSyntaxError(u8"A name must not end with an underscore.");
                }
                if (decoder.character() == Char::Underscore) {
                    decoder.throwSyntaxError(u8"A name must not contain two or more subsequent word separators.");
                }
                decoder.throwSyntaxError(u8"Unexpected character in this name.");
            }
        } else {
            if (acceptedNameEnd == AcceptedNameEnd::NamePath) {
                // Accept all names ends in a name path `name ...`
                if (decoder.character() == Char::EndOfData ||
                    decoder.character() == Char::NamePathSeparator ||
                    decoder.character() == Char::OpenSBracket) {
                    break;
                }
            }
            if (acceptedNameEnd == AcceptedNameEnd::Section) {
                // Accept all names ends inside a section `[...]`
                if (decoder.character() == CharClass::NameValueSeparator ||
                    decoder.character() == Char::NamePathSeparator ||
                    decoder.character() == Char::ClosingSBracket) {
                    break;
                }
            }
        }
        decoder.expect(CharClass::LetterOrDigit, u8"Unexpected character following a regular name.");
        while (decoder.character() == CharClass::LetterOrDigit) {
            decoder.next();
            if (std::cmp_greater(nameTransaction.capturedSize(), limits::maxNameLength)) {
                break;
            }
        }
    }
    if (std::cmp_greater(nameTransaction.capturedSize(), limits::maxNameLength)) {
        decoder.throwError(ErrorCategory::LimitExceeded, u8"A name must not exceed 100 characters.");
    }
    // Convert the captured name into its normalized form.
    auto name = nameTransaction.captured([](String &text, const auto &character) {
        if (character == Char::Space) {
            text.append(u8'_');
        } else {
            character.appendLowerCaseTo(text);
        }
    });
    nameTransaction.commit();
    return {isMetaName, std::move(name)};
}


auto expectRegularOrMetaNameToken(TokenDecoder &decoder) -> LexerToken {
    auto [isMetaName, name] = expectRegularOrMetaName(decoder, AcceptedNameEnd::Section);
    if (isMetaName) {
        return decoder.createToken(TokenType::MetaName, std::move(name));
    }
    return decoder.createToken(TokenType::RegularName, std::move(name));
}


auto expectTextName(TokenDecoder &decoder) -> LexerToken {
    assert(decoder.character() == Char::DoubleQuote);
    String name;
    decoder.next();
    parseText(decoder, name);
    if (name.empty()) {
        if (decoder.character() == Char::DoubleQuote) {
            decoder.throwSyntaxError(u8"A text name must not be a multi-line text.");
        }
        decoder.throwSyntaxError(u8"A text name must not be empty.");
    }
    return decoder.createToken(TokenType::TextName, std::move(name));
}



}

