// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>
#include <utility>


/// Helper functions for date/time calculation.
///
/// @note These functions accept the fictive year zero in order to simplify testing and calculations.
/// @tested `TimeHelperTest`
///
namespace erbsland::conf::impl::time {


/// Test if the given year is a leap year.
///
/// @param year The year from 0000-9999.
///
auto isLeapYear(int year) noexcept -> bool;

/// Get the number of days for a month.
///
/// @param year The year from 0000-9999.
/// @param month The month from 1-12
///
auto daysInMonth(int year, int month) noexcept -> int;

/// Get the number of days since epoch for the given year.
///
/// @param year The year from 0000-9999.
///
auto daysSinceEpoch(int year) noexcept -> int64_t;

/// Get the first day of the year and month (zero based)
///
/// @param year The year from 0000-9999.
/// @param month The month from 1-12
///
auto firstDayOfYearAndMonth(int year, int month) noexcept -> int;

/// Get the number of days since epoch for the given year, month and day
///
/// @param year Year 0000-9999
/// @param month Month 1-12
/// @param day Day 1-31
///
auto daysSinceEpoch(int year, int month, int day) noexcept -> int64_t;

/// Extract the year and days from the days from epoch
///
/// @param daysSinceEpoch The number of days since epoch (0000-01-01).
/// @return A tuple with the year, and the day in the year (starting from zero).
///
auto extractYearAndDays(int64_t daysSinceEpoch) noexcept -> std::pair<int, int>;

/// Extract the month and day from a year and day of year.
///
/// @param year The year (0000-9999).
/// @param dayOfYear The day of the year, starting from zero for the first day.
/// @return A tuple with the month, and the day in this month.
///
auto extractMonthAndDay(int year, int dayOfYear) noexcept -> std::pair<int, int>;


}

