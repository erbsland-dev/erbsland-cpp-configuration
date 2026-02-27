// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/utilities/TimeHelper.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace erbsland::conf::impl::time;


class TimeHelperTest final : public el::UnitTest {
public:
    // Test cases for isLeapYear
    TESTED_TARGETS(isLeapYear)
    void testIsLeapYear() {
        // Valid leap years
        REQUIRE(isLeapYear(2000) == true); // Divisible by 400
        REQUIRE(isLeapYear(1996) == true); // Divisible by 4 but not by 100
        REQUIRE(isLeapYear(2400) == true); // Divisible by 400

        // Valid non-leap years
        REQUIRE(isLeapYear(1900) == false); // Divisible by 100 but not by 400
        REQUIRE(isLeapYear(1999) == false); // Not divisible by 4
        REQUIRE(isLeapYear(2100) == false); // Divisible by 100 but not by 400

        // Special case, fictive year zero that is technically a leap year.
        REQUIRE(isLeapYear(0) == true);

        // Invalid years
        REQUIRE(isLeapYear(-1) == false);    // Below valid range
        REQUIRE(isLeapYear(-400) == false); // Negative year
        REQUIRE(isLeapYear(10000) == false); // Above valid range
    }

    // Test cases for daysInMonth
    TESTED_TARGETS(daysInMonth)
    void testDaysInMonth() {
        // Invalid years
        REQUIRE(daysInMonth(-1, 1) == 0);
        REQUIRE(daysInMonth(10000, 1) == 0);

        // Invalid months
        REQUIRE(daysInMonth(2024, 0) == 0);
        REQUIRE(daysInMonth(2024, 13) == 0);

        // February in leap years
        REQUIRE(daysInMonth(2024, 2) == 29);
        REQUIRE(daysInMonth(2000, 2) == 29);

        // February in non-leap years
        REQUIRE(daysInMonth(2023, 2) == 28);
        REQUIRE(daysInMonth(1900, 2) == 28);

        // Months with 31 days
        REQUIRE(daysInMonth(2024, 1) == 31);
        REQUIRE(daysInMonth(2024, 3) == 31);
        REQUIRE(daysInMonth(2024, 5) == 31);
        REQUIRE(daysInMonth(2024, 7) == 31);
        REQUIRE(daysInMonth(2024, 8) == 31);
        REQUIRE(daysInMonth(2024, 10) == 31);
        REQUIRE(daysInMonth(2024, 12) == 31);

        // Months with 30 days
        REQUIRE(daysInMonth(2024, 4) == 30);
        REQUIRE(daysInMonth(2024, 6) == 30);
        REQUIRE(daysInMonth(2024, 9) == 30);
        REQUIRE(daysInMonth(2024, 11) == 30);
    }

    // Test cases for daysSinceEpoch (year only)
    TESTED_TARGETS(daysSinceEpoch)
    void testDaysSinceEpoch_year() {
        // Invalid years
        REQUIRE(daysSinceEpoch(-9921) == 0);
        REQUIRE(daysSinceEpoch(-1) == 0);
        REQUIRE(daysSinceEpoch(10000) == 0);

        // Fictive year 0
        REQUIRE(daysSinceEpoch(0) == 0);
        REQUIRE(daysSinceEpoch(1) == 366);

        // Non-leap year
        REQUIRE(daysSinceEpoch(2) == 731);
        REQUIRE(daysSinceEpoch(3) == 1096);
        REQUIRE(daysSinceEpoch(4) == 1461);

        // Leap year
        REQUIRE(daysSinceEpoch(5) == 1827); // Year 4 is leap

        // Century and quadricentennial
        REQUIRE(daysSinceEpoch(100) == 36525); // 100 years
        REQUIRE(daysSinceEpoch(400) == 146097); // 400 years
        REQUIRE(daysSinceEpoch(9999) == 3652060); // Maximum valid year
    }

    // Test cases for firstDayOfYearAndMonth
    TESTED_TARGETS(firstDayOfYearAndMonth)
    void testFirstDayOfYearAndMonth() {
        // Invalid inputs
        REQUIRE(firstDayOfYearAndMonth(0, 1) == 0);
        REQUIRE(firstDayOfYearAndMonth(2024, 0) == 0);
        REQUIRE(firstDayOfYearAndMonth(2024, 13) == 0);
        REQUIRE(firstDayOfYearAndMonth(10000, 1) == 0);

        // January
        REQUIRE(firstDayOfYearAndMonth(2024, 1) == 0);

        // February in leap year
        REQUIRE(firstDayOfYearAndMonth(2024, 2) == 31);

        // February in non-leap year
        REQUIRE(firstDayOfYearAndMonth(2023, 2) == 31);

        // March in leap year
        REQUIRE(firstDayOfYearAndMonth(2024, 3) == 60);

        // March in non-leap year
        REQUIRE(firstDayOfYearAndMonth(2023, 3) == 59);

        // December
        REQUIRE(firstDayOfYearAndMonth(2024, 12) == 335);
    }

    // Test cases for daysSinceEpoch (year, month, day)
    TESTED_TARGETS(daysSinceEpoch)
    void testDaysSinceEpoch_year_month_day() {
        // Invalid inputs
        REQUIRE(daysSinceEpoch(0, 1, 1) == 0);
        REQUIRE(daysSinceEpoch(2024, 0, 1) == 0);
        REQUIRE(daysSinceEpoch(2024, 13, 1) == 0);
        REQUIRE(daysSinceEpoch(2024, 1, 0) == 0);
        REQUIRE(daysSinceEpoch(2024, 1, 32) == 0);
        REQUIRE(daysSinceEpoch(10000, 1, 1) == 0);

        // Valid dates
        REQUIRE(daysSinceEpoch(0, 1, 1) == 0);
        REQUIRE(daysSinceEpoch(0, 1, 2) == 1);
        REQUIRE(daysSinceEpoch(0, 2, 1) == 31);
        REQUIRE(daysSinceEpoch(0, 12, 31) == 365);
        REQUIRE(daysSinceEpoch(1, 1, 1) == 366);
        REQUIRE(daysSinceEpoch(1, 1, 2) == 367);
        REQUIRE(daysSinceEpoch(1, 2, 1) == 397);
        REQUIRE(daysSinceEpoch(1, 12, 31) == 730);

        // Leap year
        REQUIRE(daysSinceEpoch(2024, 2, 28) == 739309);
        REQUIRE(daysSinceEpoch(2024, 2, 29) == 739310);
        REQUIRE(daysSinceEpoch(2024, 3, 1) == 739311);
        REQUIRE(daysSinceEpoch(2024, 2, 29) == daysSinceEpoch(2024, 2, 28) + 1);
        REQUIRE(daysSinceEpoch(2024, 3, 1) == daysSinceEpoch(2024, 2, 29) + 1);

        // Non-leap year
        REQUIRE(daysSinceEpoch(2023, 2, 27) == 738943);
        REQUIRE(daysSinceEpoch(2023, 2, 28) == 738944);
        REQUIRE(daysSinceEpoch(2023, 3, 1) == 738945);
        REQUIRE(daysSinceEpoch(2023, 2, 28) == daysSinceEpoch(2023, 2, 27) + 1);
        REQUIRE(daysSinceEpoch(2023, 3, 1) == daysSinceEpoch(2023, 2, 28) + 1);

        // End of year
        REQUIRE(daysSinceEpoch(9999, 12, 31) == 3652424);
    }

    // Test cases for extractYearAndDays
    TESTED_TARGETS(extractYearAndDays)
    void testExtractYearAndDays() {
        // Invalid daysSinceEpoch
        REQUIRE(extractYearAndDays(-1) == std::make_pair(0, 0));

        // Beginning of epoch
        REQUIRE(extractYearAndDays(0) == std::make_pair(0, 0));
        REQUIRE(extractYearAndDays(1) == std::make_pair(0, 1));

        // Within first 400-year cycle
        REQUIRE(extractYearAndDays(daysSinceEpoch(400)) == std::make_pair(400, 0));

        // After 400-year cycles
        REQUIRE(extractYearAndDays(daysSinceEpoch(800)) == std::make_pair(800, 0));

        // Near the maximum valid year
        REQUIRE(extractYearAndDays(daysSinceEpoch(9999)) == std::make_pair(9999, 0));
        REQUIRE(extractYearAndDays(daysSinceEpoch(9999) + 364) == std::make_pair(9999, 364));
        REQUIRE(extractYearAndDays(daysSinceEpoch(9999) + 365) == std::make_pair(9999, 364)); // invalid, clamped high
        REQUIRE(extractYearAndDays(1'000'000'000'000'000) == std::make_pair(9999, 364)); // invalid, clamped high

        // Edge cases around leap years
        REQUIRE(extractYearAndDays(daysSinceEpoch(3)) == std::make_pair(3, 0));
        REQUIRE(extractYearAndDays(daysSinceEpoch(3) + 365) == std::make_pair(4, 0));
        REQUIRE(extractYearAndDays(daysSinceEpoch(4)) == std::make_pair(4, 0));
        REQUIRE(extractYearAndDays(daysSinceEpoch(4) + 366) == std::make_pair(5, 0));
        REQUIRE(extractYearAndDays(daysSinceEpoch(100)) == std::make_pair(100, 0));
        REQUIRE(extractYearAndDays(daysSinceEpoch(100) + 365) == std::make_pair(101, 0));

        // Large number within range
        REQUIRE(extractYearAndDays(daysSinceEpoch(2024, 3, 1)) == std::make_pair(2024, 60));
    }

    // Test cases for extractMonthAndDay
    TESTED_TARGETS(extractMonthAndDay)
    void testExtractMonthAndDay() {
        // Invalid years
        REQUIRE(extractMonthAndDay(-1, 100) == std::make_pair(0, 0));
        REQUIRE(extractMonthAndDay(10000, 100) == std::make_pair(0, 0));

        // Begin of epoch
        REQUIRE(extractMonthAndDay(0, 0) == std::make_pair(1, 1));

        // Invalid dayOfYear
        REQUIRE(extractMonthAndDay(2024, -1) == std::make_pair(0, 0));
        REQUIRE(extractMonthAndDay(2024, 366) == std::make_pair(0, 0)); // Leap year
        REQUIRE(extractMonthAndDay(2023, 365) == std::make_pair(0, 0)); // Non-leap year

        // Valid dates in non-leap year
        REQUIRE(extractMonthAndDay(2023, 0) == std::make_pair(1, 1));
        REQUIRE(extractMonthAndDay(2023, 1) == std::make_pair(1, 2));
        REQUIRE(extractMonthAndDay(2023, 30) == std::make_pair(1, 31));
        REQUIRE(extractMonthAndDay(2023, 31) == std::make_pair(2, 1));
        REQUIRE(extractMonthAndDay(2023, 58) == std::make_pair(2, 28));
        REQUIRE(extractMonthAndDay(2023, 59) == std::make_pair(3, 1));

        // Valid dates in leap year
        REQUIRE(extractMonthAndDay(2024, 0) == std::make_pair(1, 1));
        REQUIRE(extractMonthAndDay(2024, 1) == std::make_pair(1, 2));
        REQUIRE(extractMonthAndDay(2024, 30) == std::make_pair(1, 31));
        REQUIRE(extractMonthAndDay(2024, 31) == std::make_pair(2, 1));
        REQUIRE(extractMonthAndDay(2024, 59) == std::make_pair(2, 29));
        REQUIRE(extractMonthAndDay(2024, 60) == std::make_pair(3, 1));

        // End of year
        REQUIRE(extractMonthAndDay(2023, 364) == std::make_pair(12, 31));
        REQUIRE(extractMonthAndDay(2024, 365) == std::make_pair(12, 31));
    }
};
