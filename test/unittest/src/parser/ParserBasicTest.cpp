// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0



#include "ParserTestHelper.hpp"

#include <erbsland/conf/SourceIdentifier.hpp>


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
        const auto location = doc->location();
        REQUIRE_FALSE(location.isUndefined());
        REQUIRE(SourceIdentifier::areEqual(location.sourceIdentifier(), source->identifier()));
        REQUIRE_EQUAL(location.position().isUndefined(), true);
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

    void testMixedTextAndRegular() {
        source->lines = {
            u8"# Erbsland Configuration Language Test File\n",
            u8"[main]\n",
            u8"value1 = 1\n",
            u8"value2 = 2\n",
            u8"value3 = 3\n",
            u8"[main.sub_text.\"one\"]\n",
            u8"value = 10\n",
            u8"[main.sub_text.\"two\"]\n",
            u8"value = 20\n",
            u8"[main.sub_text.\"three\"]\n",
            u8"value = 30\n",
            u8"[sub.sub.sub.\"one\"]\n",
            u8"value = 101\n",
            u8"[sub.sub.sub.\"two\"]\n",
            u8"value = 102\n",
            u8"[sub.sub.sub.\"three\"]\n",
            u8"value = 103\n",
            u8"[text.\"one\"]\n",
            u8"value = 201\n",
            u8"[text.\"two\"]\n",
            u8"value = 202\n",
            u8"[text.\"three\"]\n",
            u8"value = 203\n",
        };
        parser = std::make_shared<Parser>();
        REQUIRE_NOTHROW(doc = parser->parseOrThrow(source));
        REQUIRE_FALSE(doc->empty());
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.value1", u8"Integer(1)"},
            {u8"main.value2", u8"Integer(2)"},
            {u8"main.value3", u8"Integer(3)"},
            {u8"main.sub_text", u8"SectionWithTexts()"},
            {u8"main.sub_text.\"one\"", u8"SectionWithNames()"},
            {u8"main.sub_text.\"one\".value", u8"Integer(10)"},
            {u8"main.sub_text.\"two\"", u8"SectionWithNames()"},
            {u8"main.sub_text.\"two\".value", u8"Integer(20)"},
            {u8"main.sub_text.\"three\"", u8"SectionWithNames()"},
            {u8"main.sub_text.\"three\".value", u8"Integer(30)"},
            {u8"sub", u8"IntermediateSection()"},
            {u8"sub.sub", u8"IntermediateSection()"},
            {u8"sub.sub.sub", u8"SectionWithTexts()"},
            {u8"sub.sub.sub.\"one\"", u8"SectionWithNames()"},
            {u8"sub.sub.sub.\"one\".value", u8"Integer(101)"},
            {u8"sub.sub.sub.\"two\"", u8"SectionWithNames()"},
            {u8"sub.sub.sub.\"two\".value", u8"Integer(102)"},
            {u8"sub.sub.sub.\"three\"", u8"SectionWithNames()"},
            {u8"sub.sub.sub.\"three\".value", u8"Integer(103)"},
            {u8"text", u8"SectionWithTexts()"},
            {u8"text.\"one\"", u8"SectionWithNames()"},
            {u8"text.\"one\".value", u8"Integer(201)"},
            {u8"text.\"two\"", u8"SectionWithNames()"},
            {u8"text.\"two\".value", u8"Integer(202)"},
            {u8"text.\"three\"", u8"SectionWithNames()"},
            {u8"text.\"three\".value", u8"Integer(203)"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }
};

