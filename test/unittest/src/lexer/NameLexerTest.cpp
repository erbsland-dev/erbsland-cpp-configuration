// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/lexer/NameLexer.hpp>
#include <erbsland/conf/ErrorCategory.hpp>
#include <erbsland/conf/impl/utf8/U8Format.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <format>


using namespace erbsland::conf;
using impl::NameLexer;

TESTED_TARGETS(NameLexer)
class NameLexerTest final : public el::UnitTest {
public:
    Name name;

    struct TestData {
        String text; ///< The name path to test.
        std::vector<Name> expectedNames; ///< The list of expected names.
    };
    using TestDataList = std::vector<TestData>;

    struct ErrorData {
        String text; ///< A text with an error in it.
        ErrorCategory expectedErrorCategory = ErrorCategory::Syntax; ///< The expected error class.
    };
    using ErrorDataList = std::vector<ErrorData>;

    void verifyTestData(const TestDataList &testDataList) {
        for (const auto &testData : testDataList) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                NameLexer lexer{testData.text};
                REQUIRE_NOTHROW(lexer.initialize());
                for (auto i = 0; i < testData.expectedNames.size(); ++i) {
                    runWithContext(SOURCE_LOCATION(), [&]() {
                        const auto expectedName = testData.expectedNames[i];
                        REQUIRE(lexer.hasNext());
                        REQUIRE_NOTHROW(name = lexer.next());
                        REQUIRE_EQUAL(name, expectedName);
                    }, [&]() -> std::string {
                        return std::format("Failed at index: {}", i);
                    });
                }
                REQUIRE_FALSE(lexer.hasNext());
            }, [&]() -> std::string {
                return std::format("Failed for text: `{}`", testData.text.toCharString());
            });
        }
    }

    void verifyErrorData(const ErrorDataList &errorDataList) {
        for (const auto &errorData : errorDataList) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                NameLexer lexer{errorData.text};
                REQUIRE_NOTHROW(lexer.initialize());
                try {
                    NameList names;
                    while (lexer.hasNext()) {
                        name = lexer.next();
                        names.push_back(name); // do something useful looking.
                    }
                    REQUIRE(false); // expected error, but none was thrown.
                } catch (Error const &error) {
                    REQUIRE_EQUAL(error.category(), errorData.expectedErrorCategory);
                }
            }, [&]() -> std::string {
                return std::format("Failed for text: `{}`", errorData.text.toCharString());
            });
        }
    }

    void testEmpty() {
        NameLexer lexer{u8""};
        REQUIRE_NOTHROW(lexer.initialize());
        REQUIRE_FALSE(lexer.hasNext());
        REQUIRE_NOTHROW(name = lexer.next());
    }

    void testTooLong() {
        NameLexer lexer{String(limits::maxLineLength + 100, u8'a')};
        REQUIRE_THROWS_AS(Error, lexer.initialize());
    }

    void testRegularNames() {
        const auto testData = TestDataList{
            {u8"a", {Name::createRegular("a")}},
            {u8"A_longer32_regular09_NAME", {Name::createRegular("a_longer32_regular09_name")}},
            {u8"     name",{Name::createRegular("name")}},
            {u8"name   \t     ",{Name::createRegular("name")}},
            {u8"   \t      name   \t     ",{Name::createRegular("name")}},
            {u8"A regular Name 345 with Spaces",{Name::createRegular("a_regular_name_345_with_spaces")}},
            {u8"      Name with Spaces        ",{Name::createRegular("name_with_spaces")}},
        };
        WITH_CONTEXT(verifyTestData(testData));
    }

    void testIndexes() {
        const auto testData = TestDataList{
            {u8"[0]", {Name::createIndex(0)}},
            {u8"[1]", {Name::createIndex(1)}},
            {u8"[27302]",{Name::createIndex(27302)}},
            {u8"[27'302]",{Name::createIndex(27302)}},
            {u8"[   129]",{Name::createIndex(129)}},
            {u8"[762   ]",{Name::createIndex(762)}},
            {u8"    [1]",{Name::createIndex(1)}},
            {u8"[1]    ",{Name::createIndex(1)}},
        };
        WITH_CONTEXT(verifyTestData(testData));
    }

    void testTextNames() {
        const auto testData = TestDataList{
            {u8R"("a")", {Name::createText(u8"a")}},
            {u8R"(    "a")", {Name::createText(u8"a")}},
            {u8R"("a"    )", {Name::createText(u8"a")}},
            {u8R"("   abc   ")", {Name::createText(u8"   abc   ")}},
            {u8R"("\r\t\n\u1234\u{1f20}")", {Name::createText(u8"\r\t\n\u1234\u1f20")}},
        };
        WITH_CONTEXT(verifyTestData(testData));
    }

    void testTextIndexes() {
        const auto testData = TestDataList{
            {u8R"(""[0])", {Name::createTextIndex(0)}},
            {u8R"(""[1])", {Name::createTextIndex(1)}},
            {u8R"(""[93821])", {Name::createTextIndex(93821)}},
            {u8R"(""[  93'821  ])", {Name::createTextIndex(93821)}},
        };
        WITH_CONTEXT(verifyTestData(testData));
    }

    void testSingleNameErrors() {
        const auto testData = ErrorDataList{
            {u8"?", ErrorCategory::Character},
            {u8"_name", ErrorCategory::Syntax},
            {u8"name_", ErrorCategory::Syntax},
            {u8"name__name", ErrorCategory::Syntax},
            {u8"name  name", ErrorCategory::Syntax},
            {u8"..", ErrorCategory::Syntax},
            {u8"[]", ErrorCategory::Syntax},
            {u8"[-1]", ErrorCategory::Syntax},
            {u8"[a]", ErrorCategory::Syntax},
            {u8"\"\"", ErrorCategory::Syntax},
            {u8"\"\\u{0}\"", ErrorCategory::Syntax},
            {u8"\"\"[]", ErrorCategory::Syntax},
            {u8"\"\"[-1]", ErrorCategory::Syntax},
        };
    }

    void testPaths() {
        const auto testData = TestDataList{
            {u8"a.b.c.d", {
                Name::createRegular(u8"a"),
                Name::createRegular(u8"b"),
                Name::createRegular(u8"c"),
                Name::createRegular(u8"d"),
            }},
            {u8"  a regular name  . Second One. Another  .Last Name  ", {
                Name::createRegular(u8"a_regular_name"),
                Name::createRegular(u8"second_one"),
                Name::createRegular(u8"another"),
                Name::createRegular(u8"last_name"),
            }},
            {u8"\"text\".name[123].\"text\"", {
                Name::createText(u8"text"),
                Name::createRegular(u8"name"),
                Name::createIndex(123),
                Name::createText(u8"text"),
            }},
            {u8"[789].\"\"[123].name", {
                Name::createIndex(789),
                Name::createTextIndex(123),
                Name::createRegular(u8"name")
            }},
        };
        WITH_CONTEXT(verifyTestData(testData));
    }

    void testPathErrors() {
        const auto testData = ErrorDataList{
            // Paths must not end with a separator.
            {u8"a.b.c.", ErrorCategory::UnexpectedEnd},
            {u8"a.b.c.  ", ErrorCategory::UnexpectedEnd},
            {u8"a.b.c   .", ErrorCategory::UnexpectedEnd},
            // Paths must not start with a separator.
            {u8".a.b.c", ErrorCategory::Syntax},
            {u8"    .a.b.c", ErrorCategory::Syntax},
            {u8".   a.b.c", ErrorCategory::Syntax},
            // Subsequent separators aren't allowed
            {u8"a.b..c.d", ErrorCategory::Syntax},
            {u8"a.b.    .c.d", ErrorCategory::Syntax},
            // Names must not start with underlines.
            {u8"a._b.c.d", ErrorCategory::Syntax},
            {u8"a.   _b.c.d", ErrorCategory::Syntax},
            // Indexes must not immediately follow a seperator.
            {u8"a.[1].c.d", ErrorCategory::Syntax},
            {u8"a.   [1].c.d", ErrorCategory::Syntax},
            // Empty index is not allowed.
            {u8"a.[].c.d", ErrorCategory::Syntax},
            // Names must be separated properly.
            {u8"a.\"text\"\"text\".c.d", ErrorCategory::Syntax},
            {u8"a.\"text\"name.c.d", ErrorCategory::Syntax},
            {u8"a.[1]name.c.d", ErrorCategory::Syntax},
            {u8"a.[1][2].c.d", ErrorCategory::Syntax},
            {u8"a.[1] [2].c.d", ErrorCategory::Syntax},
            {u8"a.\"\".c.d", ErrorCategory::Syntax},
            {u8"a.\"\"[1][2].c.d", ErrorCategory::Syntax},
            {u8"a.\"\"[1]\"\"[2].c.d", ErrorCategory::Syntax},
            {u8"a.\"\"[1]name.c.d", ErrorCategory::Syntax},
            // Only tab and space is considered as spacing.
            {u8"a.b\n.c.d", ErrorCategory::Syntax},
            {u8"a.b\r.c.d", ErrorCategory::Syntax},
        };
        WITH_CONTEXT(verifyErrorData(testData));
    }

};


