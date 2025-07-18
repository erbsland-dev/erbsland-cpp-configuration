// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "../TestHelper.hpp"

#include <erbsland/conf/Error.hpp>
#include <erbsland/conf/Source.hpp>
#include <erbsland/conf/impl/source/FileSource.hpp>
#include <erbsland/conf/impl/decoder/TokenDecoder.hpp>

#include <sstream>


using namespace el::conf;
using impl::Char;
using impl::CharClass;
using impl::TokenDecoder;
using impl::TokenDecoderPtr;
using impl::CharStream;
using impl::LexerToken;
using impl::FileSource;
using impl::TokenType;
using impl::internalView;


TESTED_TARGETS(TokenDecoder)
class TokenDecoderTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    std::filesystem::path testFile;
    SourcePtr source;
    TokenDecoderPtr decoder;

    void tearDown() override {
        // free all resources here to avoid side effects from deconstruction in the next test.
        decoder.reset();
        source.reset();
        testFile.clear();
        cleanUpTestFileDirectory();
    }

    auto additionalErrorMessages() -> std::string override {
        try {
            std::ostringstream oss;
            oss << "Test file path: " << testFile.string() << "\n";
            oss << "Source: " << source->identifier()->toText().toCharString() << "\n";
            oss << "Buffered Token Decoder State:\n" << internalView(*decoder)->toString(2).toCharString();
            return oss.str();
        } catch (...) {
            return "Unexpected exception.";
        }
    }

    template<typename T>
    void setupDecoder(const T &content) {
        testFile = createTestFile(content);
        REQUIRE(!testFile.empty());
        source = FileSource::fromFile(testFile);
        REQUIRE_NOTHROW(source->open());
        REQUIRE(source != nullptr);
        decoder = TokenDecoder::create(CharStream::create(source));
        REQUIRE(decoder != nullptr);
        decoder->initialize();
    }

    void requireAndNext(char32_t expectedUnicode) {
        REQUIRE(decoder->character().isChar(expectedUnicode));
        decoder->next();
    }

    void requireEndOfLine() {
        REQUIRE(decoder->character() == CharClass::LineBreak);
        auto token = decoder->createEndOfLineToken();
        REQUIRE(token.type() == TokenType::LineBreak);
        REQUIRE(token.rawText() == u8"\n");
    }

    void requireEndOfData() {
        REQUIRE(decoder->character() == Char::EndOfData);
        auto token = decoder->createEndOfDataToken();
        REQUIRE(token.type() == TokenType::EndOfData);
        REQUIRE(token.rawText().empty());
        REQUIRE(token.begin().isUndefined());
        REQUIRE(token.end().isUndefined());
    }

    void testEmptyFile() {
        setupDecoder(String{});
        REQUIRE(decoder->location().sourceIdentifier() == source->identifier());
        REQUIRE(decoder->location().position() == Position{1, 1});
        REQUIRE(decoder->character() == Char::EndOfData);
    }

    void testSimpleSequence() {
        setupDecoder(u8"abc\n😀\nxyz");
        REQUIRE(decoder->location().position() == Position{1, 1});
        WITH_CONTEXT(requireAndNext(U'a'));
        REQUIRE(decoder->location().position() == Position{1, 2});
        WITH_CONTEXT(requireAndNext(U'b'));
        REQUIRE(decoder->location().position() == Position{1, 3});
        WITH_CONTEXT(requireAndNext(U'c'));
        REQUIRE(decoder->location().position() == Position{1, 4});
        auto token = decoder->createToken(TokenType::Text, u8"");
        REQUIRE(token.rawText() == u8"abc");
        WITH_CONTEXT(requireEndOfLine());
        WITH_CONTEXT(requireAndNext(U'😀'));
        REQUIRE(decoder->location().position() == Position{2, 2});
        token = decoder->createToken(TokenType::Text, u8"");
        REQUIRE(token.rawText() == u8"😀");
        WITH_CONTEXT(requireEndOfLine());
        WITH_CONTEXT(requireAndNext(U'x'));
        REQUIRE(decoder->location().position() == Position{3, 2});
        WITH_CONTEXT(requireAndNext(U'y'));
        REQUIRE(decoder->location().position() == Position{3, 3});
        WITH_CONTEXT(requireAndNext(U'z'));
        REQUIRE(decoder->location().position() == Position{3, 4});
        token = decoder->createToken(TokenType::Text, u8"");
        REQUIRE(token.rawText() == u8"xyz");
        WITH_CONTEXT(requireEndOfData());
    }

    void testTransactionFromStart() {
        setupDecoder(u8"abcdef");
        {
            auto transaction = impl::Transaction{*decoder};
            WITH_CONTEXT(requireAndNext(U'a'));
            WITH_CONTEXT(requireAndNext(U'b'));
            WITH_CONTEXT(requireAndNext(U'c'));
        } // rollback
        {
            auto transaction = impl::Transaction{*decoder};
            WITH_CONTEXT(requireAndNext(U'a'));
            WITH_CONTEXT(requireAndNext(U'b'));
        } // rollback
        {
            auto transaction = impl::Transaction{*decoder};
            WITH_CONTEXT(requireAndNext(U'a'));
            WITH_CONTEXT(requireAndNext(U'b'));
            WITH_CONTEXT(requireAndNext(U'c'));
            WITH_CONTEXT(requireAndNext(U'd'));
        } // rollback
        WITH_CONTEXT(requireAndNext(U'a'));
        WITH_CONTEXT(requireAndNext(U'b'));
        {
            auto transaction = impl::Transaction{*decoder};
            WITH_CONTEXT(requireAndNext(U'c'));
            WITH_CONTEXT(requireAndNext(U'd'));
            WITH_CONTEXT(requireAndNext(U'e'));
            WITH_CONTEXT(requireAndNext(U'f'));
        } // rollback
        {
            auto transaction = impl::Transaction{*decoder};
            WITH_CONTEXT(requireAndNext(U'c'));
            WITH_CONTEXT(requireAndNext(U'd'));
            transaction.commit();
        } // no rollback
        WITH_CONTEXT(requireAndNext(U'e'));
        WITH_CONTEXT(requireAndNext(U'f'));
        WITH_CONTEXT(requireEndOfData());
    }

    void testNestedTransactions1() {
        setupDecoder(u8"abcdef");
        {
            auto transaction1 = impl::Transaction{*decoder};
            WITH_CONTEXT(requireAndNext(U'a'));
            WITH_CONTEXT(requireAndNext(U'b'));
            REQUIRE(transaction1.capturedString() == u8"ab");
            {
                auto transaction2 = impl::Transaction{*decoder};
                WITH_CONTEXT(requireAndNext(U'c'));
                WITH_CONTEXT(requireAndNext(U'd'));
                REQUIRE(transaction2.capturedString() == u8"cd");
                {
                    auto transaction3 = impl::Transaction{*decoder};
                    WITH_CONTEXT(requireAndNext(U'e'));
                    WITH_CONTEXT(requireAndNext(U'f'));
                    REQUIRE(transaction3.capturedString() == u8"ef");
                    transaction3.commit();
                }
                REQUIRE(transaction2.capturedString() == u8"cdef");
                transaction2.commit();
            }
            REQUIRE(transaction1.capturedString() == u8"abcdef");
            transaction1.commit();
        }
        WITH_CONTEXT(requireEndOfData());
    }

    void testNestedTransactions2() {
        setupDecoder(u8"abcdef");
        {
            auto transaction1 = impl::Transaction{*decoder};
            WITH_CONTEXT(requireAndNext(U'a'));
            WITH_CONTEXT(requireAndNext(U'b'));
            REQUIRE(transaction1.capturedString() == u8"ab");
            {
                auto transaction2 = impl::Transaction{*decoder};
                WITH_CONTEXT(requireAndNext(U'c'));
                WITH_CONTEXT(requireAndNext(U'd'));
                REQUIRE(transaction2.capturedString() == u8"cd");
                {
                    auto transaction3 = impl::Transaction{*decoder};
                    WITH_CONTEXT(requireAndNext(U'e'));
                    WITH_CONTEXT(requireAndNext(U'f'));
                    REQUIRE(transaction3.capturedString() == u8"ef");
                    transaction3.commit();
                }
                REQUIRE(transaction2.capturedString() == u8"cdef");
                // ROLLBACK
            }
            WITH_CONTEXT(requireAndNext(U'c'));
            WITH_CONTEXT(requireAndNext(U'd'));
            {
                auto transaction2 = impl::Transaction{*decoder};
                WITH_CONTEXT(requireAndNext(U'e'));
                WITH_CONTEXT(requireAndNext(U'f'));
                REQUIRE(transaction2.capturedString() == u8"ef");
                // ROLLBACK
            }
            WITH_CONTEXT(requireAndNext(U'e'));
            WITH_CONTEXT(requireAndNext(U'f'));
            REQUIRE(transaction1.capturedString() == u8"abcdef");
            transaction1.commit();
        }
        WITH_CONTEXT(requireEndOfData());
    }

    void testDocumentWithDigest() {
        // verify the used algorithm.
        REQUIRE_EQUAL(impl::defaults::documentHashAlgorithm, impl::crypto::ShaHash::Algorithm::Sha3_256);
        setupDecoder(u8"@signature: \"...\"\n[main]\nvalue: 123\nanother value: \"example\"\n");
        while (decoder->character() != Char::EndOfData) {
            decoder->next();
        }
        REQUIRE_EQUAL(decoder->digest(), Bytes::fromHex("b352bf8f49d930ec1267659eddaee1a1a6f38840e7d67ef5733ca2cee83f6633"));
    }
};

