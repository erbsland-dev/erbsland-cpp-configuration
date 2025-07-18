// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerTestHelper.hpp"


TESTED_TARGETS(Lexer)
class LexerBasicTest final : public UNITTEST_SUBCLASS(LexerTestHelper) {
public:
    void testZeroFile() {
        setupTokenIterator(String{}); // zero length file.
        WITH_CONTEXT(requireEndOfData());
    }

    void testJustSpacing() {
        setupTokenIterator(u8"    ");
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireEndOfData());
    }

    void testJustSpacingWithLineBreaks() {
        setupTokenIterator(u8"    \n    \n    ");
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireEndOfData());
        setupTokenIterator(u8"    \r\n    \r\n    ");
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\r\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\r\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireEndOfData());
    }

    void testEmptyLinesWithComments() {
        setupTokenIterator(u8"    \n\n    # comment\n\n      # comment at end");
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireNextToken(TokenType::Comment, u8"# comment"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"      "));
        WITH_CONTEXT(requireNextToken(TokenType::Comment, u8"# comment at end"));
        WITH_CONTEXT(requireEndOfData());
    }

    void testErrorPropagationSyntax() {
        // Unexpected character - syntax error from lexer.
        setupTokenIterator(u8"    x");
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireError(ErrorCategory::Syntax));
    }

    void testErrorPropagationControlCharacter() {
        // Control character - exception from decoder.
        setupTokenIterator(u8"    \x01");
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireError(ErrorCategory::Character));
    }

    void testErrorPropagationEncodingError() {
        // Invalid UTF-8 encoding - exception from decoder.
        // The error should happen after reading the spacing.
        const auto invalidUtf8 = Bytes({std::byte{0x20U}, std::byte{0x20U}, std::byte{0x20U},
            std::byte{0x20U}, std::byte{0x81U}, std::byte{0x82U}});
        REQUIRE_NOTHROW(setupTokenIterator(invalidUtf8));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireError(ErrorCategory::Encoding));
    }

    void testAccessAfterRead() {
        setupLexer(u8"    \n    \n");
        // read all tokens.
        for (auto token : lexer->tokens()) {
            // ignore
        }
        try {
            for (auto token : lexer->tokens()) {
                REQUIRE(false); // This must not work.
            }
            REQUIRE(false); // This must not work either.
        } catch (const Error &error) {
            REQUIRE(error.category() == ErrorCategory::Internal);
        }
    }

    void testSyntaxMetaName() {
        setupTokenIterator(u8"@version: \"1.0\"\n");
        WITH_CONTEXT(requireNextStringToken(TokenType::MetaName, u8"@version", u8"@version"));
        WITH_CONTEXT(requireNextToken(TokenType::NameValueSeparator, u8":"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextStringToken(TokenType::Text, u8"1.0", u8"\"1.0\""));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"@signature: \"data\"\n[main]\n");
        WITH_CONTEXT(requireNextStringToken(TokenType::MetaName, u8"@signature", u8"@signature"));
        WITH_CONTEXT(requireNextToken(TokenType::NameValueSeparator, u8":"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextStringToken(TokenType::Text, u8"data", u8"\"data\""));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextToken(TokenType::RegularName, u8"main"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());
    }

    void testComment() {
        setupTokenIterator(u8"# comment\n");
        WITH_CONTEXT(requireNextToken(TokenType::Comment, u8"# comment"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());
    }

    void testSectionMap() {
        setupTokenIterator(u8"[section]\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"----[section]\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"----["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"[section]----\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]----"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"-[ . \t  relative . section.with.elements ]-   # and a comment\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"-["));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" \t  "));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"relative", u8"relative"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"with", u8"with"));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"elements", u8"elements"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]-"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"   "));
        WITH_CONTEXT(requireNextToken(TokenType::Comment, u8"# and a comment"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"[section]*\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        // don't accept the trailing asterisk
        WITH_CONTEXT(requireError(ErrorCategory::Syntax));
    }

    void testSectionList() {
        setupTokenIterator(u8"*[section]\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"*["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"*[section]*\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"*["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]*"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"----*[section]\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"----*["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"----*[section]*\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"----*["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]*"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"*[section]----\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"*["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]----"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"*[section]*----\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"*["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]*----"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());

        setupTokenIterator(u8"-*[ . \t  relative . section.with.elements ]*-   # and a comment\n");
        WITH_CONTEXT(requireNextToken(TokenType::SectionListOpen, u8"-*["));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" \t  "));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"relative", u8"relative"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"with", u8"with"));
        WITH_CONTEXT(requireNextToken(TokenType::NamePathSeparator, u8"."));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"elements", u8"elements"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
        WITH_CONTEXT(requireNextToken(TokenType::SectionListClose, u8"]*-"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"   "));
        WITH_CONTEXT(requireNextToken(TokenType::Comment, u8"# and a comment"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireEndOfData());
    }

    void testDocumentWithDigest() {
        // verify the used algorithm.
        REQUIRE_EQUAL(impl::defaults::documentHashAlgorithm, impl::crypto::ShaHash::Algorithm::Sha3_256);
        setupTokenIterator(u8"@signature: \"data\"\n[main]\nvalue: 123\nanother value: \"example\"\n");
        while (tokenIterator != tokenIteratorEnd) {
            token = *tokenIterator;
            ++tokenIterator;
        }
        REQUIRE_EQUAL(lexer->digest(), Bytes::fromHex("b352bf8f49d930ec1267659eddaee1a1a6f38840e7d67ef5733ca2cee83f6633"));
    }
};


