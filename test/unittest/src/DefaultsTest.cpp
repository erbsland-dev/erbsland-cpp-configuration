// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/Defaults.hpp>
#include <erbsland/conf/impl/Limits.hpp>
#include <erbsland/conf/impl/crypto/ShaHash.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace erbsland::conf;


TESTED_TARGETS(defaults limits)
class DefaultsTest final : public el::UnitTest {
public:
    void testDefaultValues() {
        REQUIRE_EQUAL(impl::defaults::documentHashAlgorithm, impl::crypto::ShaHash::Algorithm::Sha3_256);
        REQUIRE_EQUAL(String{impl::defaults::textSourceIdentifier}, String{u8"text"});
        REQUIRE_EQUAL(String{impl::defaults::fileSourceIdentifier}, String{u8"file"});
        REQUIRE_EQUAL(String{impl::defaults::namePathIdentifier}, String{u8"name-path"});
        REQUIRE_EQUAL(String{impl::defaults::languageVersion}, String{u8"1.0"});
    }

    void testLimitValues() {
        REQUIRE_EQUAL(limits::maxLineLength, static_cast<std::size_t>(4000));
        REQUIRE_EQUAL(limits::maxNameLength, static_cast<std::size_t>(100));
        REQUIRE_EQUAL(limits::maxTextLength, static_cast<std::size_t>(10'000'000));
        REQUIRE_EQUAL(limits::maxDecimalDigits, static_cast<std::size_t>(19));
        REQUIRE_EQUAL(limits::maxHexadecimalDigits, static_cast<std::size_t>(16));
        REQUIRE_EQUAL(limits::maxBinaryDigits, static_cast<std::size_t>(64));
        REQUIRE_EQUAL(limits::maxNamePathLength, static_cast<std::size_t>(10));
        REQUIRE_EQUAL(limits::maxDocumentNesting, static_cast<std::size_t>(5));
    }
};

