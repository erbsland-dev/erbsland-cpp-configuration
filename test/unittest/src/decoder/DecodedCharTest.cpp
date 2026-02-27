// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"


#include <erbsland/conf/impl/decoder/DecodedChar.hpp>
#include <erbsland/conf/Position.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace erbsland::conf;
using impl::Char;
using impl::CharClass;
using impl::DecodedChar;
using impl::internalView;


TESTED_TARGETS(DecodedChar)
class DecodedCharTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    DecodedChar character;

    void testDefaultConstructor() {
        character = {};
        REQUIRE(character == Char::EndOfData);
        REQUIRE_EQUAL(character.index(), 0U);
        REQUIRE(character.position().isUndefined());
    }

    void testParameterizedConstructorAndAccessors() {
        Position position{3, 4};
        character = DecodedChar{U'A', 7, position};
        REQUIRE(character.isChar(U'A'));
        REQUIRE_EQUAL(static_cast<char32_t>(character), U'A');
        REQUIRE_EQUAL(character.index(), 7U);
        REQUIRE(character.position() == position);
    }

    void testCopyAndMove() {
        Position position{2, 5};
        character = DecodedChar{U'B', 9, position};
        DecodedChar copy{character};
        REQUIRE(copy.raw() == character.raw());
        REQUIRE(copy.index() == character.index());
        REQUIRE(copy.position() == character.position());
    }
};
