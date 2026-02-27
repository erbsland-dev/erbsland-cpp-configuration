// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Position.hpp"
#include "SourceIdentifier.hpp"
#include "String.hpp"

#include "impl/utilities/InternalView.hpp"


namespace erbsland::conf {


/// Represents the location in a parsed document.
///
/// @tested `LocationTest`
///
class Location {
public:
    /// Creates an undefined location.
    ///
    /// Can be tested with `isUndefined()`.
    ///
    Location() = default;

    /// Create a new location object.
    ///
    /// @param sourceIdentifier The source identifier.
    /// @param position The position in the document.
    ///
    explicit Location(SourceIdentifierPtr sourceIdentifier, const Position position = {}) noexcept
        : _sourceIdentifier{std::move(sourceIdentifier)}, _position{position} {
    }

    /// Default copy constructor.
    Location(const Location&) = default;
    /// Default move constructor.
    Location(Location&&) = default;
    /// Default copy assignment.
    auto operator=(const Location&) -> Location& = default;
    /// Default move assignment.
    auto operator=(Location&&) -> Location& = default;

public: // operators
    /// Compare this location to another for equality.
    ///
    /// @param other The location to compare.
    /// @return `true` if both the source identifier and position are equal.
    ///
    auto operator==(const Location &other) const noexcept -> bool {
        return SourceIdentifier::areEqual(_sourceIdentifier, other._sourceIdentifier) && _position == other._position;
    }

    /// Compare this location to another for inequality.
    ///
    /// @param other The location to compare.
    /// @return `true` if the locations are not equal, `false` otherwise.
    ///
    auto operator!=(const Location &other) const noexcept -> bool {
        return !operator==(other);
    }

public: // accessors
    /// Test if this location is undefined.
    ///
    /// @return `true` if undefined, `false` otherwise.
    ///
    [[nodiscard]] auto isUndefined() const noexcept -> bool {
        return _sourceIdentifier == nullptr && _position.isUndefined();
    }

    /// The source identifier for this location.
    ///
    [[nodiscard]] auto sourceIdentifier() const noexcept -> const SourceIdentifierPtr& {
        return _sourceIdentifier;
    }

    /// Get the position.
    ///
    [[nodiscard]] constexpr auto position() const noexcept -> Position { return _position; }

public: // conversion
    /// Get this location as a text.
    ///
    /// The location is formatted as: (source identifier):(line):(column).
    /// If no source identifier is specified, it is replaced by the text `<unknown>`.
    ///
    /// @return A string with this location information.
    ///
    [[nodiscard]] auto toText() const -> String;

public: // testing
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const Location &object) -> impl::InternalViewPtr ;
#endif

private:
    SourceIdentifierPtr _sourceIdentifier; ///< The source identifier.
    Position _position; ///< The position.
};


}


template <>
struct std::formatter<erbsland::conf::Location> : std::formatter<std::string> {
    auto format(const erbsland::conf::Location &location, format_context& ctx) const {
        return std::formatter<std::string>::format(location.toText().toCharString(), ctx);
    }
};
