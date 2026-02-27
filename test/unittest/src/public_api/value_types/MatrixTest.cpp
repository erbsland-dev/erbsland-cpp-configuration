// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/Matrix.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>


using namespace el::conf;


TESTED_TARGETS(Matrix)
class MatrixTest final : public el::UnitTest {
public:
    void testDefault() {
        const Matrix<int> matrix{};
        REQUIRE(matrix.rowCount() == 0);
        REQUIRE(matrix.columnCount() == 0);
    }

    void testConstruction() {
        Matrix<int> matrix{3, 3};
        matrix.setRow(0, {1, 2, 3});
        matrix.setRow(1, {4});
        REQUIRE(matrix.rowCount() == 3);
        REQUIRE(matrix.columnCount() == 3);
        REQUIRE(matrix.actualColumnCount(0) == 3);
        REQUIRE(matrix.actualColumnCount(1) == 1);
        REQUIRE(matrix.actualColumnCount(2) == 0);
    }

    void testAccessDefinedAndDefault() {
        Matrix<int> matrix{3, 3};
        matrix.setRow(0, {1, 2, 3});
        matrix.setValue(1, 0, 4);
        REQUIRE(matrix.valueOrThrow(0, 1) == 2);
        REQUIRE(matrix.valueOrThrow(1, 0) == 4);
        REQUIRE(matrix.value(1, 2, 0) == 0);
        REQUIRE(matrix.isDefined(0, 2));
        REQUIRE_FALSE(matrix.isDefined(1, 1));
    }

    void testBoundsChecks() {
        Matrix<int> matrix{2, 2};
        REQUIRE(matrix.actualColumnCount(5) == 0);
        REQUIRE_FALSE(matrix.isDefined(3, 1));
        REQUIRE(matrix.value(0, 5, 0) == 0);
        REQUIRE_THROWS_AS(std::out_of_range, matrix.valueOrThrow(0, 5));
        REQUIRE_THROWS_AS(std::out_of_range, matrix.setValue(0, 5, 3));
        REQUIRE_THROWS_AS(std::out_of_range, matrix.setRow(5, {1}));
        REQUIRE_THROWS_AS(std::out_of_range, matrix.setRow(0, {1, 2, 3}));
    }
};
