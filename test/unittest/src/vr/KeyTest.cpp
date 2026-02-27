// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/impl/vr/Key.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <format>


using namespace el::conf;
using namespace el::conf::impl;


TESTED_TARGETS(Key)
class KeyTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultConstruction() {
        Key key;
        REQUIRE_EQUAL(key.size(), 0);
        REQUIRE(key.elements().empty());
        REQUIRE(key.toText().empty());
    }

    void testSingleElementConstructionAndAccess() {
        Key key{u8"Alpha"};

        REQUIRE_EQUAL(key.size(), 1);
        REQUIRE_EQUAL(key.elements().size(), 1);
        REQUIRE(key.element(0) == u8"Alpha");
        REQUIRE(key.toText() == u8"Alpha");
    }

    void testMultipleElementConstructionAndAccess() {
        Key key{StringList{u8"Alpha", u8"Beta", u8"Gamma"}};

        REQUIRE_EQUAL(key.size(), 3);
        REQUIRE_EQUAL(key.elements().size(), 3);
        REQUIRE(key.element(0) == u8"Alpha");
        REQUIRE(key.element(1) == u8"Beta");
        REQUIRE(key.element(2) == u8"Gamma");
        REQUIRE(key.toText() == u8"Alpha,Beta,Gamma");
    }

    void testElementOutOfRangeReturnsEmptyString() {
        Key key{StringList{u8"Alpha", u8"Beta"}};

        REQUIRE(key.element(2).empty());
        REQUIRE(key.element(99).empty());

        Key emptyKey;
        REQUIRE(emptyKey.element(0).empty());
    }

    void testCompareAllElementsCaseSensitive() {
        Key left{StringList{u8"Alpha", u8"Beta"}};
        Key same{StringList{u8"Alpha", u8"Beta"}};
        Key differentCase{StringList{u8"alpha", u8"Beta"}};
        Key differentValue{StringList{u8"Alpha", u8"Gamma"}};

        REQUIRE(left.isEqual(same, CaseSensitivity::CaseSensitive));
        REQUIRE_FALSE(left.isEqual(differentCase, CaseSensitivity::CaseSensitive));
        REQUIRE_FALSE(left.isEqual(differentValue, CaseSensitivity::CaseSensitive));
    }

    void testCompareAllElementsCaseInsensitive() {
        Key left{StringList{u8"Alpha", u8"Beta"}};
        Key right{StringList{u8"ALPHA", u8"beta"}};

        REQUIRE(left.isEqual(right, CaseSensitivity::CaseInsensitive));
    }

    void testCompareSingleElementByIndex() {
        Key left{StringList{u8"Alpha", u8"Beta"}};
        Key right{StringList{u8"ALPHA", u8"Beta"}};

        REQUIRE_FALSE(left.isEqual(right, CaseSensitivity::CaseSensitive, 0));
        REQUIRE(left.isEqual(right, CaseSensitivity::CaseInsensitive, 0));
        REQUIRE(left.isEqual(right, CaseSensitivity::CaseSensitive, 1));
    }

    void testCompareSingleElementOutOfRange() {
        Key one{u8"Alpha"};
        Key oneAndEmpty{StringList{u8"Alpha", u8""}};
        Key oneAndValue{StringList{u8"Alpha", u8"Beta"}};

        REQUIRE(one.isEqual(oneAndEmpty, CaseSensitivity::CaseSensitive, 1));
        REQUIRE_FALSE(one.isEqual(oneAndValue, CaseSensitivity::CaseSensitive, 1));
        REQUIRE(one.isEqual(oneAndEmpty, CaseSensitivity::CaseSensitive));
        REQUIRE_FALSE(one.isEqual(oneAndValue, CaseSensitivity::CaseSensitive));
    }

    void testFormatter() {
        const Key key{StringList{u8"a", u8"b"}};
        REQUIRE_EQUAL(std::format("{}", key), "a,b");
    }
};
