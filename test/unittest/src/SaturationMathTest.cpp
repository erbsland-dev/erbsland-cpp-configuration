// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/SaturationMath.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <cstdint>
#include <limits>
#include <type_traits>


using namespace erbsland::conf;
using impl::willAddOverflow;
using impl::willMultiplyOverflow;


TESTED_TARGETS(willAddOverflow willMultiplyOverflow)
class SaturationMathTest final : public el::UnitTest {
public:
    template<typename T>
    void verifyOverflow() {
        REQUIRE_FALSE(willAddOverflow<T>(T{1}, T{2}));
        REQUIRE(willAddOverflow<T>(std::numeric_limits<T>::max(), T{1}));
        if constexpr (std::is_signed_v<T>) {
            REQUIRE(willAddOverflow<T>(std::numeric_limits<T>::min(), T{-1}));
            REQUIRE_FALSE(willAddOverflow<T>(std::numeric_limits<T>::min(), T{0}));
        }

        REQUIRE_FALSE(willMultiplyOverflow<T>(T{2}, T{10}));
        REQUIRE(willMultiplyOverflow<T>(std::numeric_limits<T>::max(), T{2}));
        if constexpr (std::is_signed_v<T>) {
            REQUIRE(willMultiplyOverflow<T>(std::numeric_limits<T>::min(), T{2}));
        }
    }

    void testOverflow32bit() {
        WITH_CONTEXT(verifyOverflow<std::int32_t>());
        WITH_CONTEXT(verifyOverflow<std::uint32_t>());
    }

    void testOverflow64bit() {
        WITH_CONTEXT(verifyOverflow<std::int64_t>());
        WITH_CONTEXT(verifyOverflow<std::uint64_t>());
    }
};

