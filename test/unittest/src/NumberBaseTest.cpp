// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/conf/impl/NumberBase.hpp>
#include <../../../src/erbsland/conf/impl/decoder/DecodedChar.hpp>
#include <erbsland/conf/Position.hpp>
#include <erbsland/unittest/UnitTest.hpp>

using namespace el::conf;
using impl::NumberBase;
using impl::DecodedChar;

TESTED_TARGETS(NumberBase DecodedChar)
class NumberBaseTest final : public el::UnitTest {
public:
    static auto makeChar(char32_t c) -> DecodedChar {
        return DecodedChar{c, 0, Position{1,1}};
    }

    void testDefaultConstructor() {
        NumberBase base;
        REQUIRE(base.raw() == NumberBase::Decimal);
    }

    void testProperties() {
        NumberBase bin(NumberBase::Binary);
        NumberBase dec(NumberBase::Decimal);
        NumberBase hex(NumberBase::Hexadecimal);

        REQUIRE(bin.maximumDigits() == limits::maxBinaryDigits);
        REQUIRE(dec.maximumDigits() == limits::maxDecimalDigits);
        REQUIRE(hex.maximumDigits() == limits::maxHexadecimalDigits);

        REQUIRE(bin.factor() == 2U);
        REQUIRE(dec.factor() == 10U);
        REQUIRE(hex.factor() == 16U);
    }

    void testIsValidDigit() {
        NumberBase bin(NumberBase::Binary);
        NumberBase dec(NumberBase::Decimal);
        NumberBase hex(NumberBase::Hexadecimal);

        auto zero = makeChar(U'0');
        auto one = makeChar(U'1');
        auto two = makeChar(U'2');
        auto nine = makeChar(U'9');
        auto aLower = makeChar(U'a');
        auto fUpper = makeChar(U'F');
        auto gUpper = makeChar(U'G');

        REQUIRE(bin.isValidDigit(zero));
        REQUIRE(bin.isValidDigit(one));
        REQUIRE_FALSE(bin.isValidDigit(two));
        REQUIRE_FALSE(bin.isValidDigit(aLower));

        REQUIRE(dec.isValidDigit(zero));
        REQUIRE(dec.isValidDigit(nine));
        REQUIRE_FALSE(dec.isValidDigit(aLower));

        REQUIRE(hex.isValidDigit(zero));
        REQUIRE(hex.isValidDigit(nine));
        REQUIRE(hex.isValidDigit(aLower));
        REQUIRE(hex.isValidDigit(fUpper));
        REQUIRE_FALSE(hex.isValidDigit(gUpper));
    }
};

