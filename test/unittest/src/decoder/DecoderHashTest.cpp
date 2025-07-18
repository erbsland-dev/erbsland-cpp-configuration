// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "../TestHelper.hpp"

#include <erbsland/conf/Source.hpp>
#include <erbsland/conf/impl/CharStream.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;
using impl::Char;
using impl::CharStream;
using impl::CharStreamPtr;
using impl::DecodedChar;


TESTED_TARGETS(Decoder ShaHash)
class DecoderHashTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    CharStreamPtr decoder;
    DecodedChar decodedChar;

    void tearDown() override {
        cleanUpTestFileDirectory();
    }

    void verifyHash(const String &content, const Bytes &expectedDigest) {
        auto source = createTestMemorySource(content);
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        decodedChar = decoder->next();
        while (decodedChar != Char::EndOfData) {
            decodedChar = decoder->next();
        }
        REQUIRE_EQUAL(decoder->digest(), expectedDigest);
    }

    void testNoHash() {
        WITH_CONTEXT(verifyHash(
            "[main]\nvalue: 123\nanother value: \"example\"\n",
            Bytes{}));
    }

    void testWithHash() {
        // verify the used algorithm.
        REQUIRE_EQUAL(impl::defaults::documentHashAlgorithm, impl::crypto::ShaHash::Algorithm::Sha3_256);
        WITH_CONTEXT(verifyHash(
            "@signature \"...\"\n[main]\nvalue: 123\nanother value: \"example\"\n",
            Bytes::fromHex("b352bf8f49d930ec1267659eddaee1a1a6f38840e7d67ef5733ca2cee83f6633")));
        WITH_CONTEXT(verifyHash(
            "@signature \"only signature line changes\"\n[main]\nvalue: 123\nanother value: \"example\"\n",
            Bytes::fromHex("b352bf8f49d930ec1267659eddaee1a1a6f38840e7d67ef5733ca2cee83f6633")));
        WITH_CONTEXT(verifyHash(
            "@signature \"...\"\n[main] #comment\nvalue: 123\nanother value: \"example\"\n",
            Bytes::fromHex("217a5a3718139d97c00d4d9e28cbae75cb5694fd578d9d3e3130e59e57974104")));
    }

    void testSignatureInSecondLine() {
        WITH_CONTEXT(verifyHash(
            "# comment\n@signature \"...\"\n[main]\nvalue: 123\nanother value: \"example\"\n", Bytes{}));
    }

    void testDocumentWithBOM() {
        WITH_CONTEXT(verifyHash(
            "\xEF\xBB\xBF@signature \"...\"\n[main]\nvalue: 123\nanother value: \"example\"\n",
            Bytes::fromHex("b352bf8f49d930ec1267659eddaee1a1a6f38840e7d67ef5733ca2cee83f6633")));
    }
};

