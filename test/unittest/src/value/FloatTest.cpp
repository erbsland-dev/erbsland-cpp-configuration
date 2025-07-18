// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/Float.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <type_traits>
#include <limits>


using namespace el::conf;


TESTED_TARGETS(Float)
class FloatTest final : public el::UnitTest {
public:
    void testAlias() {
        // Float should be a double precision floating point type
        REQUIRE(std::is_same_v<Float, double>);
        REQUIRE(sizeof(Float) == sizeof(double));
        REQUIRE(std::numeric_limits<Float>::is_iec559);
    }
};


