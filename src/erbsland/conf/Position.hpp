// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "impl/utilities/InternalView.hpp"


namespace erbsland::conf {


/// A position inside a document.
///
/// Starting with line 1 and column 1.
/// If the line or column are undefined, they are set to -1.
///
/// @tested PositionTest
///
class Position final {
public:
    /// Create a new position with the given line and column.
    ///
    /// @param line The line.
    /// @param column The column.
    ///
    constexpr Position(int line, int column) : _line(line), _column(column) {}

    /// Default constructor.
    Position() = default;
    /// Default destructor.
    ~Position() = default;
    /// Default copy constructor.
    Position(const Position &) = default;
    /// Default move constructor.
    Position(Position &&) = default;
    /// Default copy assignment.
    auto operator=(const Position &) -> Position& = default;
    /// Default move assignment.
    auto operator=(Position &&) -> Position& = default;

public: // comparison
    /// Compare this position to another for equality.
    ///
    /// @param other The other position to compare.
    /// @return `true` if both line and column are identical.
    ///
    auto operator==(const Position &other) const noexcept -> bool { return _line == other._line && _column == other._column; }

    /// Compare this position to another for inequality.
    ///
    /// @param other The other position to compare.
    /// @return `true` if positions differ, `false` otherwise.
    ///
    auto operator!=(const Position &other) const noexcept -> bool { return !operator==(other); }

public: // accessors
    /// The line, starting from 1
    ///
    [[nodiscard]] auto line() const noexcept -> int { return _line; }

    /// The column, starting from 1
    ///
    [[nodiscard]] auto column() const noexcept -> int { return _column; }

public: // tests
    /// Test if this position is undefined.
    ///
    [[nodiscard]] auto isUndefined() const noexcept -> bool { return _line <= 0; }

public: // mutators
    /// Increase the line and reset the column.
    ///
    void nextLine() noexcept { _line += 1; _column = 1; }

    /// Increase the column.
    ///
    void nextColumn() noexcept { _column += 1; }

public: // conversion
    /// Convert this position into a string.
    ///
    /// @return The string representation of this position.
    ///
    [[nodiscard]] auto toText() const noexcept -> String;

public: // testing
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const Position &object) -> impl::InternalViewPtr;
#endif

public:
    int _line{-1}; ///< The line.
    int _column{-1}; ///< The column.
};


}


template <>
struct std::formatter<erbsland::conf::Position> : std::formatter<std::string> {
    auto format(const erbsland::conf::Position position, format_context& ctx) const {
        return std::formatter<std::string>::format(position.toText().toCharString(), ctx);
    }
};
