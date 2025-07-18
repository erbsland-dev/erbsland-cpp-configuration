// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
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
            REQUIRE(U8StringView{str}.isValid());
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
                REQUIRE_FALSE(U8StringView{str}.isValid());
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
            auto actualSize = U8StringView{str}.length();
            REQUIRE_EQUAL(actualSize, expectedSize);
        }
        auto invalidBytes = Bytes::fromHex(u8"41 80 42");
        String str{reinterpret_cast<const char8_t*>(invalidBytes.data()), invalidBytes.size()};
        std::size_t length;
        REQUIRE_THROWS_AS(Error, length = U8StringView{str}.length());
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
                REQUIRE_EQUAL(U8StringView{actualText}.length(), data.maximumCharacters);
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
};

