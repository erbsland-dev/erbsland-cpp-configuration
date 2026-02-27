// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <concepts>
#include <cstddef>
#include <stdexcept>
#include <vector>


namespace erbsland::conf {


/// A 2D matrix with a per-row column counts.
/// @tested `MatrixTest`
template<typename T>
requires std::default_initializable<T>
class Matrix {
public:
    /// Create an empty matrix.
    Matrix() = default;

    /// Create a matrix with a given size.
    /// @param rowCount The number of rows.
    /// @param columnCount The number of columns.
    Matrix(const std::size_t rowCount, const std::size_t columnCount)
        : _rowCount{rowCount},
          _columnCount{columnCount},
          _actualColumnCounts(rowCount, 0),
          _values(rowCount * columnCount) {
    }

public: // access
    /// Get the number of rows in this matrix.
    [[nodiscard]] auto rowCount() const noexcept -> std::size_t {
        return _rowCount;
    }

    /// Get the number of columns in this matrix.
    [[nodiscard]] auto columnCount() const noexcept -> std::size_t {
        return _columnCount;
    }

    /// Get the actual column count for the given row.
    /// @param row The row index.
    /// @return The number of columns defined in the row.
    [[nodiscard]] auto actualColumnCount(const std::size_t row) const noexcept -> std::size_t {
        if (row >= _rowCount) {
            return 0;
        }
        return _actualColumnCounts[row];
    }

    /// Test if a value was defined in the original nested list.
    /// @param row The row index.
    /// @param column The column index.
    /// @return `true` if the value was defined.
    [[nodiscard]] auto isDefined(const std::size_t row, const std::size_t column) const noexcept -> bool {
        if (row >= _rowCount || column >= _columnCount) {
            return false;
        }
        return column < _actualColumnCounts[row];
    }

    /// Access a value by row and column.
    /// @param row The row index.
    /// @param column The column index.
    /// @param defaultValue The default value for missing cells.
    /// @return The value or the default value if it was not defined.
    [[nodiscard]] auto value(
        const std::size_t row,
        const std::size_t column,
        const T &defaultValue = {}) const noexcept -> const T& {

        if (!isDefined(row, column)) {
            return defaultValue;
        }
        return _values[toIndex(row, column)];
    }

    /// Access a value by row and column and throw on bounds errors.
    /// @param row The row index.
    /// @param column The column index.
    /// @return The value or a default value if it was not defined.
    /// @throws std::out_of_range if the row or column is outside the matrix.
    [[nodiscard]] auto valueOrThrow(const std::size_t row, const std::size_t column) const -> const T& {
        requireIndices(row, column);
        return _values[toIndex(row, column)];
    }

    /// Set a value.
    /// @param row The row index.
    /// @param column The column index.
    /// @param value The value to set.
    /// @throws std::out_of_range if the row or column is outside the matrix.
    void setValue(const std::size_t row, const std::size_t column, const T &value) {
        requireIndices(row, column);
        _values[toIndex(row, column)] = value;
        updateActualColumnCount(row, column + 1);
    }

    /// Set values for a complete row.
    /// @param row The row index.
    /// @param values The values to set.
    /// @throws std::out_of_range if the row is outside the matrix.
    void setRow(const std::size_t row, const std::vector<T> &values) {
        requireRowIndex(row);
        if (values.size() > _columnCount) {
            throw std::out_of_range("Matrix column index out of range");
        }
        for (std::size_t column = 0; column < values.size(); ++column) {
            _values[toIndex(row, column)] = values[column];
        }
        updateActualColumnCount(row, values.size());
    }

private:
    [[nodiscard]] auto toIndex(const std::size_t row, const std::size_t column) const noexcept -> std::size_t {
        return (row * _columnCount) + column;
    }

    void requireRowIndex(const std::size_t row) const {
        if (row >= _rowCount) {
            throw std::out_of_range("Matrix row index out of range");
        }
    }

    void requireIndices(const std::size_t row, const std::size_t column) const {
        requireRowIndex(row);
        if (column >= _columnCount) {
            throw std::out_of_range("Matrix column index out of range");
        }
    }

    void updateActualColumnCount(const std::size_t row, const std::size_t columnCount) {
        if (columnCount > _actualColumnCounts[row]) {
            _actualColumnCounts[row] = columnCount;
        }
    }

private:
    std::size_t _rowCount{0};
    std::size_t _columnCount{0};
    std::vector<std::size_t> _actualColumnCounts;
    std::vector<T> _values;
};


}
