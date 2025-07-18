// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Meta)
class LexerCoreMetaTest final : public UNITTEST_SUBCLASS(LexerTestHelper) {
public:
    void testCoreMetaDirectives() {
        setupTokenIterator(u8"@version: \"1.0\"\n");
        WITH_CONTEXT(requireNextStringToken(TokenType::MetaName, u8"@version", u8"@version"));
        WITH_CONTEXT(requireNextToken(TokenType::NameValueSeparator, u8":"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextStringToken(TokenType::Text, u8"1.0", u8"\"1.0\""));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"@features: \"regex timedelta\"\n");
        WITH_CONTEXT(requireNextStringToken(TokenType::MetaName, u8"@features", u8"@features"));
        WITH_CONTEXT(requireNextToken(TokenType::NameValueSeparator, u8":"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextStringToken(TokenType::Text, u8"regex timedelta", u8"\"regex timedelta\""));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"@signature: \"data\"\n");
        WITH_CONTEXT(requireNextStringToken(TokenType::MetaName, u8"@signature", u8"@signature"));
        WITH_CONTEXT(requireNextToken(TokenType::NameValueSeparator, u8":"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextStringToken(TokenType::Text, u8"data", u8"\"data\""));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"@parser_test: \"data\"\n");
        WITH_CONTEXT(requireNextStringToken(TokenType::MetaName, u8"@parser_test", u8"@parser_test"));
        WITH_CONTEXT(requireNextToken(TokenType::NameValueSeparator, u8":"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextStringToken(TokenType::Text, u8"data", u8"\"data\""));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());
    }
};


