// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "../TestHelper.hpp"

#include <erbsland/conf/impl/Char.hpp>
#include <erbsland/conf/impl/utf8/U8StringView.hpp>
#include <erbsland/conf/EscapeMode.hpp>

#include <stdexcept>


using namespace el::conf;
using impl::Char;
using impl::U8StringView;


TESTED_TARGETS(Char)
class CharTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    Char c;

    void testBasicUsage() {
        c = {};
        REQUIRE(c == Char::EndOfData);
        c = Char{Char::LcA};
        REQUIRE(c == Char::LcA);
        c = Char{U'A'};
        REQUIRE(c == Char::UcA);
        // Compare with itself.
        WITH_CONTEXT(requireAllOperators(
            Char{Char::Digit0}, Char{Char::LcA}, Char{},
            Char{Char::Digit0}, Char{Char::LcA}, Char{}
        ));
        // Compare with enum.
        WITH_CONTEXT(requireAllOperators(
            Char{Char::Digit0}, Char{Char::LcA}, Char{},
            Char::Digit0, Char::LcA, Char::EndOfData
        ));
        WITH_CONTEXT(requireAllOperators(
            Char::Digit0, Char::LcA, Char::EndOfData,
            Char{Char::Digit0}, Char{Char::LcA}, Char{}
        ));
        // Compare with char32_t
        WITH_CONTEXT(requireAllOperators(
            Char{Char::Digit0}, Char{Char::UcA}, Char{Char::LcZ},
            U'0', U'A', U'z'
        ));
        WITH_CONTEXT(requireAllOperators(
            U'0', U'A', U'z',
            Char{Char::Digit0}, Char{Char::UcA}, Char{Char::LcZ}
        ));
    }

    void testAppendToAndUtf8Size() {
        String out;

        struct TestSequence {
            Char character;
            Bytes expectedBytes;
        };
        const auto testSequences = std::vector<TestSequence>{
            // --- boundaries of UTF-8 encodings
            {Char{0x0000}, Bytes::fromHex("00")}, // minimum value
            {Char{0x007F}, Bytes::fromHex("7F")}, // last 1 byte sequence
            {Char{0x0080}, Bytes::fromHex("C2 80")}, // first 2 byte sequence
            {Char{0x07FF}, Bytes::fromHex("DF BF")}, // last 2 byte sequence
            {Char{0x0800}, Bytes::fromHex("E0 A0 80")}, // first 3 byte sequence
            {Char{0xFFFF}, Bytes::fromHex("EF BF BF")}, // last 3 byte sequence
            {Char{0x10000}, Bytes::fromHex("F0 90 80 80")}, // first 4 byte sequence
            {Char{0x10FFFF}, Bytes::fromHex("F4 8F BF BF")}, // maximum valid value

            // --- a few additional characters
            {Char{U'A'}, Bytes::fromHex("41")}, // 7-bit ASCII character
            {Char{0x00D7}, Bytes::fromHex("C3 97")}, // 2 byte sequence
            {Char{0x2190}, Bytes::fromHex("E2 86 90")}, // 3 byte sequence
            {Char{0x1F604}, Bytes::fromHex("F0 9F 98 84")}, // 4 byte sequence

            // Invalid Unicode above the valid range results in the replacement character
            {Char{0x110000}, Bytes::fromHex("EF BF BD")},
        };

        for (const auto&[character, expectedBytes] : testSequences) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                c = character;
                String actualString;
                std::u8string actualStdString;
                c.appendTo(actualString);
                c.appendTo(actualStdString);
                auto expectedString = U8StringView::fromBytes(expectedBytes);
                REQUIRE_EQUAL(actualString, expectedString);
                REQUIRE(actualStdString == expectedString.raw());
                REQUIRE_EQUAL(c.utf8Size(), expectedString.size());
            }, [&]() -> std::string {
                return std::format("Failed for character U+{:04x}.", static_cast<uint32_t>(character));
            });
        }
    }

    void testToLowerCase() {
        REQUIRE_EQUAL(Char{Char::UcA}.toLowerCase(), Char::LcA);
        REQUIRE_EQUAL(Char{Char::UcZ}.toLowerCase(), Char::LcZ);
        REQUIRE_EQUAL(Char{Char::LcA}.toLowerCase(), Char::LcA);
        REQUIRE_EQUAL(Char{Char::Micro}.toLowerCase(), Char::Micro);
    }

    void testValidUnicode() {
        REQUIRE(Char{0}.isValidUnicode());
        REQUIRE(Char{U'A'}.isValidUnicode());
        REQUIRE(Char{Char::ByteOrderMark}.isValidUnicode());
        REQUIRE_FALSE(Char{0xD800}.isValidUnicode());
        REQUIRE_FALSE(Char{0x110000}.isValidUnicode());
    }

    void testValidEscapeUnicode() {
        REQUIRE_FALSE(Char{0}.isValidEscapeUnicode());
        REQUIRE(Char{U'A'}.isValidEscapeUnicode());
        REQUIRE_FALSE(Char{Char::ByteOrderMark}.isValidEscapeUnicode());
        REQUIRE_FALSE(Char{0xD800}.isValidEscapeUnicode());
        REQUIRE_FALSE(Char{0x110000}.isValidEscapeUnicode());
    }

    void testAppendEscaped() {

        struct EscapeSequence {
            Char character;
            EscapeMode mode;
            String expected;
        };
        // In case of an error, there is no output
        const auto escapeSequences = std::vector<EscapeSequence>{
            // --- Text
            {Char{U'A'}, EscapeMode::Text, u8"A"},
            {Char{U'a'}, EscapeMode::Text, u8"a"},
            {Char{U' '}, EscapeMode::Text, u8" "},
            {Char{U'!'}, EscapeMode::Text, u8"!"},
            {Char{U'"'}, EscapeMode::Text, u8"\\\""},
            {Char{U'.'}, EscapeMode::Text, u8"."},
            {Char{U':'}, EscapeMode::Text, u8":"},
            {Char{U'='}, EscapeMode::Text, u8"="},
            {Char{U'<'}, EscapeMode::Text, u8"<"},
            {Char{U'\\'}, EscapeMode::Text, u8"\\\\"},
            {Char{0x0001}, EscapeMode::Text, u8"\\u{1}"},
            {Char{0x0002}, EscapeMode::Text, u8"\\u{2}"},
            {Char{0x0007}, EscapeMode::Text, u8"\\u{7}"},
            {Char{0x0009}, EscapeMode::Text, u8"\\t"},
            {Char{0x000a}, EscapeMode::Text, u8"\\n"},
            {Char{0x000c}, EscapeMode::Text, u8"\\u{c}"},
            {Char{0x000d}, EscapeMode::Text, u8"\\r"},
            {Char{0x007f}, EscapeMode::Text, u8"\\u{7f}"},
            {Char{0x0080}, EscapeMode::Text, u8"\\u{80}"},
            {Char{0x00a0}, EscapeMode::Text, u8"\\u{a0}"},
            {Char{0x00a1}, EscapeMode::Text, u8"\U000000a1"},
            {Char{0x01af}, EscapeMode::Text, u8"\U000001af"},
            {Char{0x2892}, EscapeMode::Text, u8"\U00002892"},
            {Char{0x7cbea}, EscapeMode::Text, u8"\U0007cbea"},
            {Char{0x102a89}, EscapeMode::Text, u8"\U00102a89"},
            {Char{0x10ffff}, EscapeMode::Text, u8"\U0010ffff"},

            // error cases
            {Char{0x0000}, EscapeMode::Text, {}},
            {Char{0xd800}, EscapeMode::Text, {}},
            {Char{0xd928}, EscapeMode::Text, {}},
            {Char{0xdfff}, EscapeMode::Text, {}},

            // --- FullTextName
            {Char{U'A'}, EscapeMode::FullTextName, u8"A"},
            {Char{U'a'}, EscapeMode::FullTextName, u8"a"},
            {Char{U' '}, EscapeMode::FullTextName, u8" "},
            {Char{U'!'}, EscapeMode::FullTextName, u8"!"},
            {Char{U'"'}, EscapeMode::FullTextName, u8"\\u{22}"},
            {Char{U'.'}, EscapeMode::FullTextName, u8"\\u{2e}"},
            {Char{U':'}, EscapeMode::FullTextName, u8"\\u{3a}"},
            {Char{U'='}, EscapeMode::FullTextName, u8"\\u{3d}"},
            {Char{U'<'}, EscapeMode::FullTextName, u8"<"},
            {Char{U'\\'}, EscapeMode::FullTextName, u8"\\u{5c}"},
            {Char{0x0001}, EscapeMode::FullTextName, u8"\\u{1}"},
            {Char{0x0002}, EscapeMode::FullTextName, u8"\\u{2}"},
            {Char{0x0007}, EscapeMode::FullTextName, u8"\\u{7}"},
            {Char{0x0009}, EscapeMode::FullTextName, u8"\\u{9}"},
            {Char{0x000a}, EscapeMode::FullTextName, u8"\\u{a}"},
            {Char{0x000c}, EscapeMode::FullTextName, u8"\\u{c}"},
            {Char{0x000d}, EscapeMode::FullTextName, u8"\\u{d}"},
            {Char{0x007f}, EscapeMode::FullTextName, u8"\\u{7f}"},
            {Char{0x0080}, EscapeMode::FullTextName, u8"\\u{80}"},
            {Char{0x01af}, EscapeMode::FullTextName, u8"\\u{1af}"},
            {Char{0x2892}, EscapeMode::FullTextName, u8"\\u{2892}"},
            {Char{0x7cbea}, EscapeMode::FullTextName, u8"\\u{7cbea}"},
            {Char{0x102a89}, EscapeMode::FullTextName, u8"\\u{102a89}"},
            {Char{0x10ffff}, EscapeMode::FullTextName, u8"\\u{10ffff}"},

            // error cases
            {Char{0x0000}, EscapeMode::FullTextName, {}},
            {Char{0xd800}, EscapeMode::FullTextName, {}},
            {Char{0xd928}, EscapeMode::FullTextName, {}},
            {Char{0xdfff}, EscapeMode::FullTextName, {}},
        };

        for (const auto&[character, mode, expectedString] : escapeSequences) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                String actualString;
                actualString.clear();
                character.appendEscaped(actualString, mode);
                REQUIRE_EQUAL(actualString, expectedString);
                const auto escapedSize = character.escapedUtf8Size(mode);
                REQUIRE_EQUAL(expectedString.size(), escapedSize);
            }, [&]() -> std::string {
                return std::format("Failed for character U+{:04x} in mode {}.",
                    static_cast<uint32_t>(character), mode);
            });
        }
    }
};

