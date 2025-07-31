// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/NamePath.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;


TESTED_TARGETS(NamePath)
class NamePathLexerTest final : public el::UnitTest {
public:
    NamePath namePath;

    auto additionalErrorMessages() -> std::string override {
        try {
            return std::format("namePath:\n{}", internalView(namePath)->toString(2));
        } catch (...) {
            return "Unexpected exception thrown";
        }
    }

    void testLexingText() {
        struct TestData {
            String text;
            NamePath expected;
        };
        const auto testData = std::vector<TestData>{
            {
                u8"",
                {}
            },
            {
                u8"one.two.three", // basic path with no normalization required.
                NamePath{{Name::createRegular(u8"one"), Name::createRegular(u8"two"), Name::createRegular(u8"three")}}
            },
            {
                u8"   Name1 . Name2 . Name 3  ", // basic path with spacing and required normalization.
                NamePath{{Name::createRegular(u8"name1"), Name::createRegular(u8"name2"), Name::createRegular(u8"name_3")}}
            },
            {
                u8"server[12].info.\" This is a text\"", // Mixed elements.
                NamePath{{Name::createRegular(u8"server"),
                    Name::createIndex(12), Name::createRegular(u8"info"),
                    Name::createText(u8" This is a text")}}
            },
            {
                u8"[12][34]", // Nested lists
                NamePath{{Name::createIndex(12), Name::createIndex(34)}}
            },
            {
                u8"server.value[12][34]", // Nested lists
                NamePath{{Name::createRegular(u8"server"),
                    Name::createRegular(u8"value"),
                    Name::createIndex(12), Name::createIndex(34)}}
            },
            {
                u8"server.text.\"\"[1234].filter", // Text index.
                NamePath{{Name::createRegular(u8"server"),
                    Name::createRegular(u8"text"),
                    Name::createTextIndex(1234),
                    Name::createRegular(u8"filter")}}
            },
            {
                u8"\"a text\".value",
                NamePath{{Name::createText(u8"a text"), Name::createRegular(u8"value")}}
            },
            {
                u8"@version", // basic path with no normalization required.
                NamePath{{Name::createRegular(u8"@version")}}
            },

        };
        for (auto const &data : testData) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                namePath = NamePath::fromText(data.text);
                REQUIRE_EQUAL(namePath, data.expected);
            }, [&]() -> std::string {
                return std::format("Failed for text: \"{}\"", data.text);
            });
        }
    }

    void testLexingInvalidText() {
        const auto testData = std::vector<String>{
            u8".",
            u8"name\n.name",
            u8"name\r.name",
            u8"name..name",
            u8"name.",
            u8"name.value.",
            u8" name   .. name",
            u8" name  .  ",
            u8"  name  .   value   .  ",
            u8"9name.value",
            u8"name.9value",
            u8"name[x]",
            u8"name.[10]",
            u8"name.value[x]",
            u8"name[0]name",
            u8"main.Name  Name",
            u8"main.\" text \"name",
            u8"main._value",
            u8"main.value_",
            u8"main.value__value",
            u8"main.value _value",
            u8"main.value_ value",
            String(5000, 'a'),
            String(u8"main.name") + String(100, 'a') + String( u8".value"),
            u8"main.\"\"",
            u8"main.\"\".value",
            u8"main.\"\"\"\n",
        };
        for (const auto &text : testData) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                REQUIRE_THROWS(NamePath::fromText(text));
            }, [&]() -> std::string {
                return std::format("Failed for text: \"{}\"", text);
            });
        }
    }

    void testPathToText() {
        struct TestData {
            NamePath path;
            String expected;
        };
        const auto testData = std::vector<TestData>{
            {
                NamePath{},
                u8""
            },
            {
                NamePath{{Name::createRegular(u8"one"), Name::createRegular(u8"two"), Name::createRegular(u8"three")}},
                u8"one.two.three"
            },
            {
                NamePath{{Name::createTextIndex(1234), Name::createRegular(u8"value")}},
                u8"\"\"[1234].value"
            },
            {
                NamePath{{Name::createRegular(u8"value"), Name::createTextIndex(1234), Name::createTextIndex(0)}},
                u8"value.\"\"[1234].\"\"[0]"
            },
        };
        for (auto const &data : testData) {
            namePath = data.path;
            REQUIRE_EQUAL(namePath.toText(), data.expected);
        }
    }
};

