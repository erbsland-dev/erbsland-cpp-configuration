// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include <erbsland/conf/impl/utilities/HashHelper.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>

using namespace erbsland::conf::impl;


TESTED_TARGETS(hashCombine)
class HashHelperTest final : public el::UnitTest {
public:
    template <typename T>
    static auto referenceCombine(std::size_t seed, const T &value) -> std::size_t {
        seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }

    void testIntegerCombination() {
        std::size_t seed = 0;
        const uint32_t value = 123456u;
        const auto expected = referenceCombine(seed, value);
        hashCombine(seed, value);
        REQUIRE_EQUAL(seed, expected);
    }

    void testMultipleCombination() {
        std::size_t seed = 0;
        const uint32_t v1 = 42u;
        const std::string v2 = "abc";
        hashCombine(seed, v1);
        hashCombine(seed, v2);
        auto expected = referenceCombine(referenceCombine(0u, v1), v2);
        REQUIRE_EQUAL(seed, expected);
    }

    void testOrderMatters() {
        std::size_t seed1 = 0;
        std::size_t seed2 = 0;
        hashCombine(seed1, 1u);
        hashCombine(seed1, 2u);
        hashCombine(seed2, 2u);
        hashCombine(seed2, 1u);
        REQUIRE(seed1 != seed2);
    }
};

