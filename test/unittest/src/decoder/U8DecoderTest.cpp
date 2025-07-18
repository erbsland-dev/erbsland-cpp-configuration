// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/Char.hpp>
#include <erbsland/conf/Error.hpp>
#include <erbsland/unittest/UnitTest.hpp>
#include <erbsland/conf/impl/utf8/U8Decoder.hpp>
#include <erbsland/conf/Bytes.hpp>

#include <vector>


using namespace erbsland::conf;
using impl::U8Decoder;
using impl::Char;


TESTED_TARGETS(U8Decoder)
class U8DecoderTest final : public el::UnitTest {
public:
    void testAsciiDecode() {
        Bytes bytes = Bytes::fromHex("41 42");
        std::size_t pos = 0;
        auto c = U8Decoder<std::byte>::decodeChar(bytes.raw(), pos);
        REQUIRE(c == Char{'A'});
        REQUIRE(pos == 1);
        c = U8Decoder<std::byte>::decodeChar(bytes.raw(), pos);
        REQUIRE(c == Char{'B'});
        REQUIRE(pos == 2);
    }

    void testMultiByteDecode() {
        Bytes bytes = Bytes::fromHex("C3 97 E2 86 90 F0 9F 98 84");
        std::size_t pos = 0;
        auto c = U8Decoder<std::byte>::decodeChar(bytes.raw(), pos);
        REQUIRE(c == Char{0x00D7});
        REQUIRE(pos == 2);
        c = U8Decoder<std::byte>::decodeChar(bytes.raw(), pos);
        REQUIRE(c == Char{0x2190});
        REQUIRE(pos == 5);
        c = U8Decoder<std::byte>::decodeChar(bytes.raw(), pos);
        REQUIRE(c == Char{0x1F604});
        REQUIRE(pos == bytes.size());
    }

    void testDecodeAllAndCount() {
        Bytes bytes = Bytes::fromHex("C3 97 E2 86 90 F0 9F 98 84");
        U8Decoder<std::byte> decoder(std::span(bytes.raw().begin(), bytes.raw().size()));
        std::vector<char32_t> decoded;
        decoder.decodeAll([&](Char ch) { decoded.push_back(static_cast<char32_t>(ch)); });
        REQUIRE(decoded.size() == 3);
        REQUIRE(decoded[0] == 0x00D7);
        REQUIRE(decoded[1] == 0x2190);
        REQUIRE(decoded[2] == 0x1F604);
        REQUIRE(decoder.countAll() == 3);
    }

    void testInvalidUtf8Sequences() {
        const auto invalidSequences = std::vector<std::pair<Bytes, std::string_view>>{
            {Bytes::fromHex("C0 80"), "Overlong two-byte sequence (U+0000 encoded in two bytes)"},
            {Bytes::fromHex("C1 80"), "Overlong two-byte sequence (U+0001 encoded in two bytes)"},
            {Bytes::fromHex("C2"), "Truncated two-byte sequence"},
            {Bytes::fromHex("C2 41"), "Invalid continuation byte in two-byte sequence"},
            {Bytes::fromHex("E2 82"), "Truncated three-byte sequence"},
            {Bytes::fromHex("E2 41 80"), "Invalid continuation byte in three-byte sequence"},
            {Bytes::fromHex("E0 80 80"), "Overlong three-byte sequence (U+0000 encoded in three bytes)"},
            {Bytes::fromHex("E0 9F BF"), "Overlong three-byte sequence (U+07FF encoded in three bytes)"},
            {Bytes::fromHex("ED A0 80"), "UTF-16 surrogate half U+D800 encoded"},
            {Bytes::fromHex("F0 41 80 80"), "Invalid continuation byte in four-byte sequence"},
            {Bytes::fromHex("F0 9F BF"), "Truncated four-byte sequence"},
            {Bytes::fromHex("F0 80 B0 B0"), "Overlong four-byte sequence (U+0FFF encoded in four bytes)"},
            {Bytes::fromHex("F0 8F BF BF"), "Overlong four-byte sequence (U+03FFF encoded in four bytes)"},
            {Bytes::fromHex("F4 90 80 80"), "Codepoint above U+10FFFF"},
            {Bytes::fromHex("F8 80 80 80 80"), "Invalid start byte (five-byte sequence)"},
            {Bytes::fromHex("FF"), "Invalid start byte (0xFF)"},
        };
        for (const auto &[bytes, description] : invalidSequences) {
            std::size_t pos = 0;
            runWithContext(SOURCE_LOCATION(), [&] {
                try {
                    static_cast<void>(U8Decoder<const std::byte>::decodeChar(bytes, pos));
                    REQUIRE(false);
                } catch (const Error &e) {
                    REQUIRE(e.category() == ErrorCategory::Encoding);
                }
                REQUIRE(pos == 0);
            }, [&]() -> std::string {
                return std::format("Decoded invalid sequence: {}", description);
            });
        }
    }
};
