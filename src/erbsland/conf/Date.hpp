// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Error.hpp"

#include "impl/utilities/ComparisonHelper.hpp"

#include <chrono>
#include <cstdint>


namespace erbsland::conf {


/// A date value with year, month and day parts.
///
/// @tested `DateTest`
///
class Date final {
public:
    /// Create an undefined date.
    ///
    /// Default constructor.
    Date() = default;

    /// Creates a new date.
    ///
    /// @param year The year in the range 1-9999.
    /// @param month The month in the range 1-12.
    /// @param day The day in the range 1-31.
    /// @throws std::invalid_argument if any of the parameters is out of the valid range or the date is not valid.
    ///
    Date(int year, int month, int day);

    // defaults
    /// Default destructor.
    ~Date() = default;
    /// Default copy constructor.
    Date(const Date&) = default;
    /// Default copy assignment.
    auto operator=(const Date&) -> Date& = default;

public: // operators
    ERBSLAND_CONF_COMPARE_MEMBER(const Date& other, _daysSinceEpoch, other._daysSinceEpoch);

public: // accessors
    /// Test if this is an undefined date.
    ///
    /// @return `true` if the date is undefined.
    ///
    [[nodiscard]] auto isUndefined() const noexcept -> bool { return _daysSinceEpoch <= 0; }

    /// The year of the date.
    ///
    /// @return The year in the range 1-9999, or, zero for undefined dates.
    ///
    [[nodiscard]] auto year() const noexcept -> int;

    /// The month of the date.
    ///
    /// @return The month in the range 1-12, or, zero for undefined dates.
    ///
    [[nodiscard]] auto month() const noexcept -> int;

    /// The day of the date.
    ///
    /// @return The day in the range 1-31, or, zero for undefined dates.
    ///
    [[nodiscard]] auto day() const noexcept -> int;

public: // conversion
    /// Convert this date into days since the epoch.
    ///
    /// Epoch is the fictive date 0000-01-01.
    ///
    /// @return The days since the fictive (invalid) date 0000-01-01. Returns zero for undefined dates.
    ///
    [[nodiscard]] auto toDaysSinceEpoch() const noexcept -> int64_t { return _daysSinceEpoch; }

    /// Convert this date into text.
    ///
    /// Uses the ISO format yyyy-mm-dd.
    ///
    /// @return The formatted date or an empty string for undefined dates.
    ///
    [[nodiscard]] auto toText() const noexcept -> String;

public:
    /// Test if the given values are valid.
    ///
    /// @param year The year in the range 1-9999.
    /// @param month The month in the range 1-12.
    /// @param day The day in the range 1-31.
    /// @return `true` if the values describe a valid date, otherwise `false`.
    ///
    static auto isValid(int year, int month, int day) noexcept -> bool;

private:
    int64_t _daysSinceEpoch{0};
};


}


template <>
struct std::formatter<erbsland::conf::Date> : std::formatter<std::string> {
    auto format(const erbsland::conf::Date date, format_context& ctx) const {
        return std::formatter<std::string>::format(date.toText().toCharString(), ctx);
    }
};
