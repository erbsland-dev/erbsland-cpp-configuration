// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/Bytes.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>


using namespace el::conf;


TESTED_TARGETS(Bytes)
class BytesTest final : public el::UnitTest {
public:
    Bytes bytes;

    void testDefault() {
        bytes = {};
        REQUIRE(bytes.empty());
        REQUIRE(bytes.size() == 0);
    }

    void testConstructor() {
        std::vector<std::byte> data = {std::byte{0x01}, std::byte{0x02}, std::byte{0x0a}};
        bytes = Bytes{std::span<std::byte>{data}};
        REQUIRE_EQUAL(bytes.toHex(), String{u8"01020a"});
        const Bytes bytes2 = Bytes{{std::byte{0xff}, std::byte{0xfe}, std::byte{0x67}}};
        bytes = Bytes{bytes2};
        REQUIRE_EQUAL(bytes.toHex(), String{u8"fffe67"});
        auto bytes3 = Bytes{{std::byte{0x11}, std::byte{0x55}, std::byte{0x99}}};
        bytes = Bytes{std::move(bytes3)};
        REQUIRE_EQUAL(bytes.toHex(), String{u8"115599"});
    }

    void testFromInitializer() {
        bytes = Bytes::from({uint8_t{0x01}, uint8_t{0x02}, uint8_t{0x0a}});
        REQUIRE(bytes.size() == 3);
        REQUIRE(bytes[0] == std::byte{0x01});
        REQUIRE(bytes[1] == std::byte{0x02});
        REQUIRE(bytes[2] == std::byte{0x0a});
        std::vector<uint8_t> data = {0xfc, 0xa8, 0x17};
        bytes = Bytes::from(data.begin(), data.end());
        REQUIRE_EQUAL(bytes.toHex(), String{u8"fca817"});
    }

    void testConvertFrom() {
        bytes = Bytes::convertFrom({0x123, -1});
        REQUIRE(bytes.size() == 2);
        REQUIRE(bytes[0] == std::byte{0x23});
        REQUIRE(bytes[1] == std::byte{0xff});
        std::vector<uint32_t> data = {0xfc, 0xa8, 0x17};
        bytes = Bytes::convertFrom(data.begin(), data.end());
        REQUIRE_EQUAL(bytes.toHex(), String{u8"fca817"});
    }

    void testHexRoundTrip() {
        bytes = Bytes::fromHex("0012ab");
        REQUIRE(bytes == Bytes::from({uint8_t{0x00}, uint8_t{0x12}, uint8_t{0xab}}));
        REQUIRE(bytes.toHex() == String{u8"0012ab"});
        bytes = Bytes::fromHex("     00   \t 12 \n\r\n  ab   ");
        REQUIRE(bytes == Bytes::from({uint8_t{0x00}, uint8_t{0x12}, uint8_t{0xab}}));
        REQUIRE(bytes.toHex() == String{u8"0012ab"});
    }

    void testFromHexInvalid() {
        auto bytes = Bytes::fromHex("0xgg");
        REQUIRE(bytes.empty());
    }
};

