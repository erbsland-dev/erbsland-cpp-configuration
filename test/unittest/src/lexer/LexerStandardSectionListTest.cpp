// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Section)
class LexerStandardSectionListTest final : public UNITTEST_SUBCLASS(LexerTestHelper) {
public:
    void testSectionLists() {
        // verify a few formats.
        setupTokenIterator(u8"*[section]\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"*["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"---*[   section]\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"---*["));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"   "));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"*[section   .  sub]---\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"*["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"   "));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"  "));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"sub", u8"sub"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]---"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"*[section   ]*---\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"*["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"   "));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]*---"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());
    }
};


