// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/utf8/U8Format.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


using namespace erbsland::conf;


TESTED_TARGETS(u8format)
class U8FormatTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testCharPointer() {
        auto result = impl::u8format("Answer {}", 42);
        REQUIRE(result == u8"Answer 42");
    }

    void testChar8Pointer() {
        auto result = impl::u8format(u8"Value {}", 15);
        REQUIRE(result == u8"Value 15");
    }

    void testMixedArguments() {
        const char8_t *fmt = u8"{} {}";
        auto result = impl::u8format(fmt, String{u8"text"}, 7);
        REQUIRE(result == u8"text 7");
    }
};

