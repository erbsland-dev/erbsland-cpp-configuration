// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ValueTestHelper.hpp"


TESTED_TARGETS(Document Value)
class ValueMatrixTest final : public UNITTEST_SUBCLASS(ValueTestHelper) {
public:
    TESTED_TARGETS(toValueList toValueMatrix)
    void testScalarConversions() {
        WITH_CONTEXT(setupTemplate2("123"));

        auto list = value->toValueList();
        REQUIRE_EQUAL(list.size(), 1U);
        REQUIRE_EQUAL(list[0]->asInteger(), 123);

        auto matrix = value->toValueMatrix();
        REQUIRE_EQUAL(matrix.rowCount(), 1U);
        REQUIRE_EQUAL(matrix.columnCount(), 1U);
        REQUIRE_EQUAL(matrix.actualColumnCount(0), 1U);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 0)->asInteger(), 123);

        auto docList = doc->toValueList();
        REQUIRE(docList.empty());
        auto docMatrix = doc->toValueMatrix();
        REQUIRE_EQUAL(docMatrix.rowCount(), 0U);
        REQUIRE_EQUAL(docMatrix.columnCount(), 0U);
    }

    TESTED_TARGETS(toValueMatrix)
    void testListConversions() {
        WITH_CONTEXT(setupTemplate1("1", "2", "3", "\"text\""));

        const auto listValue = doc->valueOrThrow(u8"main.value_list");
        auto listMatrix = listValue->toValueMatrix();
        REQUIRE_EQUAL(listMatrix.rowCount(), 3U);
        REQUIRE_EQUAL(listMatrix.columnCount(), 1U);
        REQUIRE_EQUAL(listMatrix.actualColumnCount(0), 1U);
        REQUIRE_EQUAL(listMatrix.actualColumnCount(1), 1U);
        REQUIRE_EQUAL(listMatrix.actualColumnCount(2), 1U);
        REQUIRE_EQUAL(listMatrix.valueOrThrow(0, 0)->asInteger(), 1);
        REQUIRE_EQUAL(listMatrix.valueOrThrow(1, 0)->asInteger(), 2);
        REQUIRE_EQUAL(listMatrix.valueOrThrow(2, 0)->asInteger(), 3);

        const auto matrixValue = doc->valueOrThrow(u8"main.value_matrix");
        auto matrix = matrixValue->toValueMatrix();
        REQUIRE_EQUAL(matrix.rowCount(), 3U);
        REQUIRE_EQUAL(matrix.columnCount(), 3U);
        REQUIRE_EQUAL(matrix.actualColumnCount(0), 3U);
        REQUIRE_EQUAL(matrix.actualColumnCount(1), 3U);
        REQUIRE_EQUAL(matrix.actualColumnCount(2), 3U);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 0)->asInteger(), 1);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 1)->asInteger(), 2);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 2)->asInteger(), 3);
        REQUIRE_EQUAL(matrix.valueOrThrow(1, 0)->asInteger(), 2);
        REQUIRE_EQUAL(matrix.valueOrThrow(1, 1)->asInteger(), 3);
        REQUIRE_EQUAL(matrix.valueOrThrow(1, 2)->asInteger(), 1);
        REQUIRE_EQUAL(matrix.valueOrThrow(2, 0)->asInteger(), 3);
        REQUIRE_EQUAL(matrix.valueOrThrow(2, 1)->asInteger(), 1);
        REQUIRE_EQUAL(matrix.valueOrThrow(2, 2)->asInteger(), 2);
    }

    TESTED_TARGETS(asMatrix)
    void testAsMatrixConversions() {
        WITH_CONTEXT(setupTemplate1("1", "2", "3", "\"text\""));

        const auto matrixValue = doc->valueOrThrow(u8"main.value_matrix");
        auto matrix = matrixValue->asMatrix<int>();
        REQUIRE_EQUAL(matrix.rowCount(), 3U);
        REQUIRE_EQUAL(matrix.columnCount(), 3U);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 0), 1);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 1), 2);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 2), 3);
        REQUIRE_EQUAL(matrix.valueOrThrow(1, 0), 2);
        REQUIRE_EQUAL(matrix.valueOrThrow(1, 1), 3);
        REQUIRE_EQUAL(matrix.valueOrThrow(1, 2), 1);
        REQUIRE_EQUAL(matrix.valueOrThrow(2, 0), 3);
        REQUIRE_EQUAL(matrix.valueOrThrow(2, 1), 1);
        REQUIRE_EQUAL(matrix.valueOrThrow(2, 2), 2);

        const auto invalidMatrixValue = doc->valueOrThrow(u8"main.nok_value_matrix");
        auto invalidMatrix = invalidMatrixValue->asMatrix<int>();
        REQUIRE_EQUAL(invalidMatrix.rowCount(), 0U);
        REQUIRE_EQUAL(invalidMatrix.columnCount(), 0U);
        try {
            (void)invalidMatrixValue->asMatrixOrThrow<int>();
            REQUIRE(false);
        } catch (const Error &error) {
            REQUIRE_EQUAL(error.category(), ErrorCategory::TypeMismatch);
        }
    }

    TESTED_TARGETS(getMatrix)
    void testGetMatrixConversions() {
        WITH_CONTEXT(setupTemplate1("1", "2", "3", "\"text\""));

        auto matrix = doc->getMatrix<int>(u8"main.value_matrix");
        REQUIRE_EQUAL(matrix.rowCount(), 3U);
        REQUIRE_EQUAL(matrix.columnCount(), 3U);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 0), 1);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 1), 2);
        REQUIRE_EQUAL(matrix.valueOrThrow(0, 2), 3);

        auto scalarMatrix = doc->getMatrix<int>(u8"main.value1");
        REQUIRE_EQUAL(scalarMatrix.rowCount(), 1U);
        REQUIRE_EQUAL(scalarMatrix.columnCount(), 1U);
        REQUIRE_EQUAL(scalarMatrix.valueOrThrow(0, 0), 1);

        auto invalidMatrix = doc->getMatrix<int>(u8"main.nok_value_matrix");
        REQUIRE_EQUAL(invalidMatrix.rowCount(), 0U);
        REQUIRE_EQUAL(invalidMatrix.columnCount(), 0U);
        try {
            (void)doc->getMatrixOrThrow<int>(u8"main.nok_value_matrix");
            REQUIRE(false);
        } catch (const Error &error) {
            REQUIRE_EQUAL(error.category(), ErrorCategory::TypeMismatch);
        }
    }
};