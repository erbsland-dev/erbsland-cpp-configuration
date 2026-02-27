// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/Error.hpp>
#include <erbsland/conf/impl/vr/KeyIndex.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <format>


using namespace el::conf;
using namespace el::conf::impl;


TESTED_TARGETS(KeyIndex)
class KeyIndexTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testConstructorAndAccessors() {
        const auto name = Name::createRegular(u8"my_index");
        const KeyIndex index{name, CaseSensitivity::CaseInsensitive, 2};

        REQUIRE(index.name() == name);
        REQUIRE(index.caseSensitivity() == CaseSensitivity::CaseInsensitive);
    }

    void testConstructorRejectsZeroElementCount() {
        REQUIRE_THROWS_AS(Error, KeyIndex(Name::createRegular(u8"index"), CaseSensitivity::CaseSensitive, 0));
    }

    void testEmptyIndexSingleElement() {
        const KeyIndex index{Name::createRegular(u8"single"), CaseSensitivity::CaseSensitive, 1};

        REQUIRE_FALSE(index.hasKey(u8"alpha"));
        REQUIRE_FALSE(index.hasKey(Key{u8"alpha"}));
        REQUIRE_FALSE(index.hasKey(u8"alpha", 0));
        REQUIRE_FALSE(index.hasKey(u8"alpha", 1));
    }

    void testEmptyIndexMultiElement() {
        const KeyIndex index{Name::createRegular(u8"multi"), CaseSensitivity::CaseSensitive, 3};

        REQUIRE_FALSE(index.hasKey(u8"alpha,beta,gamma"));
        REQUIRE_FALSE(index.hasKey(Key{StringList{u8"alpha", u8"beta", u8"gamma"}}));
        REQUIRE_FALSE(index.hasKey(u8"alpha", 0));
        REQUIRE_FALSE(index.hasKey(u8"beta", 1));
        REQUIRE_FALSE(index.hasKey(u8"gamma", 2));
        REQUIRE_FALSE(index.hasKey(u8"alpha,beta", 3));
    }

    void testSingleElementCaseSensitiveIndex() {
        KeyIndex index{Name::createRegular(u8"single_cs"), CaseSensitivity::CaseSensitive, 1};

        REQUIRE(index.tryAddKey(Key{u8"Alpha"}));
        REQUIRE_FALSE(index.tryAddKey(Key{u8"Alpha"}));
        REQUIRE(index.tryAddKey(Key{u8"alpha"}));

        REQUIRE(index.hasKey(u8"Alpha"));
        REQUIRE(index.hasKey(Key{u8"Alpha"}));
        REQUIRE(index.hasKey(u8"alpha"));
        REQUIRE(index.hasKey(Key{u8"alpha"}));
        REQUIRE_FALSE(index.hasKey(u8"ALPHA"));
        REQUIRE(index.hasKey(u8"Alpha", 0));
        REQUIRE_FALSE(index.hasKey(u8"ALPHA", 0));
        REQUIRE_FALSE(index.hasKey(u8"Alpha", 1));
    }

    void testSingleElementCaseInsensitiveIndex() {
        KeyIndex index{Name::createRegular(u8"single_ci"), CaseSensitivity::CaseInsensitive, 1};

        REQUIRE(index.tryAddKey(Key{u8"Alpha"}));
        REQUIRE_FALSE(index.tryAddKey(Key{u8"ALPHA"}));

        REQUIRE(index.hasKey(u8"Alpha"));
        REQUIRE(index.hasKey(u8"ALPHA"));
        REQUIRE(index.hasKey(Key{u8"alpha"}));
        REQUIRE(index.hasKey(u8"aLpHa", 0));
    }

    void testMultiElementCaseSensitiveIndex() {
        KeyIndex index{Name::createRegular(u8"multi_cs"), CaseSensitivity::CaseSensitive, 2};

        REQUIRE(index.tryAddKey(Key{StringList{u8"Alpha", u8"Beta"}}));
        REQUIRE_FALSE(index.tryAddKey(Key{StringList{u8"Alpha", u8"Beta"}}));
        REQUIRE(index.tryAddKey(Key{StringList{u8"Alpha", u8"beta"}}));
        REQUIRE(index.tryAddKey(Key{StringList{u8"alpha", u8"Beta"}}));

        REQUIRE(index.hasKey(u8"Alpha,Beta"));
        REQUIRE(index.hasKey(Key{StringList{u8"Alpha", u8"Beta"}}));
        REQUIRE_FALSE(index.hasKey(u8"ALPHA,BETA"));

        REQUIRE(index.hasKey(u8"Alpha", 0));
        REQUIRE(index.hasKey(u8"alpha", 0));
        REQUIRE(index.hasKey(u8"Beta", 1));
        REQUIRE(index.hasKey(u8"beta", 1));
        REQUIRE_FALSE(index.hasKey(u8"ALPHA", 0));
        REQUIRE_FALSE(index.hasKey(u8"BETA", 1));
        REQUIRE_FALSE(index.hasKey(u8"Alpha", 2));
    }

    void testMultiElementCaseInsensitiveIndex() {
        KeyIndex index{Name::createRegular(u8"multi_ci"), CaseSensitivity::CaseInsensitive, 2};

        REQUIRE(index.tryAddKey(Key{StringList{u8"Alpha", u8"Beta"}}));
        REQUIRE_FALSE(index.tryAddKey(Key{StringList{u8"ALPHA", u8"beta"}}));

        REQUIRE(index.hasKey(u8"alpha,beta"));
        REQUIRE(index.hasKey(Key{StringList{u8"ALPHA", u8"BETA"}}));
        REQUIRE(index.hasKey(u8"aLpHa", 0));
        REQUIRE(index.hasKey(u8"BeTa", 1));
    }

    void testTryAddKeyRejectsMismatchingElementCount() {
        KeyIndex index{Name::createRegular(u8"count_check"), CaseSensitivity::CaseSensitive, 2};

        REQUIRE_THROWS_AS(Error, index.tryAddKey(Key{}));
        REQUIRE_THROWS_AS(Error, index.tryAddKey(Key{u8"only_one"}));
        REQUIRE_THROWS_AS(Error, index.tryAddKey(Key{StringList{u8"one", u8"two", u8"three"}}));
    }

    void testHasKeyStringWithMismatchingElementCount() {
        KeyIndex index{Name::createRegular(u8"string_count"), CaseSensitivity::CaseSensitive, 3};
        REQUIRE(index.tryAddKey(Key{StringList{u8"a", u8"b", u8"c"}}));

        REQUIRE_FALSE(index.hasKey(u8"a,b"));
        REQUIRE_FALSE(index.hasKey(u8"a"));
        REQUIRE(index.hasKey(u8"a,b,c"));
    }

    void testIndexWithOneSeveralAndLargeNumberOfKeys() {
        KeyIndex single{Name::createRegular(u8"one"), CaseSensitivity::CaseSensitive, 1};
        REQUIRE(single.tryAddKey(Key{u8"one"}));
        REQUIRE(single.hasKey(u8"one"));

        KeyIndex several{Name::createRegular(u8"several"), CaseSensitivity::CaseSensitive, 2};
        REQUIRE(several.tryAddKey(Key{StringList{u8"a1", u8"b1"}}));
        REQUIRE(several.tryAddKey(Key{StringList{u8"a2", u8"b2"}}));
        REQUIRE(several.tryAddKey(Key{StringList{u8"a3", u8"b3"}}));
        REQUIRE(several.hasKey(u8"a2,b2"));
        REQUIRE(several.hasKey(u8"a3", 0));
        REQUIRE(several.hasKey(u8"b1", 1));
        REQUIRE_FALSE(several.hasKey(u8"a4,b4"));

        KeyIndex many{Name::createRegular(u8"many"), CaseSensitivity::CaseSensitive, 1};
        constexpr std::size_t keyCount = 2000;
        for (std::size_t i = 0; i < keyCount; ++i) {
            const String keyText{std::format("k_{:04}", i)};
            WITH_CONTEXT(keyText);
            REQUIRE(many.tryAddKey(Key{keyText}));
        }

        REQUIRE(many.hasKey(u8"k_0000"));
        REQUIRE(many.hasKey(u8"k_1099"));
        REQUIRE(many.hasKey(u8"k_1999"));
        REQUIRE_FALSE(many.hasKey(u8"k_2000"));
    }
};
