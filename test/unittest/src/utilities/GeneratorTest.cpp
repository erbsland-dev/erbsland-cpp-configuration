// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/utilities/Generator.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;
using impl::Generator;


TESTED_TARGETS(Generator)
class GeneratorTest final : public el::UnitTest {
public:
    struct Tracked {
        inline static int constructions = 0;
        inline static int destructions = 0;
        inline static int copies = 0;
        inline static int moves = 0;

        int value{0};

        Tracked() { ++constructions; }
        explicit Tracked(int v) : value(v) { ++constructions; }
        Tracked(const Tracked &other) : value(other.value) { ++constructions; ++copies; }
        Tracked(Tracked &&other) noexcept : value(other.value) { ++constructions; ++moves; }
        auto operator=(const Tracked &other) -> Tracked & { value = other.value; ++copies; return *this; }
        auto operator=(Tracked &&other) noexcept -> Tracked & { value = other.value; ++moves; return *this; }
        ~Tracked() { ++destructions; }

        static void reset() { constructions = destructions = copies = moves = 0; }
    };

    static auto makeGenerator(int count) -> Generator<Tracked> {
        for (int i = 1; i <= count; ++i) {
            co_yield Tracked{i};
        }
    }

    void testDefaultConstructed() {
        Generator<Tracked> gen;
        REQUIRE_FALSE(gen.next().has_value());
        REQUIRE(gen.begin() == gen.end());
    }

    void testNext() {
        Tracked::reset();
        {
            auto gen = makeGenerator(2);
            auto a = gen.next();
            REQUIRE(a.has_value());
            REQUIRE_EQUAL(a->value, 1);
            auto b = gen.next();
            REQUIRE(b.has_value());
            REQUIRE_EQUAL(b->value, 2);
            REQUIRE_FALSE(gen.next().has_value());
        }
        REQUIRE_EQUAL(Tracked::constructions, Tracked::destructions);
        REQUIRE(Tracked::copies > 0 || Tracked::moves > 0);
    }

    void testIteration() {
        Tracked::reset();
        int expected = 1;
        {
            auto gen = makeGenerator(3);
            for (auto value : gen) {
                REQUIRE_EQUAL(value.value, expected);
                ++expected;
            }
        }
        REQUIRE_EQUAL(expected, 4); // iterated 1..3
        REQUIRE_EQUAL(Tracked::constructions, Tracked::destructions);
    }

    void testMoveSemantics() {
        Tracked::reset();
        {
            auto gen1 = makeGenerator(1);
            auto gen2 = std::move(gen1);
            REQUIRE_FALSE(gen1.next().has_value());
            auto v = gen2.next();
            REQUIRE(v.has_value());
            REQUIRE_EQUAL(v->value, 1);
        }
        REQUIRE_EQUAL(Tracked::constructions, Tracked::destructions);
    }
};

