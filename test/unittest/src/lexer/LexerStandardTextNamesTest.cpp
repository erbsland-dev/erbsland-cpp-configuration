// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


// NOTES:
// - Omitted tests for layouts that aren't valid in the language. (e.g., section with text name at the beginning).


TESTED_TARGETS(Lexer) TAGS(TextNames)
class LexerStandardTextNamesTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    void testSections() {
        // focus on lexing around the text name, as most cases are already covered by the other tests.
        setupTokenIterator(u8"[section.\"with text\"]\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextStringToken(TokenType::TextName, u8"with text", u8"\"with text\""));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"[section  .   \"with text\"     ]\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"  "));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"   "));
        WITH_CONTEXT(requireNextStringToken(TokenType::TextName, u8"with text", u8"\"with text\""));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"     "));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());
    }

    void testTextWithSpecialCharacters() {
        // spacing, leading and trailing whitespace, and escape sequences are all supported.
        setupTokenIterator(u8"[section.\"    \\t\\t    \"]\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextStringToken(TokenType::TextName, u8"    \t\t    ", u8R"("    \t\t    ")"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        // more escape sequences
        setupTokenIterator(u8R"([section."😄\u0041\r\n\u{41}⇒\""])" "\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextStringToken(TokenType::TextName, u8"😄A\r\nA⇒\"", u8R"("😄\u0041\r\n\u{41}⇒\"")"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());
    }

    void testSectionUnexpectedEnd() {
        setupTokenIterator(u8"[section.\"");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));

        setupTokenIterator(u8"[section.\"  text]");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));
    }

    void testTextValueName() {
        setupTokenIterator(u8"[section]\n\"text\": 123\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextStringToken(TokenType::TextName, u8"text", u8"\"text\""));
        WITH_CONTEXT(requireNextToken(TokenType::NameValueSeparator, u8":"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextIntegerToken(TokenType::Integer, 123, u8"123"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"[section]\n\"    text   \"  = 123\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextStringToken(TokenType::TextName, u8"    text   ", u8"\"    text   \""));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"  "));
        WITH_CONTEXT(requireNextToken(TokenType::NameValueSeparator, u8"="));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextIntegerToken(TokenType::Integer, 123, u8"123"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());
    }

    void testTextValueNameUnexpectedEnd() {
        setupTokenIterator(u8"[section]\n\"text");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));
    }
};


