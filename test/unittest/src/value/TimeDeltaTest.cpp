// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/TimeDelta.hpp>
#include <erbsland/conf/TimeUnit.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>
#include <cmath>


using namespace erbsland::conf;


TESTED_TARGETS(TimeDelta)
class TimeDeltaTest final : public el::UnitTest {
public:
    // Test the default constructor
    void testDefaultConstructor() {
        TimeDelta delta;
        REQUIRE(delta.nanoseconds() == 0);
        REQUIRE(delta.microseconds() == 0);
        REQUIRE(delta.milliseconds() == 0);
        REQUIRE(delta.seconds() == 0);
        REQUIRE(delta.minutes() == 0);
        REQUIRE(delta.hours() == 0);
        REQUIRE(delta.days() == 0);
        REQUIRE(delta.weeks() == 0);
        REQUIRE(delta.months() == 0);
        REQUIRE(delta.years() == 0);
        REQUIRE(delta.isSecondBased() == true);
        REQUIRE(delta.toSeconds() == 0.0);
    }

    void testSingleUnitConstructor() {
        TimeDelta deltaSeconds(TimeUnit::Seconds, 10);
        REQUIRE(deltaSeconds.seconds() == 10);
        REQUIRE(deltaSeconds.isSecondBased() == true);
        REQUIRE(deltaSeconds.toSeconds() == 10.0);

        TimeDelta deltaMinutes(TimeUnit::Minutes, 5);
        REQUIRE(deltaMinutes.minutes() == 5);
        REQUIRE(deltaMinutes.isSecondBased() == true);
        REQUIRE(deltaMinutes.toSeconds() == 300.0);

        TimeDelta deltaMonths(TimeUnit::Months, 2);
        REQUIRE(deltaMonths.months() == 2);
        REQUIRE(deltaMonths.isSecondBased() == false);
        REQUIRE(deltaMonths.toSeconds() == 2 * 2628000.0); // 2 * 30.44 * 86400
    }

    void testAccessorsAndMutators() {
        TimeDelta delta;

        // Nanoseconds
        delta.setNanoseconds(100);
        REQUIRE(delta.nanoseconds() == 100);
        REQUIRE(delta.count(TimeUnit::Nanoseconds) == 100);
        REQUIRE(delta.count<TimeUnit::Nanoseconds>() == 100);

        // Microseconds
        delta.setMicroseconds(200);
        REQUIRE(delta.microseconds() == 200);
        REQUIRE(delta.count(TimeUnit::Microseconds) == 200);
        REQUIRE(delta.count<TimeUnit::Microseconds>() == 200);

        // Milliseconds
        delta.setMilliseconds(300);
        REQUIRE(delta.milliseconds() == 300);
        REQUIRE(delta.count(TimeUnit::Milliseconds) == 300);
        REQUIRE(delta.count<TimeUnit::Milliseconds>() == 300);

        // Seconds
        delta.setSeconds(400);
        REQUIRE(delta.seconds() == 400);
        REQUIRE(delta.count(TimeUnit::Seconds) == 400);
        REQUIRE(delta.count<TimeUnit::Seconds>() == 400);

        // Minutes
        delta.setMinutes(50);
        REQUIRE(delta.minutes() == 50);
        REQUIRE(delta.count(TimeUnit::Minutes) == 50);
        REQUIRE(delta.count<TimeUnit::Minutes>() == 50);

        // Hours
        delta.setHours(6);
        REQUIRE(delta.hours() == 6);
        REQUIRE(delta.count(TimeUnit::Hours) == 6);
        REQUIRE(delta.count<TimeUnit::Hours>() == 6);

        // Days
        delta.setDays(7);
        REQUIRE(delta.days() == 7);
        REQUIRE(delta.count(TimeUnit::Days) == 7);
        REQUIRE(delta.count<TimeUnit::Days>() == 7);

        // Weeks
        delta.setWeeks(8);
        REQUIRE(delta.weeks() == 8);
        REQUIRE(delta.count(TimeUnit::Weeks) == 8);
        REQUIRE(delta.count<TimeUnit::Weeks>() == 8);

        // Months
        delta.setMonths(9);
        REQUIRE(delta.months() == 9);
        REQUIRE(delta.count(TimeUnit::Months) == 9);
        REQUIRE(delta.count<TimeUnit::Months>() == 9);

        // Years
        delta.setYears(10);
        REQUIRE(delta.years() == 10);
        REQUIRE(delta.count(TimeUnit::Years) == 10);
        REQUIRE(delta.count<TimeUnit::Years>() == 10);
    }

    void testEqualityOperators() {
        TimeDelta delta1;
        TimeDelta delta2;

        REQUIRE(delta1 == delta2);
        REQUIRE(!(delta1 != delta2));

        delta1.setSeconds(10);
        REQUIRE(!(delta1 == delta2));
        REQUIRE(delta1 != delta2);

        delta2.setSeconds(10);
        REQUIRE(delta1 == delta2);
        REQUIRE(!(delta1 != delta2));

        // Test with different units but same total seconds
        TimeDelta delta3(TimeUnit::Minutes, 1);
        TimeDelta delta4(TimeUnit::Seconds, 60);
        REQUIRE(delta3 == delta4);
        REQUIRE(!(delta3 != delta4));

        // Test with floating point precision
        TimeDelta delta5(TimeUnit::Seconds, 1);
        TimeDelta delta6(TimeUnit::Milliseconds, 1000);
        REQUIRE(delta5 == delta6);
    }

    void testRelationalOperators() {
        TimeDelta delta1(TimeUnit::Seconds, 30);
        TimeDelta delta2(TimeUnit::Seconds, 60);
        TimeDelta delta3(TimeUnit::Minutes, 1); // 60 seconds

        REQUIRE(delta1 < delta2);
        REQUIRE(delta1 <= delta2);
        REQUIRE(delta2 > delta1);
        REQUIRE(delta2 >= delta1);
        REQUIRE(delta2 <= delta3);
        REQUIRE(delta2 >= delta3);
        REQUIRE(!(delta1 > delta2));
        REQUIRE(!(delta1 >= delta2));
        REQUIRE(!(delta2 < delta1));
        REQUIRE(!(delta2 <= delta1));
    }

    void testArithmeticOperators() {
        TimeDelta delta1(TimeUnit::Seconds, 30);
        TimeDelta delta2(TimeUnit::Seconds, 45);

        // Test addition
        TimeDelta sum = delta1 + delta2;
        REQUIRE(sum.seconds() == 75);
        REQUIRE(sum.toSeconds() == 75.0);

        // Test subtraction
        TimeDelta diff = delta2 - delta1;
        REQUIRE(diff.seconds() == 15);
        REQUIRE(diff.toSeconds() == 15.0);

        // Test +=
        delta1 += delta2;
        REQUIRE(delta1.seconds() == 75);
        REQUIRE(delta1.toSeconds() == 75.0);

        // Test -=
        delta1 -= delta2;
        REQUIRE(delta1.seconds() == 30);
        REQUIRE(delta1.toSeconds() == 30.0);

        // Test adding different units
        TimeDelta delta3(TimeUnit::Minutes, 2); // 120 seconds
        TimeDelta sum2 = delta1 + delta3;
        REQUIRE(sum2.seconds() == 30);
        REQUIRE(sum2.minutes() == 2);
        REQUIRE(sum2.toSeconds() == 150.0);

        // Test subtracting different units
        TimeDelta diff2 = delta3 - delta1;
        REQUIRE(diff2.minutes() == 2);
        REQUIRE(diff2.seconds() == -30);
        REQUIRE(diff2.toSeconds() == 90.0); // 120 - 30
    }

    void testToSeconds() {
        TimeDelta delta;
        delta.setSeconds(10);
        REQUIRE(std::abs(delta.toSeconds() - 10.0) < 1e-9);

        delta.setMinutes(2); // 120 seconds
        REQUIRE(std::abs(delta.toSeconds() - 130.0) < 1e-9);

        delta.setHours(1); // 3600 seconds
        REQUIRE(std::abs(delta.toSeconds() - 3730.0) < 1e-9);

        delta.setMonths(1); // 2628000 seconds
        REQUIRE(std::abs(delta.toSeconds() - (3730.0 + 2628000.0)) < 1e-3); // Allow slight precision loss
    }

    void testToNanoseconds() {
        TimeDelta delta;
        delta.setSeconds(1);
        REQUIRE(delta.toNanoseconds() == 1000000000);

        delta.setMilliseconds(500);
        REQUIRE(delta.toNanoseconds() == 1000000000 + 500000000);

        delta.setMicroseconds(250);
        REQUIRE(delta.toNanoseconds() == 1000000000 + 500000000 + 250000);

        delta.setNanoseconds(125);
        REQUIRE(delta.toNanoseconds() == 1000000000 + 500000000 + 250000 + 125);

        // Test with no overflow
        TimeDelta deltaNoOverflow(TimeUnit::Seconds, 100);
        REQUIRE(deltaNoOverflow.toNanoseconds() == 100000000000);

        // Test overflow scenario (this value exceeds int64_t)
        TimeDelta deltaOverflow;
        deltaOverflow.setYears(std::numeric_limits<int32_t>::max());
        REQUIRE_THROWS_AS(std::domain_error, deltaOverflow.toNanoseconds() == 0);
    }

    void testIsSecondBased() {
        TimeDelta delta1;
        REQUIRE(delta1.isSecondBased() == true);

        TimeDelta delta2(TimeUnit::Minutes, 5);
        REQUIRE(delta2.isSecondBased() == true);

        TimeDelta delta3(TimeUnit::Months, 1);
        REQUIRE(delta3.isSecondBased() == false);

        TimeDelta delta4;
        delta4.setSeconds(10);
        delta4.setMonths(2);
        REQUIRE(delta4.isSecondBased() == false);
    }

    void testToText() {
        TimeDelta delta;
        delta.setSeconds(10);
        delta.setMinutes(5);
        delta.setHours(2);
        delta.setDays(1);
        delta.setWeeks(3);
        delta.setMonths(4);
        delta.setYears(1);

        String text = delta.toText();
        REQUIRE(text.contains(u8"1 Years"));
        REQUIRE(text.contains(u8"4 Months"));
        REQUIRE(text.contains(u8"3 Weeks"));
        REQUIRE(text.contains(u8"1d"));
        REQUIRE(text.contains(u8"2h"));
        REQUIRE(text.contains(u8"5m"));
        REQUIRE(text.contains(u8"10s"));
    }

    void testExceptionOnToNanosecondsOverflow() {
        TimeDelta delta;
        delta.setYears(1000000); // This should cause an overflow when converted to nanoseconds
        REQUIRE_THROWS_AS(std::domain_error, delta.toNanoseconds() == 0);
        delta = {};
        delta.setDays(106'751); // This is OK
        REQUIRE_NOTHROW(delta.toNanoseconds() == 9223286400000000000ll);
        delta.setHours(2'562'047); // This itself is ok
        // But if nanoseconds for days and hours are added, it causes an overflow.
        REQUIRE_THROWS_AS(std::domain_error, delta.toNanoseconds() == 0);
    }

    void testMultipleUnitsCombination() {
        TimeDelta delta;
        delta.setYears(1);    // 31,557,600 seconds
        delta.setMonths(6);   // 6 * 2,628,000 = 15,768,000 seconds
        delta.setWeeks(2);    // 2 * 604,800 = 1,209,600 seconds
        delta.setDays(3);     // 3 * 86,400 = 259,200 seconds
        delta.setHours(4);    // 4 * 3,600 = 14,400 seconds
        delta.setMinutes(5);  // 5 * 60 = 300 seconds
        delta.setSeconds(6);  // 6 seconds

        double expectedSeconds = 31557600.0 + 15768000.0 + 1209600.0 + 259200.0 + 14400.0 + 300.0 + 6.0;
        REQUIRE(std::abs(delta.toSeconds() - expectedSeconds) < 1e-3);
    }

    void testNegativeCounts() {
        TimeDelta delta;
        delta.setSeconds(-10);
        delta.setMinutes(-5);

        REQUIRE(delta.seconds() == -10);
        REQUIRE(delta.minutes() == -5);
        REQUIRE(delta.toSeconds() == -10.0 + (-5) * 60.0);

        TimeDelta delta2(TimeUnit::Seconds, -10);
        TimeDelta delta3(TimeUnit::Minutes, -5);
        REQUIRE(delta == delta2 + delta3);
    }

    void testNegateDelta() {
        TimeDelta delta{TimeUnit::Seconds, 20};
        delta.setCount(TimeUnit::Hours, -10);
        auto negatedDelta = -delta;
        REQUIRE(negatedDelta.seconds() == -20);
        REQUIRE(negatedDelta.hours() == 10);
    }

    void testZeroCounts() {
        TimeDelta delta;
        delta.setSeconds(0);
        delta.setMinutes(0);
        delta.setHours(0);

        REQUIRE(delta.toSeconds() == 0.0);
        REQUIRE(delta.isSecondBased() == true);
    }

    void testLargeCountsToSeconds() {
        TimeDelta delta;
        delta.setYears(10); // 10 * 31,557,600 = 315,576,000 seconds
        delta.setMonths(120); // 120 * 2,628,000 = 315,360,000 seconds
        delta.setSeconds(3600); // 3,600 seconds

        double expectedSeconds = 315576000.0 + 315360000.0 + 3600.0;
        REQUIRE(std::abs(delta.toSeconds() - expectedSeconds) < 1e-3);
    }
};

