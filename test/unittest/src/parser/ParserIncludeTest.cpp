// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ParserTestHelper.hpp"


TESTED_TARGETS(Parser)
class ParserIncludeTest final : public UNITTEST_SUBCLASS(ParserTestHelper) {
public:
    void tearDown() override {
        cleanUpTestFileDirectory();
        doc = {};
    }

    void expectParserError(
        const std::filesystem::path &path,
        const ErrorCategory errorCategory,
        String expectedWordInErrorMessage = {}) {
        try {
            Parser parser;
            doc = parser.parseOrThrow(Source::fromFile(path));
            REQUIRE(false);
        } catch (const Error &error) {
            REQUIRE_EQUAL(error.category(), errorCategory);
            if (!expectedWordInErrorMessage.empty()) {
                auto success = error.message().find(expectedWordInErrorMessage.raw()) != std::string::npos;
                if (!success) {
                    consoleWriteLine(error.toText().toCharString());
                }
                REQUIRE(success);
            }
        }
    }

    void testBasicInclude() {
        const auto mainFile = createTestFile(
            "config/main.elcl",
            u8"[main]\n"
            u8"value 01 = 5001\n"
            u8"value 02 = 5002\n\n"
            u8"@include: \"sub_01/config_02.elcl\"\n"
            u8"[second]\n"
            u8"value 03 = 6001\n"
            u8"@include: \"sub_02/*.elcl\"\n");
        createTestFile(
            "config/sub_01/config_02.elcl",
            u8"[sub 01]\n"
            u8"value 04 = 7001\n"
            u8"value 05 = 7002\n");
        createTestFile(
            "config/sub_02/config_03.elcl",
            u8"[sub 02]\n"
            u8"value 06 = 8001\n"
            u8"value 07 = 8002\n");
        createTestFile(
            "config/sub_02/config_04.elcl",
            u8"[sub 03]\n"
            "value 08 = 9001\n"
            "value 09 = 9002\n");
        Parser parser;
        REQUIRE_NOTHROW(doc = parser.parseOrThrow(Source::fromFile(mainFile)));
        REQUIRE(doc != nullptr);
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.value_01", u8"Integer(5001)"},
            {u8"main.value_02", u8"Integer(5002)"},
            {u8"second", u8"SectionWithNames()"},
            {u8"second.value_03", u8"Integer(6001)"},
            {u8"sub_01", u8"SectionWithNames()"},
            {u8"sub_01.value_04", u8"Integer(7001)"},
            {u8"sub_01.value_05", u8"Integer(7002)"},
            {u8"sub_02", u8"SectionWithNames()"},
            {u8"sub_02.value_06", u8"Integer(8001)"},
            {u8"sub_02.value_07", u8"Integer(8002)"},
            {u8"sub_03", u8"SectionWithNames()"},
            {u8"sub_03.value_08", u8"Integer(9001)"},
            {u8"sub_03.value_09", u8"Integer(9002)"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testRecursiveIncludeAndCorrectOrder() {
        const auto mainFile = createTestFile(
            "config/main.elcl",
            u8"*[block]\n"
            u8"value 01 = 123\n"
            u8"@include: \"sub/**/*.elcl\"\n");
        createTestFile(
            "config/sub/a.elcl",
            u8"*[block]\n"
            u8"value 02 = 123\n");
        createTestFile(
            "config/sub/b.elcl",
            u8"*[block]\n"
            u8"value 03 = 123\n");
        createTestFile(
            "config/sub/a/a.elcl",
            u8"*[block]\n"
            u8"value 04 = 123\n");
        createTestFile(
            "config/sub/a/b.elcl",
            u8"*[block]\n"
            u8"value 05 = 123\n");
        createTestFile(
            "config/sub/b/a.elcl",
            u8"*[block]\n"
            u8"value 06 = 123\n");
        createTestFile(
            "config/sub/b/b.elcl",
            u8"*[block]\n"
            u8"value 07 = 123\n");
        Parser parser;
        REQUIRE_NOTHROW(doc = parser.parseOrThrow(Source::fromFile(mainFile)));
        REQUIRE(doc != nullptr);
        auto expectedValueMap = ExpectedValueMap{
            {u8"block", u8"SectionList()"},
            {u8"block[0]", u8"SectionWithNames()"},
            {u8"block[0].value_01", u8"Integer(123)"},
            {u8"block[1]", u8"SectionWithNames()"},
            {u8"block[1].value_02", u8"Integer(123)"},
            {u8"block[2]", u8"SectionWithNames()"},
            {u8"block[2].value_03", u8"Integer(123)"},
            {u8"block[3]", u8"SectionWithNames()"},
            {u8"block[3].value_04", u8"Integer(123)"},
            {u8"block[4]", u8"SectionWithNames()"},
            {u8"block[4].value_05", u8"Integer(123)"},
            {u8"block[5]", u8"SectionWithNames()"},
            {u8"block[5].value_06", u8"Integer(123)"},
            {u8"block[6]", u8"SectionWithNames()"},
            {u8"block[6].value_07", u8"Integer(123)"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testErrorIncludeNotFound() {
        const auto mainFile = createTestFile(
            "config/main.elcl",
            u8"@include: \"config02.elcl\"\n");
        WITH_CONTEXT(expectParserError(mainFile, ErrorCategory::Syntax, u8"not find"));
    }

    void testNoWildcardMatches1() {
        const auto mainFile = createTestFile(
            "config/main.elcl",
            u8"@include: \"none*.elcl\"\n");
        Parser parser;
        REQUIRE_NOTHROW(doc = parser.parseOrThrow(Source::fromFile(mainFile)));
        REQUIRE(doc != nullptr);
    }

    void testNoWildcardMatches2() {
        const auto mainFile = createTestFile(
            "config/main.elcl",
            u8"@include: \"**/none.elcl\"\n");
        Parser parser;
        REQUIRE_NOTHROW(doc = parser.parseOrThrow(Source::fromFile(mainFile)));
        REQUIRE(doc != nullptr);
    }

    void testErrorLoop() {
        const auto mainFile = createTestFile(
            "config/main.elcl",
            u8"@include: \"config02.elcl\"\n");
        createTestFile(
            "config/config02.elcl",
            u8"@include: \"config03.elcl\"\n");
        createTestFile(
            "config/config03.elcl",
            u8"@include: \"main.elcl\"\n");
        WITH_CONTEXT(expectParserError(mainFile, ErrorCategory::Syntax, u8"loop"));
    }

    void testErrorNestingLimit() {
        const auto mainFile = createTestFile(
            "config/main.elcl",
            u8"@include: \"config02.elcl\"\n");
        createTestFile(
            "config/config02.elcl",
            u8"@include: \"config03.elcl\"\n");
        createTestFile(
            "config/config03.elcl",
            u8"@include: \"config04.elcl\"\n");
        createTestFile(
            "config/config04.elcl",
            u8"@include: \"config05.elcl\"\n");
        createTestFile(
            "config/config05.elcl",
            u8"@include: \"config06.elcl\"\n");
        createTestFile(
            "config/config06.elcl",
            u8"@include: \"config07.elcl\"\n");
        createTestFile(
            "config/config07.elcl",
            u8"[main]\n");
        WITH_CONTEXT(expectParserError(mainFile, ErrorCategory::LimitExceeded, u8"nesting"));
    }
};

