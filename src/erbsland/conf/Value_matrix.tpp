// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


namespace erbsland::conf {


template <typename T>
auto Value::asMatrixOrThrow() const -> Matrix<T> {
    constexpr auto expectedType = ValueType::from<T>();
    static_assert(
        expectedType.raw() != ValueType::Undefined, "no type support available for the specified template argument.");
    if (type() != ValueType::ValueList && !type().isScalar()) {
        throw Error(
            ErrorCategory::TypeMismatch,
            impl::u8format(u8"Expected a matrix of '{}' values, but got a value of type '{}'.", expectedType, type()),
            namePath(),
            location());
    }
    const auto valueList = toValueMatrix();
    const auto rowCount = valueList.rowCount();
    const auto columnCount = valueList.columnCount();
    Matrix<T> result{rowCount, columnCount};
    for (std::size_t row = 0; row < rowCount; ++row) {
        for (std::size_t column = 0; column < valueList.actualColumnCount(row); ++column) {
            const auto &value = valueList.valueOrThrow(row, column);
            if (!value) {
                throw Error(
                    ErrorCategory::TypeMismatch,
                    impl::u8format(
                        u8"Expected all values in the matrix to be of type '{}', but found an empty value.",
                        expectedType),
                    namePath(),
                    location());
            }
            if (value->type() != expectedType) {
                throw Error(
                    ErrorCategory::TypeMismatch,
                    impl::u8format(
                        u8"Expected all values in the matrix to be of type '{}', but found an element of type '{}'.",
                        expectedType,
                        value->type()),
                    value->namePath(),
                    value->location());
            }
            result.setValue(row, column, value->asType<T>());
        }
    }
    return result;
}


template <typename T>
auto Value::asMatrix() const noexcept -> Matrix<T> {
    try {
        return asMatrixOrThrow<T>();
    } catch (const Error &) {
        return {};
    }
}


template <typename T>
auto Value::getMatrixOrThrow(const NamePathLike &namePath) const -> Matrix<T> {
    auto valueAtPath = valueOrThrow(namePath);
    constexpr auto expectedType = ValueType::from<T>();
    static_assert(
        expectedType.raw() != ValueType::Undefined, "no type support available for the specified template argument.");
    return valueAtPath->asMatrixOrThrow<T>();
}


template <typename T>
auto Value::getMatrix(const NamePathLike &namePath) const noexcept -> Matrix<T> {
    try {
        return getMatrixOrThrow<T>(namePath);
    } catch (const Error &) {
        return {};
    }
}


}