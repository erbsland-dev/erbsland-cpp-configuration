// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/conf/String.hpp>
#include <erbsland/conf/CaseSensitivity.hpp>
#include <erbsland/conf/Bytes.hpp>
#include <erbsland/conf/Error.hpp>
#include <erbsland/conf/impl/utf8/U8StringView.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <compare>

using namespace el::conf;

class StringUtf8ExtensionsTest final : public el::UnitTest {
public:
    void testIsValidUtf8() {
        const auto valid = std::vector<String>{
            {},
            u8"a",
            u8"😀",
            u8"text😀text",
            u8"     text→⟨•×☮️       \n\r\r\r",
        };
        for (const auto &s : valid) {
            REQUIRE(s.isValidUtf8());
        }
        const auto invalid = std::vector<Bytes>{
            Bytes::fromHex(u8"41 42 43 80 41 42"),
            Bytes::fromHex(u8"41 E0 80 41 42"),
        };
        for (const auto &bytes : invalid) {
            String s{reinterpret_cast<const char8_t*>(bytes.data()), bytes.size()};
            REQUIRE_FALSE(s.isValidUtf8());
        }
    }

    void testCharacterLength() {
        const auto data = std::vector<std::pair<String, std::size_t>>{
            {u8"", 0},
            {u8"a", 1},
            {u8"😀", 1},
            {u8"text😀text", 9},
            {u8"→⟨•×", 4},
        };
        for (const auto &d : data) {
            REQUIRE_EQUAL(d.first.characterLength(), d.second);
        }
        auto invalidBytes = Bytes::fromHex(u8"41 80 42");
        String bad{reinterpret_cast<const char8_t*>(invalidBytes.data()), invalidBytes.size()};
        std::size_t len = 0;
        REQUIRE_THROWS_AS(Error, len = bad.characterLength());
        (void)len;
    }

    void testCharacterCompare() {
        struct TD { String a; String b; std::strong_ordering exp; };
        const auto cases = std::vector<TD>{
            {u8"", u8"", std::strong_ordering::equal},
            {u8"", u8"a", std::strong_ordering::less},
            {u8"a", u8"", std::strong_ordering::greater},
            {u8"abc", u8"abc", std::strong_ordering::equal},
            {u8"abc", u8"abd", std::strong_ordering::less},
            {u8"abd", u8"abc", std::strong_ordering::greater},
            {u8"→⟨•×😀", u8"→⟨•×😀", std::strong_ordering::equal},
        };
        for (const auto &c : cases) {
            REQUIRE_EQUAL(c.a.characterCompare(c.b), c.exp);
        }
        // Case-insensitive (ASCII)
        REQUIRE_EQUAL(String{u8"config"}.characterCompare(u8"CONFIG", CaseSensitivity::CaseInsensitive), std::strong_ordering::equal);
        REQUIRE(String{u8"aaaa"}.characterCompare(u8"AAAB", CaseSensitivity::CaseInsensitive) < 0);
        REQUIRE(String{u8"AAAB"}.characterCompare(u8"aaaa", CaseSensitivity::CaseInsensitive) > 0);
    }

    void testStartsAndEndsWith() {
        // startsWith
        REQUIRE(String{u8"abc"}.startsWith(u8""));
        REQUIRE(String{u8"abc"}.startsWith(u8"ab"));
        REQUIRE_FALSE(String{u8"abc"}.startsWith(u8"ac"));
        REQUIRE(String{u8"→⟨•×😀abcdef"}.startsWith(u8"→⟨•×😀"));
        // endsWith
        REQUIRE(String{u8"abc"}.endsWith(u8""));
        REQUIRE(String{u8"abc"}.endsWith(u8"bc"));
        REQUIRE_FALSE(String{u8"abc"}.endsWith(u8"xbc"));
        REQUIRE(String{u8"abcdef→⟨•×😀"}.endsWith(u8"•×😀"));
        // case-insensitive
        REQUIRE(String{u8"CONFIG"}.startsWith(u8"con", CaseSensitivity::CaseInsensitive));
        REQUIRE(String{u8"app.LOG"}.endsWith(u8".log", CaseSensitivity::CaseInsensitive));
        REQUIRE_FALSE(String{u8"note.txt"}.endsWith(u8".md", CaseSensitivity::CaseInsensitive));
    }

    void testContains() {
        // default case-sensitive
        REQUIRE(String{u8""}.contains(u8""));
        REQUIRE_FALSE(String{u8""}.contains(u8"a"));
        REQUIRE(String{u8"a"}.contains(u8""));
        REQUIRE(String{u8"abc"}.contains(u8"ab"));
        REQUIRE(String{u8"abc"}.contains(u8"bc"));
        REQUIRE(String{u8"→⟨•×😀abcdef"}.contains(u8"⟨•×😀a"));
        REQUIRE_FALSE(String{u8"abc"}.contains(u8"ac"));
        REQUIRE_FALSE(String{u8"text"}.contains(u8"XYZ"));
        // case-insensitive
        REQUIRE(String{u8"CONFIG"}.contains(u8"fig", CaseSensitivity::CaseInsensitive));
        REQUIRE(String{u8"Readme.MD"}.contains(u8".md", CaseSensitivity::CaseInsensitive));
        REQUIRE_FALSE(String{u8"abc"}.contains(u8"ABD", CaseSensitivity::CaseInsensitive));
    }

    void testSplitAndJoin() {
        const auto parts = String{u8",a,,b,"}.split(U',');
        REQUIRE_EQUAL(parts.size(), 5U);
        REQUIRE_EQUAL(parts[0], u8"");
        REQUIRE_EQUAL(parts[1], u8"a");
        REQUIRE_EQUAL(parts[2], u8"");
        REQUIRE_EQUAL(parts[3], u8"b");
        REQUIRE_EQUAL(parts[4], u8"");

        const auto limited = String{u8"a,b,c,d"}.split(U',', 2);
        REQUIRE_EQUAL(limited.size(), 3U);
        REQUIRE_EQUAL(limited[0], u8"a");
        REQUIRE_EQUAL(limited[1], u8"b");
        REQUIRE_EQUAL(limited[2], u8"c,d");

        const auto emptyParts = String{u8""}.split(U',');
        REQUIRE_EQUAL(emptyParts.size(), 1U);
        REQUIRE_EQUAL(emptyParts[0], u8"");

        const String glue{u8"|"};
        REQUIRE_EQUAL(glue.join(parts), u8"|a||b|");

        const String emptyGlue{u8""};
        const StringList none{};
        REQUIRE_EQUAL(emptyGlue.join(none), String{});
    }

    void testTransformedAndForEachCharacter() {
        auto src = String{u8"AbC→😀"};
        auto lower = src.transformed([](char32_t c){ return c + 1; });
        REQUIRE_EQUAL(lower, String{u8"BcD↓😁"});
        // forEachCharacter reconstruct
        String rebuilt;
        std::size_t count = 0;
        src.forEachCharacter([&](char32_t c){ rebuilt.append(c); ++count; });
        REQUIRE_EQUAL(rebuilt, src);
        REQUIRE_EQUAL(count, src.characterLength());
    }

    void testToSafeText() {
        // valid text: toSafeText should be escaped with ErrorText mode and not truncated for large maximum
        auto s = String{u8"line1\n😀line2"};
        auto safe = s.toSafeText(1000);
        REQUIRE_EQUAL(safe, impl::U8StringView{s}.toEscaped(EscapeMode::ErrorText));
        // truncated center with ellipsis
        auto longText = String{u8"昨日、東京の小さなカフェで「pancake🍓」と☕️を頼んだら、すごく美味しかった！おすすめ😊"};
        auto truncated = longText.toSafeText(20);
        REQUIRE(truncated.characterLength() <= 20);
        // invalid bytes -> fixed message
        auto invalid = Bytes::fromHex(u8"41 80 42");
        String bad{reinterpret_cast<const char8_t*>(invalid.data()), invalid.size()};
        REQUIRE_EQUAL(bad.toSafeText(), String{u8"<contains UTF-8 encoding errors>"});
    }
};

TESTED_TARGETS(String)
