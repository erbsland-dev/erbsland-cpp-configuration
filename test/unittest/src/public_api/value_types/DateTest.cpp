// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/Date.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>


using namespace el::conf;


TESTED_TARGETS(Date)
class DateTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testValidDate() {
        Date const d{2024, 7, 20};
        REQUIRE(!d.isUndefined());
        REQUIRE(d.day() == 20);
        REQUIRE(d.month() == 7);
        REQUIRE(d.year() == 2024);
    }

    void testUndefinedDate() {
        Date const d;
        REQUIRE(d.isUndefined());
        REQUIRE(d.year() == 0);
        REQUIRE(d.month() == 0);
        REQUIRE(d.day() == 0);
        REQUIRE(d.toText() == u8"");
    }

    void testInvalidDates() {
        REQUIRE_THROWS_AS(std::invalid_argument, Date{0, 1, 1});    // Year out of range
        REQUIRE_THROWS_AS(std::invalid_argument, Date{2024, 0, 1});  // Month out of range
        REQUIRE_THROWS_AS(std::invalid_argument, Date{2024, 1, 0});  // Day out of range
        REQUIRE_THROWS_AS(std::invalid_argument, Date{2024, 2, 30}); // Invalid day in February
        REQUIRE_THROWS_AS(std::invalid_argument, Date{2024, 13, 1}); // Invalid month
        REQUIRE_THROWS_AS(std::invalid_argument, Date{9999, 12, 32}); // Invalid day
    }

    void testNegativeDates() {
        REQUIRE_THROWS_AS(std::invalid_argument, Date{-2024, 1, 1}); // Negative year
        REQUIRE_THROWS_AS(std::invalid_argument, Date{2024, -1, 1}); // Negative month
        REQUIRE_THROWS_AS(std::invalid_argument, Date{2024, 1, -1}); // Negative day
    }

    void testIsValid() {
        REQUIRE(Date::isValid(2024, 7, 20));     // Valid date
        REQUIRE(Date::isValid(2024, 2, 29));     // Valid leap day
        REQUIRE(!Date::isValid(2023, 2, 29));    // Invalid leap day
        REQUIRE(!Date::isValid(2024, 0, 20));    // Invalid month
        REQUIRE(!Date::isValid(2024, 7, 32));    // Invalid day
        REQUIRE(!Date::isValid(0, 7, 20));       // Invalid year
    }

    void testLeapYearEdgeCases() {
        Date leapYearDate{2020, 2, 29};
        REQUIRE_FALSE(leapYearDate.isUndefined());
        REQUIRE(leapYearDate.toText() == u8"2020-02-29");

        Date nonLeapYearDate{2021, 2, 28};
        REQUIRE_FALSE(nonLeapYearDate.isUndefined());
        REQUIRE(nonLeapYearDate.toText() == u8"2021-02-28");

        Date centuryNonLeapYearDate{1900, 2, 28};
        REQUIRE_FALSE(centuryNonLeapYearDate.isUndefined());
        REQUIRE(centuryNonLeapYearDate.toText() == u8"1900-02-28");

        Date leapCenturyYearDate{2000, 2, 29};
        REQUIRE_FALSE(leapCenturyYearDate.isUndefined());
        REQUIRE(leapCenturyYearDate.toText() == u8"2000-02-29");
    }

    void testToText() {
        Date const d1{2024, 7, 20};
        REQUIRE(d1.toText() == u8"2024-07-20");

        Date const d2;
        REQUIRE(d2.toText() == u8"");  // Undefined date should return empty string
    }

    void testCopyAndAssignment() {
        Date const d1{2024, 7, 20};
        Date const d2 = d1;  // Copy constructor
        REQUIRE(d1.year() == d2.year());
        REQUIRE(d1.month() == d2.month());
        REQUIRE(d1.day() == d2.day());

        Date d3;
        d3 = d1;  // Assignment operator
        REQUIRE(d3.year() == d1.year());
        REQUIRE(d3.month() == d1.month());
        REQUIRE(d3.day() == d1.day());
    }

    void testComparison() {
        WITH_CONTEXT(requireAllOperators<Date, Date>(
            Date{2024, 7, 20}, Date{2024, 7, 21}, Date{2024, 7, 22},
            Date{2024, 7, 20}, Date{2024, 7, 21}, Date{2024, 7, 22}
        ));
        WITH_CONTEXT(requireStrictOrder(std::array<Date, 14>({
            Date{},
            Date{1, 1, 1},
            Date{1, 1, 2},
            Date{1, 2, 1},
            Date{1, 3, 1},
            Date{2, 1, 1},
            Date{3, 1, 1},
            Date{100, 2, 27},
            Date{1030, 11, 12},
            Date{1900, 2, 28},
            Date{2024, 11, 12},
            Date{4219, 7, 21},
            Date{8920, 6, 14},
            Date{9999, 12, 31}
        })));
    }
};

