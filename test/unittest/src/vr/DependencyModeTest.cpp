// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/impl/vr/DependencyMode.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <format>


using namespace el::conf;
using namespace el::conf::impl;


TESTED_TARGETS(DependencyMode)
class DependencyModeTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultConstructor() {
        const DependencyMode mode;

        REQUIRE(mode == DependencyMode::Undefined);
        REQUIRE(mode.raw() == DependencyMode::Undefined);
    }

    void testEnumConstructorAndComparison() {
        const DependencyMode ifMode{DependencyMode::If};

        REQUIRE(ifMode == DependencyMode::If);
        REQUIRE(DependencyMode::If == ifMode);
        REQUIRE(ifMode != DependencyMode::IfNot);
        REQUIRE(ifMode.raw() == DependencyMode::If);
    }

    void testAssignmentFromEnum() {
        DependencyMode mode;
        mode = DependencyMode::XOR;

        REQUIRE(mode == DependencyMode::XOR);
        REQUIRE(mode.raw() == DependencyMode::XOR);
    }

    void testToText() {
        const std::array<std::pair<DependencyMode::Enum, String>, 6> mappings = {
            std::pair{DependencyMode::If, String{u8"if"}},
            std::pair{DependencyMode::IfNot, String{u8"if_not"}},
            std::pair{DependencyMode::OR, String{u8"or"}},
            std::pair{DependencyMode::XNOR, String{u8"xnor"}},
            std::pair{DependencyMode::XOR, String{u8"xor"}},
            std::pair{DependencyMode::AND, String{u8"and"}},
        };
        for (const auto &[mode, expectedText] : mappings) {
            REQUIRE_EQUAL(DependencyMode{mode}.toText(), expectedText);
        }

        REQUIRE_EQUAL(DependencyMode{}.toText(), String{u8"undefined"});
    }

    void testFromText() {
        REQUIRE(DependencyMode::fromText(u8"if") == DependencyMode::If);
        REQUIRE(DependencyMode::fromText(u8"IF") == DependencyMode::If);
        REQUIRE(DependencyMode::fromText(u8"if_not") == DependencyMode::IfNot);
        REQUIRE(DependencyMode::fromText(u8"if not") == DependencyMode::IfNot);
        REQUIRE(DependencyMode::fromText(u8"OR") == DependencyMode::OR);
        REQUIRE(DependencyMode::fromText(u8"xNoR") == DependencyMode::XNOR);
        REQUIRE(DependencyMode::fromText(u8"xor") == DependencyMode::XOR);
        REQUIRE(DependencyMode::fromText(u8"aNd") == DependencyMode::AND);

        REQUIRE(DependencyMode::fromText(String{}) == DependencyMode::Undefined);
        REQUIRE(DependencyMode::fromText(u8"unknown") == DependencyMode::Undefined);
        REQUIRE(DependencyMode::fromText(u8"123456789012345678901") == DependencyMode::Undefined);
    }

    void testFormatter() {
        REQUIRE_EQUAL(std::format("{}", DependencyMode{DependencyMode::IfNot}), "if_not");
    }
};
