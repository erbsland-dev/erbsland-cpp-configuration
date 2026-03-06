// Copyright (c) 2024-2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/Error.hpp>
#include <erbsland/conf/String.hpp>
#include <erbsland/conf/impl/utf8/U8StringView.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>

#include "erbsland/conf/Bytes.hpp"


using namespace el::conf;
using impl::U8StringView;
using impl::ElideLocation;


TESTED_TARGETS(String U8StringView)
class U8StringViewTest final : public el::UnitTest {
public:
    void testEscapedSize() {
        const auto testData = std::vector<std::tuple<EscapeMode, String, std::size_t>>{
            {EscapeMode::Text, u8"\n", 2}, // "\n"
            {EscapeMode::FullTextName, u8"\n", 5}, // "\u{a}"
            {EscapeMode::FullTestAdapter, u8"\n", 5}, // "\u{a}"
            {EscapeMode::ErrorText, u8"\n", 2}, // "\n"
            {EscapeMode::Text, u8"\\", 2}, // "\\"
            {EscapeMode::FullTextName, u8"\\", 6}, // "\u{5c}"
            {EscapeMode::FullTestAdapter, u8"\\", 6}, // "\u{5c}"
            {EscapeMode::ErrorText, u8"\\", 2}, // "\\"
            {EscapeMode::Text, u8"😀", 4}, // 4 UTF-8 bytes
            {EscapeMode::FullTextName, u8"😀", 9}, // "\u{1f600}"
            {EscapeMode::FullTestAdapter, u8"😀", 9}, // "\u{1f600}"
            {EscapeMode::ErrorText, u8"😀", 4}, // 4 UTF-8 bytes
            {EscapeMode::Text, u8"text😀text", 4+8}, // 4 UTF-8 bytes
            {EscapeMode::FullTextName, u8"text😀text", 9+8}, // "\u{1f600}"
            {EscapeMode::FullTestAdapter, u8"text😀text", 9+8}, // "\u{1f600}"
            {EscapeMode::ErrorText, u8"text😀text", 4+8}, // 4 UTF-8 bytes
        };
        for (const auto &[mode, str, expectedSize] : testData) {
            auto size = str.escapedSize(mode);
            REQUIRE_EQUAL(size, expectedSize);
        }
    }

    void testToEscape() {
        const auto testData = std::vector<std::tuple<EscapeMode, String, String>>{
            {EscapeMode::Text, u8"\n", u8"\\n"},
            {EscapeMode::FullTextName, u8"\n", u8"\\u{a}"},
            {EscapeMode::FullTestAdapter, u8"\n", u8"\\u{a}"},
            {EscapeMode::ErrorText, u8"\n", u8"\\n"},
            {EscapeMode::Text, u8"\\", u8"\\\\"},
            {EscapeMode::FullTextName, u8"\\", u8"\\u{5c}"},
            {EscapeMode::FullTestAdapter, u8"\\", u8"\\u{5c}"},
            {EscapeMode::ErrorText, u8"\\", u8"\\\\"},
            {EscapeMode::Text, u8"😀", u8"😀"},
            {EscapeMode::FullTextName, u8"😀", u8"\\u{1f600}"},
            {EscapeMode::FullTestAdapter, u8"😀", u8"\\u{1f600}"},
            {EscapeMode::ErrorText, u8"😀", u8"😀"},
            {EscapeMode::Text, u8"text😀text", u8"text😀text"},
            {EscapeMode::FullTextName, u8"text😀text", u8"text\\u{1f600}text"},
            {EscapeMode::FullTestAdapter, u8"text😀text", u8"text\\u{1f600}text"},
            {EscapeMode::ErrorText, u8"text😀text", u8"text😀text"},
        };
        for (const auto &[mode, str, expectedString] : testData) {
            auto actualString = str.toEscaped(mode);
            REQUIRE_EQUAL(actualString, expectedString);
        }
    }

    void testIsValidUtf8() {
        const auto validTestData = std::vector<String>{
            {},
            u8"a",
            u8"😀",
            u8"text😀text",
            u8"     text→⟨•×☮️       \n\r\r\r",
        };
        for (const auto &str : validTestData) {
            REQUIRE(str.isValidUtf8());
        }
        const auto invalidTestData = std::vector<Bytes>{
            Bytes::fromHex(u8"41 42 43 80 41 42"), // continuation with no start
            Bytes::fromHex(u8"41 42 43 87 41 42"), // continuation with no start
            Bytes::fromHex(u8"41 42 43 91 41 42"), // continuation with no start
            Bytes::fromHex(u8"41 42 43 af 41 42"), // continuation with no start
            Bytes::fromHex(u8"41 42 43 bf 41 42"), // continuation with no start
            Bytes::fromHex(u8"20 ED A0 80 40"), // low surrogate
            Bytes::fromHex(u8"40 41 42 43 ED BF BF 40 41 42"), // high surrogate
            Bytes::fromHex(u8"41 F4 90 80 80 41"), // too high
            Bytes::fromHex(u8"41 F5 90 80 80 80 41"), // too high
            Bytes::fromHex(u8"41 F6 90 80 80 80 80 41"), // too high
            Bytes::fromHex(u8"41 F7 90 80 80 80 80 80 41"), // too high
            Bytes::fromHex(u8"41 F8 90 80 80 80 80 80 80 41"), // too high
            Bytes::fromHex(u8"41 F9 90 80 80 80 80 80 80 80 41"), // too high
            Bytes::fromHex(u8"41 FA 90 80 80 80 80 80 80 80 80 41"), // too high
            Bytes::fromHex(u8"41 FB 90 80 80 80 80 80 80 80 80 80 41"), // too high
            Bytes::fromHex(u8"41 FC 90 80 80 80 80 80 80 80 80 80 80 41"), // too high
            Bytes::fromHex(u8"41 FD 90 80 80 80 80 80 80 80 80 80 80 80 41"), // too high
            Bytes::fromHex(u8"41 42 43 C0 80 41 42"), // encoded with too many bytes
            Bytes::fromHex(u8"41 42 43 C1 80 41 42"), // encoded with too many bytes
            Bytes::fromHex(u8"41 42 43 E0 9F BF 41 42"), // encoded with too many bytes
            Bytes::fromHex(u8"41 42 43 F0 8F BF BF 41 42"), // encoded with too many bytes
            Bytes::fromHex(u8"41 42 C2 41 42"), // missing continuation byte
            Bytes::fromHex(u8"41 42 E0 80 41 42"), // missing continuation byte
            Bytes::fromHex(u8"41 42 F0 80 80 41 42"), // missing continuation byte
        };
        for (const auto &bytes : invalidTestData) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                REQUIRE_THROWS(U8StringView::fromBytes(bytes));
                // bypass initial UTF-8 check.
                String str{reinterpret_cast<const char8_t*>(bytes.data()), bytes.size()};
                REQUIRE_FALSE(str.isValidUtf8());
            }, [&]() -> std::string {
                return std::format("Invalid UTF-8 bytes: {}", bytes.toHex());
            });
        }
    }

    void testToUtf8() {
        const auto testData = std::vector<std::tuple<String, Bytes>>{
            {u8"", Bytes::fromHex(u8"")},
            {u8"a", Bytes::fromHex(u8"61")},
            {u8"😀", Bytes::fromHex(u8"f0 9f 98 80")},
            {u8"text😀text", Bytes::fromHex(u8"74 65 78 74 f0 9f 98 80 74 65 78 74")},
        };
        for (const auto &[str, expectedBytes] : testData) {
            auto actualBytes = U8StringView{str}.toBytes();
            REQUIRE_EQUAL(actualBytes, expectedBytes);
        }
    }

    void testFromUtf8() {
        const auto testData = std::vector<std::tuple<Bytes, String>>{
            {Bytes::fromHex(u8""), u8""},
            {Bytes::fromHex(u8"61"), u8"a"},
            {Bytes::fromHex(u8"f0 9f 98 80"), u8"😀"},
            {Bytes::fromHex(u8"74 65 78 74 f0 9f 98 80 74 65 78 74"), u8"text😀text"},
        };
        for (const auto &[bytes, expectedString] : testData) {
            auto actualString = U8StringView::fromBytes(bytes);
            REQUIRE_EQUAL(actualString, expectedString);
        }
        String str;
        REQUIRE_THROWS_AS(Error, str = U8StringView::fromBytes(Bytes::fromHex(u8"41 80 42")));
    }

    void testUtf8Length() {
        const auto testData = std::vector<std::tuple<String, std::size_t>>{
            {u8"", 0},
            {u8"a", 1},
            {u8"😀", 1},
            {u8"text😀text", 9},
            {u8"→⟨•×", 4},
        };
        for (const auto &[str, expectedSize] : testData) {
            auto actualSize = str.characterLength();
            REQUIRE_EQUAL(actualSize, expectedSize);
        }
        auto invalidBytes = Bytes::fromHex(u8"41 80 42");
        String str{reinterpret_cast<const char8_t*>(invalidBytes.data()), invalidBytes.size()};
        std::size_t length;
        REQUIRE_THROWS_AS(Error, length = str.characterLength());
    }

    void testUtf8CharStart() {
        const auto text = String{u8"abcdef→⟨•×😀abcdef"};
        const auto positions = std::vector<std::size_t>{
            0, 1, 2, 3, 4, 5,
            6, // → 3
            9, // ⟨ 3
            12, // • 3
            15, // × 2
            17, // 😀 4
            21, 22, 23, 24, 25, 26
        };
        for (std::size_t i = 0; i < positions.size(); ++i) {
            auto actualPosition = U8StringView{text}.startForChar(i);
            REQUIRE_EQUAL(actualPosition, positions[i]);
        }
        REQUIRE_THROWS(U8StringView{text}.startForChar(positions.size()));
        REQUIRE_THROWS(U8StringView{text}.startForChar(5000));
    }

    void testUtf8TruncatedWithElide() {
        const auto shortText = String{u8"⟨😄😆⟩"};
        auto actualText = U8StringView{shortText}.truncatedWithElide(10, ElideLocation::Begin);
        REQUIRE_EQUAL(actualText, shortText);
        actualText = U8StringView{shortText}.truncatedWithElide(10, ElideLocation::Center);
        REQUIRE_EQUAL(actualText, shortText);
        actualText = U8StringView{shortText}.truncatedWithElide(10, ElideLocation::End);
        REQUIRE_EQUAL(actualText, shortText);
        actualText = U8StringView{shortText}.truncatedWithElide(5, ElideLocation::Begin, u8"😀");
        REQUIRE_EQUAL(actualText, shortText);

        const auto longText = String{
            u8"昨日、東京の小さなカフェで「pancake🍓」と☕️を頼んだら、すごく美味しかった！おすすめ😊"};
        struct TestData {
            std::size_t maximumCharacters;
            ElideLocation location;
            String elideSequence;
            String expectedText;
        };
        const auto textData = std::vector<TestData>{
            {
                .maximumCharacters = 20,
                .location = ElideLocation::Begin,
                .elideSequence = u8"…",
                .expectedText = u8"…んだら、すごく美味しかった！おすすめ😊",
            },
            {
                .maximumCharacters = 20,
                .location = ElideLocation::End,
                .elideSequence = u8"…",
                .expectedText = u8"昨日、東京の小さなカフェで「panca…",
            },
            {
                .maximumCharacters = 20,
                .location = ElideLocation::Center,
                .elideSequence = u8"…",
                .expectedText = u8"昨日、東京の小さな…しかった！おすすめ😊",
            },
            {
                .maximumCharacters = 20,
                .location = ElideLocation::Begin,
                .elideSequence = u8"(...)",
                .expectedText = u8"(...)すごく美味しかった！おすすめ😊",
            },
            {
                .maximumCharacters = 20,
                .location = ElideLocation::End,
                .elideSequence = u8"(...)",
                .expectedText = u8"昨日、東京の小さなカフェで「p(...)",
            },
            {
                .maximumCharacters = 20,
                .location = ElideLocation::Center,
                .elideSequence = u8"(...)",
                .expectedText = u8"昨日、東京の小(...)った！おすすめ😊",
            },
        };
        for (const auto &data : textData) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                actualText = U8StringView{longText}.truncatedWithElide(
                    data.maximumCharacters,
                    data.location,
                    data.elideSequence);
                REQUIRE_EQUAL(actualText.characterLength(), data.maximumCharacters);
                REQUIRE_EQUAL(actualText, data.expectedText);
            }, [&]() -> std::string {
                std::string result;
                result += std::format("Failed for maximumCharacters: {}\nlocation: {}\nelideSequence: \"{}\"\n",
                    data.maximumCharacters,
                    static_cast<int>(data.location),
                    data.elideSequence.toCharString());
                result += std::format("Expected result: \"{}\"\n", data.expectedText.toCharString());
                result += std::format("Actual result: \"{}\"\n", actualText.toCharString());
                return result;
            });
        }
    }

    void testUtf8Compare() {
        struct TestData {
            String left;
            String right;
            std::strong_ordering expectedResult;
        };
        const auto testData = std::vector<TestData>{
            {
                {},
                {},
                std::strong_ordering::equal,
            },
            {
                {},
                u8"a",
                std::strong_ordering::less,
            },
            {
                u8"a",
                {},
                std::strong_ordering::greater,
            },
            {
                u8"昨日、東京の小さなカフェで「pancake🍓」と☕️を頼んだら、すごく美味しかった！おすすめ😊",
                u8"昨日、東京の小さなカフェで「pancake🍓」と☕️を頼んだら、すごく美味しかった！おすすめ😊",
                std::strong_ordering::equal,
            },
            {
                u8"昨日、東京の小さなカフェで「pancake🍓」と☕️を頼んだら、すごく美味しかった！おすすめ😊",
                u8"昨日、東京の小さなカフェで「pancake🍓」と☕️を頼んだら、すごく美味しかった！おすすめ",
                std::strong_ordering::greater,
            },
            {
                u8"昨日、東京の小さなカフェで「pancake🍎」と☕️を頼んだら、すごく美味しかった！おすすめ😊",
                u8"昨日、東京の小さなカフェで「pancake🍓」と☕️を頼んだら、すごく美味しかった！おすすめ😊",
                std::strong_ordering::less,
            },
            {
                u8"config/a/test.elcl",
                u8"config/test.elcl",
                std::strong_ordering::less,
            },
        };
        for (const auto &data : testData) {
            auto actualResult = U8StringView{data.left}.compare(data.right);
            REQUIRE_EQUAL(actualResult, data.expectedResult);
        }
    }

    void testUtf8StartsWith() {
        // Default comparator cases
        struct TestData {
            String haystack;
            String needle;
            bool expected;
        };
        const auto defaultData = std::vector<TestData>{
            {u8"", u8"", true},
            {u8"", u8"a", false},
            {u8"a", u8"", true},
            {u8"abc", u8"abc", true},
            {u8"abc", u8"ab", true},
            {u8"abc", u8"abcx", false},
            {u8"abc", u8"ac", false},
            {u8"→⟨•×😀abcdef", u8"→", true},
            {u8"→⟨•×😀abcdef", u8"→⟨•", true},
            {u8"→⟨•×😀abcdef", u8"→⟨•×😀", true},
            {u8"→⟨•×😀abcdef", u8"↔", false},
        };
        for (const auto &d : defaultData) {
            REQUIRE_EQUAL(U8StringView{d.haystack}.startsWith(d.needle), d.expected);
        }

        // Case-insensitive comparator
        const auto ciData = std::vector<TestData>{
            {u8"", u8"", true},
            {u8"Config", u8"con", true},
            {u8"config", u8"CON", true},
            {u8"CONFIG", u8"Conf", true},
            {u8"ABC", u8"abd", false},
        };
        for (const auto &d : ciData) {
            REQUIRE_EQUAL(U8StringView{d.haystack}.startsWith(d.needle, impl::Char::compareCaseInsensitive), d.expected);
        }

    }

    void testUtf8EndsWith() {
        // Default comparator cases
        struct TestData {
            String haystack;
            String needle;
            bool expected;
        };
        const auto defaultData = std::vector<TestData>{
            {u8"", u8"", true},
            {u8"", u8"a", false},
            {u8"a", u8"", true},
            {u8"abc", u8"abc", true},
            {u8"abc", u8"bc", true},
            {u8"abc", u8"xbc", false},
            {u8"abc", u8"abcd", false},
            {u8"abcdef→⟨•×😀", u8"😀", true},
            {u8"abcdef→⟨•×😀", u8"•×😀", true},
            {u8"abcdef→⟨•×😀", u8"×😀x", false},
        };
        for (const auto &d : defaultData) {
            REQUIRE_EQUAL(U8StringView{d.haystack}.endsWith(d.needle), d.expected);
        }

        // Case-insensitive comparator
        const auto ciData = std::vector<TestData>{
            {u8"", u8"", true},
            {u8"app.LOG", u8".log", true},
            {u8"app.log", u8".LOG", true},
            {u8"Readme.MD", u8".md", true},
            {u8"note.txt", u8".md", false},
        };
        for (const auto &d : ciData) {
            REQUIRE_EQUAL(U8StringView{d.haystack}.endsWith(d.needle, impl::Char::compareCaseInsensitive), d.expected);
        }

    }

    void testUtf8Contains() {
        // Default comparator cases
        struct TestData {
            String haystack;
            String needle;
            bool expected;
        };
        const auto defaultData = std::vector<TestData>{
            {u8"", u8"", true},
            {u8"", u8"a", false},
            {u8"a", u8"", true},
            {u8"abc", u8"abc", true},
            {u8"abc", u8"ab", true},
            {u8"abc", u8"bc", true},
            {u8"abc", u8"ac", false},
            {u8"xxxabababcxxx", u8"ababc", true},
            {u8"→⟨•×😀abcdef", u8"→", true},
            {u8"→⟨•×😀abcdef", u8"⟨•×", true},
            {u8"→⟨•×😀abcdef", u8"•×😀a", true},
            {u8"→⟨•×😀abcdef", u8"↔", false},
        };
        for (const auto &d : defaultData) {
            REQUIRE_EQUAL(U8StringView{d.haystack}.contains(d.needle), d.expected);
        }

        // Case-insensitive comparator
        const auto ciData = std::vector<TestData>{
            {u8"", u8"", true},
            {u8"Config", u8"onF", true},
            {u8"CONFIG", u8"conf", true},
            {u8"abc", u8"ABD", false},
        };
        for (const auto &d : ciData) {
            REQUIRE_EQUAL(U8StringView{d.haystack}.contains(d.needle, impl::Char::compareCaseInsensitive), d.expected);
        }
    }

    void testFirstByteIndex() {
        const String text = u8"a😀b😀";
        const auto view = U8StringView{text};
        REQUIRE_EQUAL(view.firstByteIndex(impl::Char{U'a'}), 0U);
        REQUIRE_EQUAL(view.firstByteIndex(impl::Char{U'😀'}), 1U);
        REQUIRE_EQUAL(view.firstByteIndex(impl::Char{U'😀'}, 5), 6U);
        REQUIRE_EQUAL(view.firstByteIndex(impl::Char{U'x'}), std::u8string_view::npos);
        REQUIRE_THROWS_AS(std::range_error, view.firstByteIndex(impl::Char{U'😀'}, 2));
        REQUIRE_THROWS_AS(std::range_error, view.firstByteIndex(impl::Char{U'😀'}, 100));
    }

    void testSplit() {
        const auto text = String{u8",a,,b,"};
        const auto list = U8StringView{text}.split(impl::Char{U','}, {});
        REQUIRE_EQUAL(list.size(), 5U);
        REQUIRE_EQUAL(list[0], u8"");
        REQUIRE_EQUAL(list[1], u8"a");
        REQUIRE_EQUAL(list[2], u8"");
        REQUIRE_EQUAL(list[3], u8"b");
        REQUIRE_EQUAL(list[4], u8"");

        const auto limitedText = String{u8"a,b,c,d"};
        const auto limited = U8StringView{limitedText}.split(impl::Char{U','}, 2);
        REQUIRE_EQUAL(limited.size(), 3U);
        REQUIRE_EQUAL(limited[0], u8"a");
        REQUIRE_EQUAL(limited[1], u8"b");
        REQUIRE_EQUAL(limited[2], u8"c,d");
    }

    void testJoin() {
        const auto glueText = String{u8"—"};
        const auto glue = U8StringView{glueText};
        const StringList parts{u8"a", u8"", u8"b"};
        REQUIRE_EQUAL(glue.join(parts), u8"a——b");

        const auto emptyGlueText = String{u8""};
        const auto emptyGlue = U8StringView{emptyGlueText};
        const StringList single{u8"only"};
        REQUIRE_EQUAL(emptyGlue.join(single), u8"only");
        REQUIRE_EQUAL(emptyGlue.join({}), String{});
    }

    void testTrimmed() {
        const auto emptyText = String{u8""};
        REQUIRE_EQUAL(U8StringView{emptyText}.trimmed(), String{});
        const auto trimmedText = String{u8"\t  trimmed text \t"};
        REQUIRE_EQUAL(U8StringView{trimmedText}.trimmed(), u8"trimmed text");
        const auto spacingText = String{u8"\t  \t"};
        REQUIRE_EQUAL(U8StringView{spacingText}.trimmed(), String{});
        const auto emojiText = String{u8"😀 kept \t"};
        REQUIRE_EQUAL(U8StringView{emojiText}.trimmed(), u8"😀 kept");
    }

    void testTransformed() {
        const auto text = String{u8"aB😀"};
        const auto transformed = U8StringView{text}.transformed([](const impl::Char character) {
            if (character == impl::Char{U'a'}) {
                return impl::Char{U'A'};
            }
            if (character == impl::Char{U'B'}) {
                return impl::Char{U'b'};
            }
            if (character == impl::Char{U'😀'}) {
                return impl::Char{U'😎'};
            }
            return character;
        });
        REQUIRE_EQUAL(transformed, u8"Ab😎");
    }

    void testTransformed32() {
        const auto text = String{u8"Ab😀"};
        const auto transformed = U8StringView{text}.transformed32([](const char32_t character) {
            if (character == U'A') {
                return U'a';
            }
            if (character == U'b') {
                return U'B';
            }
            if (character == U'😀') {
                return U'😁';
            }
            return character;
        });
        REQUIRE_EQUAL(transformed, u8"aB😁");
    }

    void testForEachChar() {
        std::u32string collected;
        std::size_t count = 0;
        const auto text = String{u8"A😀z"};
        U8StringView{text}.forEachChar([&](const impl::Char character) {
            collected.push_back(character.raw());
            ++count;
        });
        REQUIRE_EQUAL(collected, std::u32string{U"A😀z"});
        REQUIRE_EQUAL(count, 3U);
    }

    void testForEachChar32() {
        String rebuilt;
        std::size_t count = 0;
        const auto text = String{u8"A😀z"};
        U8StringView{text}.forEachChar32([&](const char32_t character) {
            rebuilt.append(character);
            ++count;
        });
        REQUIRE_EQUAL(rebuilt, u8"A😀z");
        REQUIRE_EQUAL(count, 3U);
    }

    void testToSafeText() {
        const auto text = String{u8"line1\n😀line2"};
        const auto view = U8StringView{text}; // Keep the backing string alive for the duration of the view.
        REQUIRE_EQUAL(view.toSafeText(100), u8"line1\\n😀line2");

        const auto longText = String{
            u8"昨日、東京の小さなカフェで「pancake🍓」と☕️を頼んだら、すごく美味しかった！おすすめ😊"};
        const auto truncated = U8StringView{longText}.toSafeText(20, ElideLocation::End);
        REQUIRE_EQUAL(truncated.characterLength(), 20U);
        REQUIRE(truncated.endsWith(u8"…"));

        const auto invalidBytes = Bytes::fromHex(u8"41 80 42");
        const auto invalid = String{reinterpret_cast<const char8_t *>(invalidBytes.data()), invalidBytes.size()};
        REQUIRE_EQUAL(U8StringView{invalid}.toSafeText(), u8"<contains UTF-8 encoding errors>");
    }

    void testToNameNormalized() {
        const auto emptyText = String{u8""};
        REQUIRE_EQUAL(U8StringView{emptyText}.toNameNormalized(), String{});
        const auto namedText = String{u8"  This Name\tValue "};
        REQUIRE_EQUAL(U8StringView{namedText}.toNameNormalized(), u8"this_name_value");
        const auto spacedText = String{u8"\t  A  B\tC"};
        REQUIRE_EQUAL(U8StringView{spacedText}.toNameNormalized(), u8"a__b_c");
        const auto validText = String{u8" Already_Valid"};
        REQUIRE_EQUAL(U8StringView{validText}.toNameNormalized(), u8"already_valid");
    }
};
