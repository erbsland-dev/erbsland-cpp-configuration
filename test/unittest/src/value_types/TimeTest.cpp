// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "../TestHelper.hpp"

#include <erbsland/conf/Time.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>
#include <stdexcept>
#include <utility>


using namespace el::conf;


TESTED_TARGETS(Time)
class TimeTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultConstructor() {
        Time time;

        // The time should be undefined
        REQUIRE(time.isUndefined());

        // Accessors should return default or zero values
        REQUIRE(time.hour() == 0);
        REQUIRE(time.minute() == 0);
        REQUIRE(time.second() == 0);
        REQUIRE(time.secondFraction() == std::chrono::nanoseconds(0));
        REQUIRE(time.toNanoseconds() == std::chrono::nanoseconds(0));

        // The offset should be local time by default
        REQUIRE(time.offset().isLocalTime());
        REQUIRE_FALSE(time.offset().isUTC());
        REQUIRE(time.offset().totalSeconds() == std::chrono::seconds{0});
        REQUIRE_FALSE(time.offset().isNegative());
        REQUIRE(time.offset().hours() == 0);
        REQUIRE(time.offset().minutes() == 0);
        REQUIRE(time.offset().seconds() == 0);

        // toText should return an empty string for undefined time
        REQUIRE(time.toText().empty());
    }

    void testConstructorWithHMSUTC() {
        TimeOffset utcOffset(0); // UTC
        Time time(12, 30, 45, 500'000'000, utcOffset);

        // The time should not be undefined
        REQUIRE_FALSE(time.isUndefined());

        // Accessors should return the correct values
        REQUIRE(time.hour() == 12);
        REQUIRE(time.minute() == 30);
        REQUIRE(time.second() == 45);
        REQUIRE(time.secondFraction() == std::chrono::nanoseconds(500'000'000));

        // Total nanoseconds since midnight
        int64_t expectedNanoseconds = 12 * 3600 * 1'000'000'000LL
                                     + 30 * 60 * 1'000'000'000LL
                                     + 45 * 1'000'000'000LL
                                     + 500'000'000LL;
        REQUIRE(time.toNanoseconds().count() == expectedNanoseconds);

        // Offset should be UTC
        REQUIRE(time.offset().isUTC());
        REQUIRE_FALSE(time.offset().isLocalTime());
        REQUIRE(time.offset().totalSeconds() == std::chrono::seconds{0});
        REQUIRE_FALSE(time.offset().isNegative());
        REQUIRE(time.offset().hours() == 0);
        REQUIRE(time.offset().minutes() == 0);
        REQUIRE(time.offset().seconds() == 0);

        // toText should return the correct ISO format with 'z'
        REQUIRE(time.toText() == "12:30:45.5z");
    }

    void testConstructorWithHMSPositiveOffset() {
        TimeOffset positiveOffset(3600); // +01:00:00
        Time time(23, 59, 59, 999'999'999, positiveOffset);

        // The time should not be undefined
        REQUIRE_FALSE(time.isUndefined());

        // Accessors should return the correct values
        REQUIRE(time.hour() == 23);
        REQUIRE(time.minute() == 59);
        REQUIRE(time.second() == 59);
        REQUIRE(time.secondFraction() == std::chrono::nanoseconds(999'999'999));

        // Total nanoseconds since midnight
        int64_t expectedNanoseconds = 23 * 3600 * 1'000'000'000LL
                                     + 59 * 60 * 1'000'000'000LL
                                     + 59 * 1'000'000'000LL
                                     + 999'999'999LL;
        REQUIRE(time.toNanoseconds().count() == expectedNanoseconds);

        // Offset should be positive
        REQUIRE_FALSE(time.offset().isLocalTime());
        REQUIRE_FALSE(time.offset().isUTC());
        REQUIRE(time.offset().totalSeconds() == std::chrono::seconds{3600});
        REQUIRE_FALSE(time.offset().isNegative());
        REQUIRE(time.offset().hours() == 1);
        REQUIRE(time.offset().minutes() == 0);
        REQUIRE(time.offset().seconds() == 0);

        // toText should return the correct ISO format with offset
        REQUIRE(time.toText() == "23:59:59.999999999+01:00");
    }

    void testConstructorWithHMSNegativeOffset() {
        TimeOffset negativeOffset(-7200); // -02:00:00
        Time time(0, 0, 0, 0, negativeOffset);

        // The time should not be undefined
        REQUIRE_FALSE(time.isUndefined());

        // Accessors should return the correct values
        REQUIRE(time.hour() == 0);
        REQUIRE(time.minute() == 0);
        REQUIRE(time.second() == 0);
        REQUIRE(time.secondFraction() == std::chrono::nanoseconds(0));

        // Total nanoseconds since midnight
        int64_t expectedNanoseconds = 0;
        REQUIRE(time.toNanoseconds().count() == expectedNanoseconds);

        // Offset should be negative
        REQUIRE_FALSE(time.offset().isLocalTime());
        REQUIRE_FALSE(time.offset().isUTC());
        REQUIRE(time.offset().totalSeconds() == std::chrono::seconds{-7200});
        REQUIRE(time.offset().isNegative());
        REQUIRE(time.offset().hours() == 2);
        REQUIRE(time.offset().minutes() == 0);
        REQUIRE(time.offset().seconds() == 0);

        // toText should return the correct ISO format with offset
        REQUIRE(time.toText() == "00:00:00-02:00");
    }

    void testConstructorWithNanosecondsUTC() {
        TimeOffset utcOffset(0); // UTC
        int64_t nanoseconds = 13 * 3600 * 1'000'000'000LL + 15 * 60 * 1'000'000'000LL + 30 * 1'000'000'000LL + 250'000'000LL; // 13:15:30.250
        Time time(nanoseconds, utcOffset);

        // The time should not be undefined
        REQUIRE_FALSE(time.isUndefined());

        // Accessors should return the correct values
        REQUIRE(time.hour() == 13);
        REQUIRE(time.minute() == 15);
        REQUIRE(time.second() == 30);
        REQUIRE(time.secondFraction() == std::chrono::nanoseconds(250'000'000));

        // Total nanoseconds since midnight
        REQUIRE(time.toNanoseconds().count() == nanoseconds);

        // Offset should be UTC
        REQUIRE(time.offset().isUTC());
        REQUIRE_FALSE(time.offset().isLocalTime());
        REQUIRE(time.offset().totalSeconds() == std::chrono::seconds{0});
        REQUIRE_FALSE(time.offset().isNegative());
        REQUIRE(time.offset().hours() == 0);
        REQUIRE(time.offset().minutes() == 0);
        REQUIRE(time.offset().seconds() == 0);

        // toText should return the correct ISO format with 'z'
        REQUIRE(time.toText() == "13:15:30.25z");
    }

    void testConstructorWithNanosecondsPositiveOffset() {
        TimeOffset positiveOffset(4500); // +01:15:00
        int64_t nanoseconds = 6 * 3600 * 1'000'000'000LL + 45 * 60 * 1'000'000'000LL + 15 * 1'000'000'000LL + 500'000'000LL; // 06:45:15.500
        Time time(nanoseconds, positiveOffset);

        // The time should not be undefined
        REQUIRE_FALSE(time.isUndefined());

        // Accessors should return the correct values
        REQUIRE(time.hour() == 6);
        REQUIRE(time.minute() == 45);
        REQUIRE(time.second() == 15);
        REQUIRE(time.secondFraction() == std::chrono::nanoseconds(500'000'000));

        // Total nanoseconds since midnight
        REQUIRE(time.toNanoseconds().count() == nanoseconds);

        // Offset should be positive
        REQUIRE_FALSE(time.offset().isLocalTime());
        REQUIRE_FALSE(time.offset().isUTC());
        REQUIRE(time.offset().totalSeconds() == std::chrono::seconds{4500});
        REQUIRE_FALSE(time.offset().isNegative());
        REQUIRE(time.offset().hours() == 1);
        REQUIRE(time.offset().minutes() == 15);
        REQUIRE(time.offset().seconds() == 0);

        // toText should return the correct ISO format with offset
        REQUIRE(time.toText() == "06:45:15.5+01:15");
    }

    void testConstructorWithNanosecondsNegativeOffset() {
        TimeOffset negativeOffset(-10800); // -03:00:00
        int64_t nanoseconds = 18 * 3600 * 1'000'000'000LL + 30 * 60 * 1'000'000'000LL + 45 * 1'000'000'000LL + 750'000'000LL; // 18:30:45.750
        Time time(nanoseconds, negativeOffset);

        // The time should not be undefined
        REQUIRE_FALSE(time.isUndefined());

        // Accessors should return the correct values
        REQUIRE(time.hour() == 18);
        REQUIRE(time.minute() == 30);
        REQUIRE(time.second() == 45);
        REQUIRE(time.secondFraction() == std::chrono::nanoseconds(750'000'000));

        // Total nanoseconds since midnight
        REQUIRE(time.toNanoseconds().count() == nanoseconds);

        // Offset should be negative
        REQUIRE_FALSE(time.offset().isLocalTime());
        REQUIRE_FALSE(time.offset().isUTC());
        REQUIRE(time.offset().totalSeconds() == std::chrono::seconds{-10800});
        REQUIRE(time.offset().isNegative());
        REQUIRE(time.offset().hours() == 3);
        REQUIRE(time.offset().minutes() == 0);
        REQUIRE(time.offset().seconds() == 0);

        // toText should return the correct ISO format with offset
        REQUIRE(time.toText() == "18:30:45.75-03:00");
    }

    void testConstructorWithHMSInvalidHour() {
        TimeOffset utcOffset(0); // UTC
        int invalidHour = 24; // Invalid, should be 0-23
        int minute = 0;
        int second = 0;
        int nanosecond = 0;

        // Expect the constructor to throw std::invalid_argument for invalid hour
        REQUIRE_THROWS_AS(std::invalid_argument, Time(invalidHour, minute, second, nanosecond, utcOffset));
    }

    void testConstructorWithHMSInvalidMinute() {
        TimeOffset utcOffset(0); // UTC
        int hour = 12;
        int invalidMinute = 60; // Invalid, should be 0-59
        int second = 0;
        int nanosecond = 0;

        // Expect the constructor to throw std::invalid_argument for invalid minute
        REQUIRE_THROWS_AS(std::invalid_argument, Time(hour, invalidMinute, second, nanosecond, utcOffset));
    }

    void testConstructorWithHMSInvalidSecond() {
        TimeOffset utcOffset(0); // UTC
        int hour = 12;
        int minute = 30;
        int invalidSecond = 60; // Invalid, should be 0-59
        int nanosecond = 0;

        // Expect the constructor to throw std::invalid_argument for invalid second
        REQUIRE_THROWS_AS(std::invalid_argument, Time(hour, minute, invalidSecond, nanosecond, utcOffset));
    }

    void testConstructorWithHMSInvalidNanosecond() {
        TimeOffset utcOffset(0); // UTC
        int hour = 12;
        int minute = 30;
        int second = 30;
        int invalidNanosecond = 1'000'000'000; // Invalid, should be 0-999'999'999

        // Expect the constructor to throw std::invalid_argument for invalid nanosecond
        REQUIRE_THROWS_AS(std::invalid_argument, Time(hour, minute, second, invalidNanosecond, utcOffset));
    }

    void testConstructorWithNanosecondsNegative() {
        TimeOffset utcOffset(0); // UTC
        int64_t invalidNanoseconds = -1; // Negative nanoseconds

        // Expect the constructor to throw std::invalid_argument for negative nanoseconds
        REQUIRE_THROWS_AS(std::invalid_argument, Time(invalidNanoseconds, utcOffset));
    }

    void testConstructorWithNanosecondsExceeds24h() {
        TimeOffset utcOffset(0); // UTC
        // 24 hours in nanoseconds: 24 * 3600 * 1'000'000'000 = 86'400'000'000'000
        int64_t invalidNanoseconds = 86'400'000'000'000LL; // Exactly 24h

        // Expect the constructor to throw std::invalid_argument for nanoseconds exceeding 24h
        REQUIRE_THROWS_AS(std::invalid_argument, Time(invalidNanoseconds, utcOffset));
    }

    void testCopyConstructor() {
        TimeOffset positiveOffset(3600); // +01:00:00
        Time original(14, 45, 30, 250'000'000, positiveOffset);
        Time copy = original;

        // The copied time should have the same properties as the original
        REQUIRE_FALSE(copy.isUndefined());
        REQUIRE(copy.hour() == original.hour());
        REQUIRE(copy.minute() == original.minute());
        REQUIRE(copy.second() == original.second());
        REQUIRE(copy.secondFraction() == original.secondFraction());
        REQUIRE(copy.toNanoseconds() == original.toNanoseconds());

        // Offsets should be equal
        REQUIRE(copy.offset().isLocalTime() == original.offset().isLocalTime());
        REQUIRE(copy.offset().isUTC() == original.offset().isUTC());
        REQUIRE(copy.offset().totalSeconds() == original.offset().totalSeconds());
        REQUIRE(copy.offset().isNegative() == original.offset().isNegative());
        REQUIRE(copy.offset().hours() == original.offset().hours());
        REQUIRE(copy.offset().minutes() == original.offset().minutes());
        REQUIRE(copy.offset().seconds() == original.offset().seconds());

        // toText should be equal
        REQUIRE(copy.toText() == original.toText());
    }

    void testCopyAssignment() {
        TimeOffset negativeOffset(-18000); // -05:00:00
        Time original(5, 15, 45, 750'000'000, negativeOffset);
        Time copy;
        copy = original;

        // The copied time should have the same properties as the original
        REQUIRE_FALSE(copy.isUndefined());
        REQUIRE(copy.hour() == original.hour());
        REQUIRE(copy.minute() == original.minute());
        REQUIRE(copy.second() == original.second());
        REQUIRE(copy.secondFraction() == original.secondFraction());
        REQUIRE(copy.toNanoseconds() == original.toNanoseconds());

        // Offsets should be equal
        REQUIRE(copy.offset().isLocalTime() == original.offset().isLocalTime());
        REQUIRE(copy.offset().isUTC() == original.offset().isUTC());
        REQUIRE(copy.offset().totalSeconds() == original.offset().totalSeconds());
        REQUIRE(copy.offset().isNegative() == original.offset().isNegative());
        REQUIRE(copy.offset().hours() == original.offset().hours());
        REQUIRE(copy.offset().minutes() == original.offset().minutes());
        REQUIRE(copy.offset().seconds() == original.offset().seconds());

        // toText should be equal
        REQUIRE(copy.toText() == original.toText());
    }

    void testMoveConstructor() {
        TimeOffset positiveOffset(7200); // +02:00:00
        Time original(20, 0, 0, 0, positiveOffset);
        Time moved = std::move(original);

        // The moved time should retain the original data
        REQUIRE_FALSE(moved.isUndefined());
        REQUIRE(moved.hour() == 20);
        REQUIRE(moved.minute() == 0);
        REQUIRE(moved.second() == 0);
        REQUIRE(moved.secondFraction() == std::chrono::nanoseconds(0));
        REQUIRE(moved.toNanoseconds().count() == 72'000'000'000'000LL);

        // Offset should be correctly moved
        REQUIRE(moved.offset().isLocalTime() == false);
        REQUIRE(moved.offset().isUTC() == false);
        REQUIRE(moved.offset().totalSeconds() == std::chrono::seconds{7200});
        REQUIRE_FALSE(moved.offset().isNegative());
        REQUIRE(moved.offset().hours() == 2);
        REQUIRE(moved.offset().minutes() == 0);
        REQUIRE(moved.offset().seconds() == 0);

        // The state of 'original' after move is unspecified; no checks performed
    }

    void testMoveAssignment() {
        TimeOffset negativeOffset(-3600); // -01:00:00
        Time original(7, 30, 15, 125'000'000, negativeOffset);
        Time moved;
        moved = std::move(original);

        // The moved time should retain the original data
        REQUIRE_FALSE(moved.isUndefined());
        REQUIRE(moved.hour() == 7);
        REQUIRE(moved.minute() == 30);
        REQUIRE(moved.second() == 15);
        REQUIRE(moved.secondFraction() == std::chrono::nanoseconds(125'000'000));
        REQUIRE(moved.toNanoseconds().count() == (7 * 3600 + 30 * 60 + 15) * 1'000'000'000LL + 125'000'000LL);

        // Offset should be correctly moved
        REQUIRE(moved.offset().isLocalTime() == false);
        REQUIRE(moved.offset().isUTC() == false);
        REQUIRE(moved.offset().totalSeconds() == std::chrono::seconds{-3600});
        REQUIRE(moved.offset().isNegative());
        REQUIRE(moved.offset().hours() == 1);
        REQUIRE(moved.offset().minutes() == 0);
        REQUIRE(moved.offset().seconds() == 0);

        // The state of 'original' after move is unspecified; no checks performed
    }

    void testIsUndefined() {
        Time undefinedTime;
        Time definedTime(0, 0, 0, 0, TimeOffset());

        // Only the default-constructed time should be undefined
        REQUIRE(undefinedTime.isUndefined());
        REQUIRE_FALSE(definedTime.isUndefined());
    }

    void testHourAccessor() {
        TimeOffset utcOffset(0); // UTC
        Time time(23, 59, 59, 999'999'999, utcOffset);

        REQUIRE(time.hour() == 23);
    }

    void testMinuteAccessor() {
        TimeOffset utcOffset(0); // UTC
        Time time(12, 34, 56, 789'000'000, utcOffset);

        REQUIRE(time.minute() == 34);
    }

    void testSecondAccessor() {
        TimeOffset utcOffset(0); // UTC
        Time time(6, 7, 8, 900'000'000, utcOffset);

        REQUIRE(time.second() == 8);
    }

    void testSecondFractionAccessor() {
        TimeOffset utcOffset(0); // UTC
        Time time(15, 45, 30, 123'456'789, utcOffset);

        REQUIRE(time.secondFraction() == std::chrono::nanoseconds(123'456'789));
    }

    void testOffsetAccessor() {
        TimeOffset positiveOffset(1800); // +00:30:00
        Time time(9, 15, 0, 0, positiveOffset);

        // Check that the offset accessor returns the correct offset
        REQUIRE(time.offset().isLocalTime() == positiveOffset.isLocalTime());
        REQUIRE(time.offset().isUTC() == positiveOffset.isUTC());
        REQUIRE(time.offset().totalSeconds() == positiveOffset.totalSeconds());
        REQUIRE(time.offset().isNegative() == positiveOffset.isNegative());
        REQUIRE(time.offset().hours() == positiveOffset.hours());
        REQUIRE(time.offset().minutes() == positiveOffset.minutes());
        REQUIRE(time.offset().seconds() == positiveOffset.seconds());
    }

    void testToNanoseconds() {
        const TimeOffset utcOffset(0); // UTC
        const Time time(1, 2, 3, 4, utcOffset);
        constexpr int64_t expectedNanoseconds = (1 * 3600 + 2 * 60 + 3) * 1'000'000'000LL + 4LL;
        REQUIRE(time.toNanoseconds().count() == expectedNanoseconds);
    }

    void testToText() {
        // Undefined time
        Time undefinedTime;
        REQUIRE(undefinedTime.toText().empty());

        // UTC time
        TimeOffset utcOffset(0); // UTC
        Time utcTime(0, 0, 0, 0, utcOffset);
        REQUIRE(utcTime.toText() == "00:00:00z");

        // Local time
        TimeOffset localOffset; // Default constructor represents local time
        Time localTime(12, 34, 56, 789'000'000, localOffset);
        // toText should return only the time without offset
        REQUIRE(localTime.toText() == "12:34:56.789");

        // Positive offset without nanoseconds
        TimeOffset positiveOffset(3600); // +01:00
        Time posOffsetTime(23, 59, 59, 0, positiveOffset);
        REQUIRE(posOffsetTime.toText() == "23:59:59+01:00");

        // Negative offset with nanoseconds
        TimeOffset negativeOffset(-7200); // -02:00
        Time negOffsetTime(5, 30, 15, 250'000'000, negativeOffset);
        REQUIRE(negOffsetTime.toText() == "05:30:15.25-02:00");

        // Time with fractional seconds and positive offset
        Time posOffsetFractionTime(14, 45, 30, 500'000'000, positiveOffset);
        REQUIRE(posOffsetFractionTime.toText() == "14:45:30.5+01:00");
    }

    void testOperators() {
        WITH_CONTEXT(requireAllOperators<Time, Time>(
            Time{5, 30, 15, 293, TimeOffset::utc()},
            Time{12, 22, 0, 0, TimeOffset::utc()},
            Time{23, 59, 59, 999'999'999, TimeOffset::utc()},
            Time{5, 30, 15, 293, TimeOffset::utc()},
            Time{12, 22, 0, 0, TimeOffset::utc()},
            Time{23, 59, 59, 999'999'999, TimeOffset::utc()}
        ));
        WITH_CONTEXT(requireStrictOrder(std::array<Time, 12>{
            Time{},
            Time{0, 0, 0, 0, TimeOffset::utc()},
            Time{3, 25, 0, 0, TimeOffset{-7200}},
            Time{2, 25, 0, 0, TimeOffset::utc()},
            Time{1, 25, 0, 0, TimeOffset{7200}},
            Time{10, 58, 12, 0, TimeOffset::utc()},
            Time{10, 58, 12, 1, TimeOffset::utc()},
            Time{10, 58, 13, 1, TimeOffset::utc()},
            Time{10, 59, 13, 1, TimeOffset::utc()},
            Time{11, 59, 13, 1, TimeOffset::utc()},
            Time{11, 59, 13, 2, TimeOffset{}}, // local time = UTC for comparison.
            Time{23, 59, 59, 999'999'999, TimeOffset::utc()}
        }));
    }
};

