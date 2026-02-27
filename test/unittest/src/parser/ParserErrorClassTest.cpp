// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/Parser.hpp>

#include <algorithm>
#include <map>


using namespace el::conf;


TESTED_TARGETS(Parser)
class ParserErrorClassTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    using TestCases = std::vector<std::pair<String, ErrorCategory>>;

    void verifyTestCases(const TestCases &testCases) {
        for (const auto &testCase : testCases) {
            ErrorCategory actualCategory;
            String actualMessage;
            ErrorCategory expectedCategory;
            runWithContext(SOURCE_LOCATION(), [&] {
                expectedCategory = testCase.second;
                auto source = createTestMemorySource(testCase.first);
                auto parser = Parser{};
                try {
                    auto doc = parser.parseOrThrow(source);
                    REQUIRE(false); // must not succeed.
                } catch (const Error &error) {
                    actualCategory = error.category();
                    actualMessage = error.message();
                    REQUIRE_EQUAL(actualCategory, expectedCategory);
                } catch (const erbsland::AssertFailed&) {
                    actualMessage = "Parsing succeeded, but should have failed.";
                    throw;
                } catch (const std::exception &error) {
                    actualMessage = String{std::format(
                        "Unexpected exception: type={}, what={}", typeid(error).name(), error.what())};
                    REQUIRE(false);
                }
            }, [&]() -> std::string {
                return std::format(
                    "Failed for text: \"{}\"\nExpected {}, got {}.\nError message: {}",
                    testCase.first.toEscaped(EscapeMode::FullTestAdapter).toCharString(),
                    expectedCategory, actualCategory, actualMessage.toCharString());
            });
        }
    }

    void testUnexpectedEndVsSyntaxError() {
        // Test situations where the parser should detect an unexpected end of the document and not just
        // a syntax error.
        const auto testCases = TestCases{
            {u8"#comment\r", ErrorCategory::UnexpectedEnd},
            {u8"[", ErrorCategory::UnexpectedEnd},
            {u8"[\n", ErrorCategory::Syntax},
            {u8"[\r\n", ErrorCategory::Syntax},
            {u8"[main", ErrorCategory::UnexpectedEnd},
            {u8"[main\n", ErrorCategory::Syntax},
            {u8"[main\r\n", ErrorCategory::Syntax},
            {u8"[main ", ErrorCategory::UnexpectedEnd},
            {u8"[main \n", ErrorCategory::Syntax},
            {u8"[main \r\n", ErrorCategory::Syntax},
            {u8"[main.", ErrorCategory::UnexpectedEnd},
            {u8"[main.\n", ErrorCategory::Syntax},
            {u8"[main.\r\n", ErrorCategory::Syntax},
            {u8"[main. ", ErrorCategory::UnexpectedEnd},
            {u8"[main. \n", ErrorCategory::Syntax},
            {u8"[main. \r\n", ErrorCategory::Syntax},
            {u8"[main.sub", ErrorCategory::UnexpectedEnd},
            {u8"[main.sub\n", ErrorCategory::Syntax},
            {u8"[main.sub\r\n", ErrorCategory::Syntax},
            {u8"[main.sub ", ErrorCategory::UnexpectedEnd},
            {u8"[main.sub \n", ErrorCategory::Syntax},
            {u8"[main.sub \r\n", ErrorCategory::Syntax},
            {u8"[main.sub.", ErrorCategory::UnexpectedEnd},
            {u8"[main.sub.\n", ErrorCategory::Syntax},
            {u8"[main.sub.\r\n", ErrorCategory::Syntax},
            {u8"[main.sub. ", ErrorCategory::UnexpectedEnd},
            {u8"[main.sub. \n", ErrorCategory::Syntax},
            {u8"[main.sub. \r\n", ErrorCategory::Syntax},
            {u8"[main.\"", ErrorCategory::UnexpectedEnd},
            {u8"[main.\"\n", ErrorCategory::Syntax},
            {u8"[main.\"\r\n", ErrorCategory::Syntax},
            {u8"[main.\"sub", ErrorCategory::UnexpectedEnd},
            {u8"[main.\"sub\n", ErrorCategory::Syntax},
            {u8"[main.\"sub\r\n", ErrorCategory::Syntax},
            {u8"[main.\"sub\"", ErrorCategory::UnexpectedEnd},
            {u8"[main.\"sub\"\n", ErrorCategory::Syntax},
            {u8"[main.\"sub\"\r\n", ErrorCategory::Syntax},
            {u8"[main.\"sub\" ", ErrorCategory::UnexpectedEnd},
            {u8"[main.\"sub\" \n", ErrorCategory::Syntax},
            {u8"[main.\"sub\" \r\n", ErrorCategory::Syntax},
            {u8"[main.\"sub\".", ErrorCategory::UnexpectedEnd},
            {u8"[main.\"sub\".\n", ErrorCategory::Syntax},
            {u8"[main.\"sub\".\r\n", ErrorCategory::Syntax},
            {u8"[main.\"sub\". ", ErrorCategory::UnexpectedEnd},
            {u8"[main.\"sub\". \n", ErrorCategory::Syntax},
            {u8"[main.\"sub\". \r\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue    ", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue # comment", ErrorCategory::Syntax},
            {u8"[main]\nvalue:", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: # comment", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue=", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue= # comment", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue   :", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue   : # comment", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue:\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: # comment\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue=\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue= # comment\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text ", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text value", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text value\"", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text value\"\n", ErrorCategory::Syntax},
            {u8"[main]\n\"text value\"    ", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text value\" # comment", ErrorCategory::Syntax},
            {u8"[main]\n\"text value\":", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text value\": # comment", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text value\"   :", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text value\"   : # comment", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text value\":\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\n\"text value\": # comment\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: \"", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: \"\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: \"text", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: \"text\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: `", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: `\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: `text", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: `text\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: /", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: /\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: /text", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: /text\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: <", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: <\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: <hex", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: <hex\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: <hex:", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: <hex:\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: <0102", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: <0102\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: \"\"\"", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: \"\"\"\n    text", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: \"\"\"\n    text\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: ```", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: ```\n    text", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: ```\n    text\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: ///", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: ///\n    text", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: ///\n    text\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: <<<", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: <<<\n    0102", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: <<<\n    0102\n", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 100'", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 0x", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 0x1000'", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 0x\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 0b", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 0b1111'", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 0b\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 1, 2,", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 1, 2, ", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 1, 2,\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 100e", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 0.1e+", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 100e\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 0.1e+\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-0", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-0", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-01t", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-01 12:", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-01 12:0", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-01 12:05:", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-01 12:05:3", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-01 12:05:34+", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-01 12:05:34+0", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-01 12:05:34+01:", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-08-01 12:05:34+01:3", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 12:", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 12:0", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 12:05:", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 12:05:3", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 12:05:34+", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 12:05:34+0", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 12:05:34+01:", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 12:05:34+01:3", ErrorCategory::UnexpectedEnd},
            {u8"[main]\nvalue: 2025-\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-0\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-0\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-01t\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-01 12:\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-01 12:0\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-01 12:05:\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-01 12:05:3\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-01 12:05:34+\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-01 12:05:34+0\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-01 12:05:34+01:\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 2025-08-01 12:05:34+01:3\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 12:\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 12:0\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 12:05:\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 12:05:3\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 12:05:34+\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 12:05:34+0\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 12:05:34+01:\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: 12:05:34+01:3\n", ErrorCategory::Syntax},
        };
        WITH_CONTEXT(verifyTestCases(testCases));
    }

    void testUnsupportedVsSyntaxError() {
        // Test situations where the parser should detect an unsupported error or a syntax error.
        const auto testCases = std::vector<std::pair<String, ErrorCategory>>{
            {u8"@version: \"0.9\"\n", ErrorCategory::Unsupported},
            {u8"@version: `0.9`\n", ErrorCategory::Syntax},
            {u8"@version: `1.0`\n", ErrorCategory::Syntax},
            {u8"@version: \"\"\"\n    1.0\n    \"\"\"\n", ErrorCategory::Syntax},
            {u8"@version: 1\n", ErrorCategory::Syntax},
            {u8"@version: 2\n", ErrorCategory::Syntax},
            {u8"@features: \"abcde\"\n", ErrorCategory::Unsupported},
            {u8"@features: \"core abcde\"\n", ErrorCategory::Unsupported},
            {u8"@features: `core`\n", ErrorCategory::Syntax},
            {u8"@features: \"\"\"\n    core\n    \"\"\"\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: <base64: 01234>\n", ErrorCategory::Unsupported},
            {u8"[main]\nvalue: <none$: 01234>\n", ErrorCategory::Syntax},
            {u8"[main]\nvalue: <<<base64\n    01234\n    >>>\n", ErrorCategory::Unsupported},
            {u8"[main]\nvalue: <<<none$\n    01234>\n    >>>\n", ErrorCategory::Syntax},
        };
        WITH_CONTEXT(verifyTestCases(testCases));
    }

    void testIndentationVsSyntaxError() {
        // Test situations where the parser should detect an indentation error and not a syntax error.
        const auto testCases = std::vector<std::pair<String, ErrorCategory>>{
            {u8"[main]\nv: \"\"\"\n  t\n t\n  \"\"\"\n", ErrorCategory::Indentation},
            {u8"[main]\nv: \"\"\"\n  t\n_ t\n  \"\"\"\n", ErrorCategory::Syntax},
            {u8"[main]\nv: \"\"\"\n\tt\n        t\n\t\"\"\"\n", ErrorCategory::Indentation},
            {u8"[main]\nv:\n  \"\"\"\n t  \"\"\"\n", ErrorCategory::Indentation},
            {u8"[main]\nv: ```\n  t\n t\n  ```\n", ErrorCategory::Indentation},
            {u8"[main]\nv: ```\n  t\n_ t\n  ```\n", ErrorCategory::Syntax},
            {u8"[main]\nv: ```\n\tt\n        t\n\t```\n", ErrorCategory::Indentation},
            {u8"[main]\nv:\n  ```\n t  ```\n", ErrorCategory::Indentation},
            {u8"[main]\nv: <<<\n  00\n 00\n  >>>\n", ErrorCategory::Indentation},
            {u8"[main]\nv: <<<\n  00\n_ 00\n  >>>\n", ErrorCategory::Syntax},
            {u8"[main]\nv: <<<\n\t00\n        00\n\t>>>\n", ErrorCategory::Indentation},
            {u8"[main]\nv:\n  <<<\n 00  >>>\n", ErrorCategory::Indentation},
            {u8"[main]\nv:\n  * 1\n * 2\n", ErrorCategory::Indentation},
        };
        WITH_CONTEXT(verifyTestCases(testCases));
    }

    void testCharacterVsSyntaxError() {
        // Test where the more specialized character error should be reported instead of a syntax error.
        // As this parser tests for illegal control characters just after UTF-8 decoding,
        // the question is just if the error gets correctly propagated through the lexer.
        // By inserting a control character into every position of the test document,
        // propagation errors should be sufficiently uncovered.
        const auto testDocument = String{u8"# Comment\n"
            "[main]\n"
            "v1: true\n"
            "v2: 123'456\n"
            "v3:\n\t0xab'01\n"
            "v4:\n 0b11'00#c\n"
            "v5: 12kb #c\n"
            "v6: 12 kb\t\n"
            "v7: 123'456 \n"
            "v8: \"t\"\n"
            "v9: 0.7e+2\t#c\n"
            "v10: 01:02:03.123+01:30\n"
            "v11: 2025-01-02\n"
            "v12: 2025-01-02 01:02:03.123+01:30\n"
            "v13: 2025-01-02t01:02:03.123+01:30\n"
            "v14: 12h\n"
            "v15: 5 years\n"
            "v16: `c`\n"
            "v17: <01>\n"
            "v18: \"\"\"\n t\n \"\"\"\n"
            "v19: \"\"\" #c\n t\n \"\"\" #c\n"
            "v20: ```\n c\n ```\n"
            "v21: <<<\n 01\n >>>\n"
            "#c"};
        // make sure the test document parses without errors:
        auto source = createTestMemorySource(testDocument);
        auto parser = Parser{};
        DocumentPtr doc;
        REQUIRE_NOTHROW(doc = parser.parseOrThrow(source))
        // shift a control character through the test file.
        for (std::size_t i = 0; i < testDocument.size() + 1; ++i) {
            auto newDocument = String{};
            newDocument.reserve(testDocument.size() + 1);
            newDocument.append(testDocument.substr(0, i));
            newDocument.append(u8"\b");
            newDocument.append(testDocument.substr(i));
            source = createTestMemorySource(newDocument);
            try {
                parser.parseOrThrow(source);
                REQUIRE(false);
            } catch (const Error &error) {
                REQUIRE_EQUAL(error.category(), ErrorCategory::Character);
            }
        }
    }

    void testLimitExceededVsSyntaxError() {
        // An overlong line that contains syntax errors should report LimitExceeded first
        const String overlongLine1 = String{4000, '0'} + u8"\n";
        // If the line length is correct, it's just a syntax error.
        const String exact4000Line2 = String{3999, '0'} + u8"\n";
        // An overlong name should report LimitExceeded before a Syntax error.
        const String overLongName1 = String{u8"["} + String{101, 'a'} + String{u8"_]\n"};
        const String overLongName2 = String{u8"["} + String{100, 'a'} + String{u8"_]\n"};
        // With 100 characters, the ending `_` is just a syntax error.
        const String exact100Name = String{u8"["} + String{99, 'a'} + String{u8"_]\n"};
        // Oversized name-path
        const String oversizedNamePath1 = String{u8"[a.b.c.d.e.f.g.h.i.j.k]\n"};
        // Oversized name-path with syntax error.
        const String oversizedNamePath2 = String{u8"[a.b.c.d.e.f.g.h.i.j.k._]\n"};
        // Just a syntax error.
        const String namePathWithSyntaxError = String{u8"[a.b.c.d.e.f.g.h.i.j._]\n"};
        const auto testCases = std::vector<std::pair<String, ErrorCategory>>{
            {overlongLine1, ErrorCategory::LimitExceeded},
            {exact4000Line2, ErrorCategory::Syntax},
            {overLongName1, ErrorCategory::LimitExceeded},
            {overLongName2, ErrorCategory::LimitExceeded},
            {exact100Name, ErrorCategory::Syntax},
            {oversizedNamePath1, ErrorCategory::LimitExceeded},
            {oversizedNamePath2, ErrorCategory::LimitExceeded},
            {namePathWithSyntaxError, ErrorCategory::Syntax},
        };
        WITH_CONTEXT(verifyTestCases(testCases));
    }
};

