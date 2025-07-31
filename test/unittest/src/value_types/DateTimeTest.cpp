// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "../TestHelper.hpp"

#include <erbsland/conf/Date.hpp>
#include <erbsland/conf/DateTime.hpp>
#include <erbsland/conf/Time.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>
#include <stdexcept>
#include <utility>


using namespace el::conf;


TESTED_TARGETS(DateTime Date Time TimeOffset)
class DateTimeTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultConstructor() {
        DateTime dt;
        REQUIRE(dt.isUndefined());
        REQUIRE(dt.date().isUndefined());
        REQUIRE(dt.time().isUndefined());
        REQUIRE(dt.toText().empty());
    }

    void testParameterizedConstructor() {
        Date date(2024, 4, 27);  // April 27, 2024
        Time time(15, 30, 45, 250'000'000, TimeOffset::utc()); // 15:30:45.250 UTC

        DateTime dt(date, time);

        // The DateTime should not be undefined
        REQUIRE_FALSE(dt.isUndefined());

        // Accessors should return the correct Date and Time
        REQUIRE_FALSE(dt.date().isUndefined());
        REQUIRE(dt.date().year() == 2024);
        REQUIRE(dt.date().month() == 4);
        REQUIRE(dt.date().day() == 27);

        REQUIRE_FALSE(dt.time().isUndefined());
        REQUIRE(dt.time().hour() == 15);
        REQUIRE(dt.time().minute() == 30);
        REQUIRE(dt.time().second() == 45);
        REQUIRE(dt.time().secondFraction() == std::chrono::nanoseconds(250'000'000));

        // toText should return the correct ISO format
        REQUIRE(dt.toText() == "2024-04-27 15:30:45.25z");
    }

    // Test the copy constructor
    void testCopyConstructor() {
        Date date(2023, 12, 31);  // December 31, 2023
        TimeOffset negOffset(-3600); // -01:00:00
        Time time(23, 59, 59, 999'999'999, negOffset); // 23:59:59.999999999-01:00:00

        DateTime original(date, time);
        DateTime copy = original;

        // The copied DateTime should be equal to the original
        REQUIRE_FALSE(copy.isUndefined());

        // Check Date
        REQUIRE_FALSE(copy.date().isUndefined());
        REQUIRE(copy.date().year() == 2023);
        REQUIRE(copy.date().month() == 12);
        REQUIRE(copy.date().day() == 31);

        // Check Time
        REQUIRE_FALSE(copy.time().isUndefined());
        REQUIRE(copy.time().hour() == 23);
        REQUIRE(copy.time().minute() == 59);
        REQUIRE(copy.time().second() == 59);
        REQUIRE(copy.time().secondFraction() == std::chrono::nanoseconds(999'999'999));

        // Check Offset
        REQUIRE(copy.time().offset().isNegative());
        REQUIRE(copy.time().offset().totalSeconds() == std::chrono::seconds{-3600});
        REQUIRE(copy.time().offset().hours() == 1);
        REQUIRE(copy.time().offset().minutes() == 0);
        REQUIRE(copy.time().offset().seconds() == 0);

        // Check toText
        REQUIRE(copy.toText() == "2023-12-31 23:59:59.999999999-01:00");
    }

    // Test the copy assignment operator
    void testCopyAssignment() {
        Date date1(2022, 1, 1);   // January 1, 2022
        TimeOffset posOffset(7200); // +02:00:00
        Time time1(0, 0, 0, 0, posOffset); // 00:00:00+02:00:00

        Date date2(2021, 6, 15);  // June 15, 2021
        TimeOffset utcOffset(0);  // UTC
        Time time2(12, 30, 30, 500'000'000, utcOffset); // 12:30:30.500 UTC

        DateTime original(date1, time1);
        DateTime copy;
        copy = original;

        // The copied DateTime should be equal to the original
        REQUIRE_FALSE(copy.isUndefined());

        // Check Date
        REQUIRE_FALSE(copy.date().isUndefined());
        REQUIRE(copy.date().year() == 2022);
        REQUIRE(copy.date().month() == 1);
        REQUIRE(copy.date().day() == 1);

        // Check Time
        REQUIRE_FALSE(copy.time().isUndefined());
        REQUIRE(copy.time().hour() == 0);
        REQUIRE(copy.time().minute() == 0);
        REQUIRE(copy.time().second() == 0);
        REQUIRE(copy.time().secondFraction() == std::chrono::nanoseconds(0));

        // Check Offset
        REQUIRE_FALSE(copy.time().offset().isNegative());
        REQUIRE(copy.time().offset().totalSeconds() == std::chrono::seconds{7200});
        REQUIRE(copy.time().offset().hours() == 2);
        REQUIRE(copy.time().offset().minutes() == 0);
        REQUIRE(copy.time().offset().seconds() == 0);

        // Check toText
        REQUIRE(copy.toText() == "2022-01-01 00:00:00+02:00");
    }

    // Test the move constructor
    void testMoveConstructor() {
        Date date(2025, 7, 20);  // July 20, 2025
        TimeOffset posOffset(3600); // +01:00:00
        Time time(8, 15, 30, 750'000'000, posOffset); // 08:15:30.750+01:00:00

        DateTime original(date, time);
        DateTime moved = std::move(original);

        // The moved DateTime should retain the original data
        REQUIRE_FALSE(moved.isUndefined());

        // Check Date
        REQUIRE_FALSE(moved.date().isUndefined());
        REQUIRE(moved.date().year() == 2025);
        REQUIRE(moved.date().month() == 7);
        REQUIRE(moved.date().day() == 20);

        // Check Time
        REQUIRE_FALSE(moved.time().isUndefined());
        REQUIRE(moved.time().hour() == 8);
        REQUIRE(moved.time().minute() == 15);
        REQUIRE(moved.time().second() == 30);
        REQUIRE(moved.time().secondFraction() == std::chrono::nanoseconds(750'000'000));

        // Check Offset
        REQUIRE_FALSE(moved.time().offset().isNegative());
        REQUIRE(moved.time().offset().totalSeconds() == std::chrono::seconds{3600});
        REQUIRE(moved.time().offset().hours() == 1);
        REQUIRE(moved.time().offset().minutes() == 0);
        REQUIRE(moved.time().offset().seconds() == 0);

        // Check toText
        REQUIRE(moved.toText() == "2025-07-20 08:15:30.75+01:00");

        // The state of 'original' after move is unspecified; no checks performed
    }

    // Test the move assignment operator
    void testMoveAssignment() {
        Date date1(2020, 2, 29);  // February 29, 2020 (leap day)
        TimeOffset utcOffset(0);  // UTC
        Time time1(23, 59, 59, 999'999'999, utcOffset); // 23:59:59.999999999z

        Date date2(2019, 11, 11); // November 11, 2019
        TimeOffset negOffset(-5400); // -01:30:00
        Time time2(11, 11, 11, 111'111'111, negOffset); // 11:11:11.111111111-01:30:00

        DateTime original(date1, time1);
        DateTime moved;
        moved = std::move(original);

        // The moved DateTime should retain the original data
        REQUIRE_FALSE(moved.isUndefined());

        // Check Date
        REQUIRE_FALSE(moved.date().isUndefined());
        REQUIRE(moved.date().year() == 2020);
        REQUIRE(moved.date().month() == 2);
        REQUIRE(moved.date().day() == 29);

        // Check Time
        REQUIRE_FALSE(moved.time().isUndefined());
        REQUIRE(moved.time().hour() == 23);
        REQUIRE(moved.time().minute() == 59);
        REQUIRE(moved.time().second() == 59);
        REQUIRE(moved.time().secondFraction() == std::chrono::nanoseconds(999'999'999));

        // Check Offset
        REQUIRE_FALSE(moved.time().offset().isNegative());
        REQUIRE(moved.time().offset().isUTC());
        REQUIRE(moved.time().offset().totalSeconds() == std::chrono::seconds{0});
        REQUIRE(moved.time().offset().hours() == 0);
        REQUIRE(moved.time().offset().minutes() == 0);
        REQUIRE(moved.time().offset().seconds() == 0);

        // Check toText
        REQUIRE(moved.toText() == "2020-02-29 23:59:59.999999999z");

        // The state of 'original' after move is unspecified; no checks performed
    }

    // Test the isUndefined accessor
    void testIsUndefined() {
        DateTime undefinedDT;
        Date date(2021, 5, 17);
        TimeOffset utcOffset(0);
        Time time(10, 20, 30, 400'000'000, utcOffset);
        DateTime definedDT(date, time);

        // Only the default-constructed DateTime should be undefined
        REQUIRE(undefinedDT.isUndefined());
        REQUIRE_FALSE(definedDT.isUndefined());
    }

    // Test the date accessor
    void testDateAccessor() {
        Date date(2022, 8, 15);  // August 15, 2022
        TimeOffset posOffset(1800); // +00:30:00
        Time time(14, 45, 0, 0, posOffset); // 14:45:00.000+00:30:00
        DateTime dt(date, time);

        // Check Date accessor
        const Date& accessedDate = dt.date();
        REQUIRE_FALSE(accessedDate.isUndefined());
        REQUIRE(accessedDate.year() == 2022);
        REQUIRE(accessedDate.month() == 8);
        REQUIRE(accessedDate.day() == 15);
    }

    // Test the time accessor
    void testTimeAccessor() {
        Date date(2023, 3, 10);  // March 10, 2023
        TimeOffset negOffset(-2700); // -00:45:00
        Time time(6, 30, 15, 600'000'000, negOffset); // 06:30:15.600-00:45:00
        DateTime dt(date, time);

        // Check Time accessor
        const Time& accessedTime = dt.time();
        REQUIRE_FALSE(accessedTime.isUndefined());
        REQUIRE(accessedTime.hour() == 6);
        REQUIRE(accessedTime.minute() == 30);
        REQUIRE(accessedTime.second() == 15);
        REQUIRE(accessedTime.secondFraction() == std::chrono::nanoseconds(600'000'000));

        // Check Offset
        REQUIRE(accessedTime.offset().isNegative());
        REQUIRE(accessedTime.offset().totalSeconds() == std::chrono::seconds{-2700});
        REQUIRE(accessedTime.offset().hours() == 0);
        REQUIRE(accessedTime.offset().minutes() == 45);
        REQUIRE(accessedTime.offset().seconds() == 0);
    }

    // Test the toText method for various scenarios
    void testToText() {
        // Undefined DateTime
        DateTime undefinedDT;
        REQUIRE(undefinedDT.toText().empty());

        // UTC DateTime
        Date dateUTC(2024, 1, 1);  // January 1, 2024
        TimeOffset utcOffset(0);    // UTC
        Time timeUTC(0, 0, 0, 0, utcOffset); // 00:00:00.000z
        DateTime dtUTC(dateUTC, timeUTC);
        REQUIRE(dtUTC.toText() == "2024-01-01 00:00:00z");

        // Local Time DateTime
        Date dateLocal(2023, 12, 25); // December 25, 2023
        TimeOffset localOffset;        // Default constructor represents local time
        Time timeLocal(12, 0, 0, 0, localOffset); // 12:00:00.000
        DateTime dtLocal(dateLocal, timeLocal);
        REQUIRE(dtLocal.toText() == "2023-12-25 12:00:00");

        // Positive Offset DateTime with fractional seconds
        Date datePos(2022, 7, 4);  // July 4, 2022
        TimeOffset posOffset(5400); // +01:30:00
        Time timePos(18, 45, 30, 123'000'000, posOffset); // 18:45:30.123+01:30
        DateTime dtPos(datePos, timePos);
        REQUIRE(dtPos.toText() == "2022-07-04 18:45:30.123+01:30");

        // Negative Offset DateTime with fractional seconds
        Date dateNeg(2021, 11, 11);  // November 11, 2021
        TimeOffset negOffset(-3600); // -01:00:00
        Time timeNeg(23, 59, 59, 999'999'999, negOffset); // 23:59:59.999999999-01:00
        DateTime dtNeg(dateNeg, timeNeg);
        REQUIRE(dtNeg.toText() == "2021-11-11 23:59:59.999999999-01:00");
    }

    // Test edge cases and invalid inputs
    void testEdgeCasesAndInvalidInputs() {
        // Leap Day
        Date leapDate(2020, 2, 29);  // February 29, 2020
        TimeOffset utcOffset(0);      // UTC
        Time leapTime(12, 0, 0, 0, utcOffset); // 12:00:00.000z
        DateTime leapDT(leapDate, leapTime);
        REQUIRE(leapDT.toText() == "2020-02-29 12:00:00z");

        // Start of Epoch
        Date epochDate(1970, 1, 1);  // January 1, 1970
        Time epochTime(0, 0, 0, 0, utcOffset); // 00:00:00.000z
        DateTime epochDT(epochDate, epochTime);
        REQUIRE(epochDT.toText() == "1970-01-01 00:00:00z");

        // Maximum Valid Date
        Date maxDate(9999, 12, 31);  // December 31, 9999
        Time maxTime(23, 59, 59, 999'999'999, utcOffset); // 23:59:59.999999999z
        DateTime maxDT(maxDate, maxTime);
        REQUIRE(maxDT.toText() == "9999-12-31 23:59:59.999999999z");

        // Minimum Valid Date
        Date minDate(0001, 1, 1);  // January 1, 0001
        Time minTime(0, 0, 0, 0, utcOffset); // 00:00:00.000z
        DateTime minDT(minDate, minTime);
        REQUIRE(minDT.toText() == "0001-01-01 00:00:00z");
    }

    void testOperators() {
        WITH_CONTEXT(requireAllOperators<DateTime, DateTime>(
            DateTime{Date{2022, 8, 15}, Time{14, 45, 0, 0, TimeOffset::utc()}},
            DateTime{Date{2023, 9, 1}, Time{23, 17, 1, 23, TimeOffset::utc()}},
            DateTime{Date{2024, 1, 17}, Time{1, 22, 39, 0, TimeOffset::utc()}},
            DateTime{Date{2022, 8, 15}, Time{14, 45, 0, 0, TimeOffset::utc()}},
            DateTime{Date{2023, 9, 1}, Time{23, 17, 1, 23, TimeOffset::utc()}},
            DateTime{Date{2024, 1, 17}, Time{1, 22, 39, 0, TimeOffset::utc()}}
        ));
        WITH_CONTEXT(requireStrictOrder(std::array<DateTime, 9>{
            DateTime{},
            DateTime{Date{1, 1, 1}, Time{0, 0, 0, 0, TimeOffset::utc()}},
            DateTime{Date{1500, 1, 1}, Time{1, 0, 0, 0, TimeOffset{-7200}}},
            DateTime{Date{1500, 1, 1}, Time{0, 0, 0, 0, TimeOffset::utc()}},
            DateTime{Date{1499, 12, 31}, Time{23, 0, 0, 0, TimeOffset{7200}}},
            DateTime{Date{2022, 8, 15}, Time{14, 45, 0, 0, TimeOffset::utc()}},
            DateTime{Date{2023, 9, 1}, Time{23, 17, 1, 23, TimeOffset::utc()}},
            DateTime{Date{2024, 1, 17}, Time{1, 22, 39, 0, TimeOffset::utc()}},
            DateTime{Date{9999, 12, 31}, Time{23, 59, 59, 999'999'999, TimeOffset::utc()}}
        }))
    }
};

