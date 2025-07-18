// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0



#include "ParserTestHelper.hpp"


TESTED_TARGETS(Parser)
class ParserBasicTest final : public UNITTEST_SUBCLASS(ParserTestHelper) {
public:
    MockSourcePtr source;
    std::shared_ptr<Parser> parser;

    void setUp() override {
        source = std::make_shared<MockSource>();
        parser = {};
        doc = {};
    }

    void tearDown() override {
        source = {};
        parser = {};
        doc = {};
    }

    /// Test if the source was used as expected, with one open and one close and no unnecessary reads.
    void verifySequentialRead() {
        REQUIRE_EQUAL(source->actions.size(), source->lines.size() + 2);
        REQUIRE(source->actions.front() == u8"open");
        REQUIRE(source->actions.back() == u8"close");
        for (std::size_t i = 1; i < source->actions.size() - 1; ++i) {
            REQUIRE(source->actions[i] == u8"readLine");
        }
    }

    void testDoNothing() {
        parser = std::make_shared<Parser>();
        parser = {};
    }

    void testEmptyDocument() {
        parser = std::make_shared<Parser>();
        REQUIRE_NOTHROW(doc = parser->parseOrThrow(source));
        WITH_CONTEXT(verifySequentialRead());
        REQUIRE(doc != nullptr);
        REQUIRE(doc->empty());
    }

    void testEmptyWithComments() {
        source->lines = {
            u8"# comment\n",
            u8"\n",
            u8"  \n",
            u8"    # comment at end\n"
        };
        parser = std::make_shared<Parser>();
        REQUIRE_NOTHROW(doc = parser->parseOrThrow(source));
        WITH_CONTEXT(verifySequentialRead());
        REQUIRE(doc != nullptr);
        REQUIRE(doc->empty());
    }

    void testEmptyWithMeta() {
        source->lines = {
            u8"@version: \"1.0\"\n",
            u8"@features: \"core float\"\n",
            u8"\n",
            u8"# comment at end\n"
        };
        parser = std::make_shared<Parser>();
        REQUIRE_NOTHROW(doc = parser->parseOrThrow(source));
        WITH_CONTEXT(verifySequentialRead());
        REQUIRE(doc != nullptr);
        REQUIRE(doc->empty());
    }

    void testSmallDocument() {
        source->lines = {
            u8"# A realistic configuration example for ELCL\n",
            u8"@version: \"1.0\"\n",
            u8"\n",
            u8"# a small document\n",
            u8"--[ main ]--\n",
            u8"Connect = \"host01.example.com\"\n",
            u8"Server Port = 1234\n",
            u8"\n",
            u8"[ main . Client ]\n",
            u8"name: \" example client \"\n",
            u8"Welcome Message: \"\"\"    # The welcome message\n",
            u8"    Hello user!  \n",
            u8"    This is the welcome message...\n",
            u8"    \"\"\"\n",
            u8"\n",
            u8"--*[server]*--\n",
            u8"host: \"host02.example.com\"\n",
            u8"--*[server]*--\n",
            u8"host: \"host03.example.com\"\n",
            u8"port: 0xfffe\n"
            u8"\n",
        };
        parser = std::make_shared<Parser>();
        REQUIRE_NOTHROW(doc = parser->parseOrThrow(source));
        REQUIRE_FALSE(doc->empty());
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.connect", u8"Text(\"host01\\u{2e}example\\u{2e}com\")"},
            {u8"main.server_port", u8"Integer(1234)"},
            {u8"main.client", u8"SectionWithNames()"},
            {u8"main.client.name", u8"Text(\" example client \")"},
            {u8"main.client.welcome_message", u8"Text(\"Hello user!\\u{a}This is the welcome message\\u{2e}\\u{2e}\\u{2e}\")"},
            {u8"server", u8"SectionList()"},
            {u8"server[0]", u8"SectionWithNames()"},
            {u8"server[0].host", u8"Text(\"host02\\u{2e}example\\u{2e}com\")"},
            {u8"server[1]", u8"SectionWithNames()"},
            {u8"server[1].host", u8"Text(\"host03\\u{2e}example\\u{2e}com\")"},
            {u8"server[1].port", u8"Integer(65534)"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testCharacterEncodingError() {
        source->lines = {
            Bytes::fromHex("2320 4572 6273 6C61 6E64 2043 6F6E 6669 6775 7261 7469 6F6E 204C 616E 6775 6167 6520 5465 7374 2046 696C 650A"),
            Bytes::fromHex("5B6D 6169 6E5D 0A"),
            Bytes::fromHex("7661 6C75 653A 2060 EDA080 60"), // error: EDA080 = U+D800 = surrogate!
        };
        parser = std::make_shared<Parser>();
        REQUIRE_THROWS_AS(Error, doc = parser->parseOrThrow(source));
    }
};

