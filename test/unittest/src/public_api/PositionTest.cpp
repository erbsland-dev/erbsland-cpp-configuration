// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/Position.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;


TESTED_TARGETS(Position)
class PositionTest final : public el::UnitTest {
public:
    void testDefaultConstructor() {
        Position pos;
        REQUIRE(pos.isUndefined());
        REQUIRE(pos.line() == -1);
        REQUIRE(pos.column() == -1);
        REQUIRE(pos.toText() == u8"undefined");
    }

    void testParameterizedConstructor() {
        Position pos(3, 7);
        REQUIRE_FALSE(pos.isUndefined());
        REQUIRE(pos.line() == 3);
        REQUIRE(pos.column() == 7);
        REQUIRE(pos.toText() == u8"3:7");
    }

    void testEqualityAndMutators() {
        Position pos1(1, 1);
        Position pos2(1, 1);
        Position pos3(2, 2);
        REQUIRE(pos1 == pos2);
        REQUIRE(pos1 != pos3);

        pos1.nextColumn();
        REQUIRE(pos1.line() == 1);
        REQUIRE(pos1.column() == 2);
        REQUIRE(pos1.toText() == u8"1:2");

        pos1.nextLine();
        REQUIRE(pos1.line() == 2);
        REQUIRE(pos1.column() == 1);
        REQUIRE(pos1.toText() == u8"2:1");
    }

    void testDigitLengths() {
        Position twoDigitLine(10, 5);
        REQUIRE(twoDigitLine.toText() == u8"10:5");

        Position twoDigitColumn(1, 23);
        REQUIRE(twoDigitColumn.toText() == u8"1:23");

        Position threeDigits(123, 456);
        REQUIRE(threeDigits.toText() == u8"123:456");
    }

    void testUndefinedComparison() {
        Position undefinedPos;
        Position definedPos(1, 1);
        REQUIRE(undefinedPos != definedPos);
        REQUIRE_FALSE(undefinedPos == definedPos);
    }
};

