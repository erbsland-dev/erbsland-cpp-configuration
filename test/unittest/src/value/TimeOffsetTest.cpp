// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/TimeOffset.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>
#include <utility>
#include <sstream>


using namespace el::conf;
using enum TimeOffset::Precision;


TESTED_TARGETS(TimeOffset)
class TimeOffsetTest final : public el::UnitTest {
public:
    TimeOffset offset;

    auto additionalErrorMessages() -> std::string override {
        try {
            auto text = std::ostringstream{};
            text << "offset.isLocalTime() == " << offset.isLocalTime() << '\n';
            text << "offset.isUTC() == " << offset.isUTC() << '\n';
            text << "offset.totalSeconds() == " << offset.totalSeconds().count() << '\n';
            text << "offset.isNegative() == " << offset.isNegative() << '\n';
            text << "offset.hours() == " << offset.hours() << '\n';
            text << "offset.minutes() == " << offset.minutes() << '\n';
            text << "offset.seconds() == " << offset.seconds() << '\n';
            text << "offset.toText() == \"" << offset.toText().toCharString() << "\"\n";
            text << "offset.toText(Hours) == \"" << offset.toText(Hours).toCharString() << "\"\n";
            text << "offset.toText(Minutes) == \"" << offset.toText(Minutes).toCharString() << "\"\n";
            text << "offset.toText(Seconds) == \"" << offset.toText(Seconds).toCharString() << "\"\n";
            return text.str();
        } catch (...) {
            return {"Unexpected exception"};
        }
    }

    void setUp() override {
        offset = {};
    }

    void testDefaultConstructor() {
        // The offset should represent local time
        REQUIRE(offset.isLocalTime());

        // The offset should not be UTC
        REQUIRE_FALSE(offset.isUTC());

        // Total seconds should be zero for local time
        REQUIRE(offset.totalSeconds() == std::chrono::seconds{0});

        // isNegative should be false for local time
        REQUIRE_FALSE(offset.isNegative());

        // Hours, minutes, and seconds should be zero for local time
        REQUIRE(offset.hours() == 0);
        REQUIRE(offset.minutes() == 0);
        REQUIRE(offset.seconds() == 0);

        // toText should return an empty string for local time
        REQUIRE(offset.toText().empty());
    }

    void testConstructorFromSecondsUTC() {
        offset = TimeOffset(0);

        // The offset should represent UTC
        REQUIRE(offset.isUTC());

        // The offset should not be local time
        REQUIRE_FALSE(offset.isLocalTime());

        // Total seconds should be zero for UTC
        REQUIRE(offset.totalSeconds() == std::chrono::seconds{0});

        // isNegative should be false for UTC
        REQUIRE_FALSE(offset.isNegative());

        // Hours, minutes, and seconds should be zero for UTC
        REQUIRE(offset.hours() == 0);
        REQUIRE(offset.minutes() == 0);
        REQUIRE(offset.seconds() == 0);

        // toText should return "z" for UTC
        REQUIRE(offset.toText() == "z");
    }

    void testConstructorFromSecondsPositive() {
        constexpr auto seconds = std::chrono::seconds{5400}; // +01:30:00
        offset = TimeOffset(seconds);

        // The offset should not be local time or UTC
        REQUIRE_FALSE(offset.isLocalTime());
        REQUIRE_FALSE(offset.isUTC());

        // Total seconds should match the input
        REQUIRE(offset.totalSeconds() == seconds);

        // isNegative should be false for positive offsets
        REQUIRE_FALSE(offset.isNegative());

        // Hours, minutes, and seconds should be correctly calculated
        REQUIRE(offset.hours() == 1);
        REQUIRE(offset.minutes() == 30);
        REQUIRE(offset.seconds() == 0);

        REQUIRE(offset.toText() == "+01:30");
        REQUIRE(offset.toText(Hours) == "+01");
        REQUIRE(offset.toText(Minutes) == "+01:30");
        REQUIRE(offset.toText(Seconds) == "+01:30:00");
    }

    void testConstructorFromSecondsNegative() {
        constexpr auto seconds = std::chrono::seconds{-8100}; // -02:15:00
        offset = TimeOffset(seconds);

        // The offset should not be local time or UTC
        REQUIRE_FALSE(offset.isLocalTime());
        REQUIRE_FALSE(offset.isUTC());

        // Total seconds should match the input
        REQUIRE(offset.totalSeconds() == seconds);

        // isNegative should be true for negative offsets
        REQUIRE(offset.isNegative());

        // Hours, minutes, and seconds should be correctly calculated
        REQUIRE(offset.hours() == 2);
        REQUIRE(offset.minutes() == 15);
        REQUIRE(offset.seconds() == 0);

        // toText should return the correct formatted string
        REQUIRE(offset.toText() == "-02:15");
        REQUIRE(offset.toText(Hours) == "-02");
        REQUIRE(offset.toText(Minutes) == "-02:15");
        REQUIRE(offset.toText(Seconds) == "-02:15:00");
    }

    void testConstructorFromSecondsInvalid() {
        // The valid range for seconds is limited (e.g., within +/- 24 hours)
        constexpr int32_t invalidSeconds = std::numeric_limits<int32_t>::max(); // Out of range
        REQUIRE_THROWS_AS(std::out_of_range, TimeOffset(invalidSeconds));
        REQUIRE_THROWS_AS(std::out_of_range, TimeOffset(std::chrono::seconds{invalidSeconds}));
    }

    void testConstructorFromHMSPositive() {
        constexpr bool isNegative = false;
        constexpr int hours = 5;
        constexpr int minutes = 45;
        constexpr int seconds = 30; // should be ignored in text.

        offset = TimeOffset(isNegative, hours, minutes, seconds);

        // The offset should not be local time or UTC
        REQUIRE_FALSE(offset.isLocalTime());
        REQUIRE_FALSE(offset.isUTC());

        // Total seconds should be correctly calculated
        REQUIRE(offset.totalSeconds() == std::chrono::seconds{5 * 3600 + 45 * 60 + 30});

        // isNegative should be false
        REQUIRE_FALSE(offset.isNegative());

        // Hours, minutes, and seconds should match the input
        REQUIRE(offset.hours() == hours);
        REQUIRE(offset.minutes() == minutes);
        REQUIRE(offset.seconds() == seconds);

        // toText should return the correct formatted string
        REQUIRE(offset.toText() == "+05:45:30");
        REQUIRE(offset.toText(Hours) == "+05");
        REQUIRE(offset.toText(Minutes) == "+05:45");
        REQUIRE(offset.toText(Seconds) == "+05:45:30");
    }

    void testConstructorFromHMSNegative() {
        constexpr bool isNegative = true;
        constexpr int hours = 3;
        constexpr int minutes = 20;
        constexpr int seconds = 15;

        offset = TimeOffset(isNegative, hours, minutes, seconds);

        // The offset should not be local time or UTC
        REQUIRE_FALSE(offset.isLocalTime());
        REQUIRE_FALSE(offset.isUTC());

        // Total seconds should be correctly calculated
        REQUIRE(offset.totalSeconds() == std::chrono::seconds{-(hours * 3600 + minutes * 60 + seconds)});

        // isNegative should be true
        REQUIRE(offset.isNegative());

        // Hours, minutes, and seconds should match the input
        REQUIRE(offset.hours() == hours);
        REQUIRE(offset.minutes() == minutes);
        REQUIRE(offset.seconds() == seconds);

        // toText should return the correct formatted string
        REQUIRE(offset.toText() == "-03:20:15");
        REQUIRE(offset.toText(Hours) == "-03");
        REQUIRE(offset.toText(Minutes) == "-03:20");
        REQUIRE(offset.toText(Seconds) == "-03:20:15");
    }

    void testConstructorFromHMSInvalidHours() {
        constexpr bool isNegative = false;
        constexpr int minutes = 0;
        constexpr int seconds = 0;

        // Expect the constructor to throw std::out_of_range for invalid hours
        REQUIRE_THROWS_AS(std::out_of_range, TimeOffset(isNegative, 24, minutes, seconds));
        REQUIRE_THROWS_AS(std::out_of_range, TimeOffset(isNegative, -1, minutes, seconds));
    }

    void testConstructorFromHMSInvalidMinutes() {
        constexpr bool isNegative = false;
        constexpr int hours = 12;
        constexpr int seconds = 0;

        // Expect the constructor to throw std::out_of_range for invalid minutes
        REQUIRE_THROWS_AS(std::out_of_range, TimeOffset(isNegative, hours, 60, seconds));
        REQUIRE_THROWS_AS(std::out_of_range, TimeOffset(isNegative, hours, -1, seconds));
    }

    void testConstructorFromHMSInvalidSeconds() {
        constexpr bool isNegative = false;
        constexpr int hours = 12;
        constexpr int minutes = 30;

        // Expect the constructor to throw std::out_of_range for invalid seconds
        REQUIRE_THROWS_AS(std::out_of_range, TimeOffset(isNegative, hours, minutes, 60));
        REQUIRE_THROWS_AS(std::out_of_range, TimeOffset(isNegative, hours, minutes, -1));
    }

    void testCopyConstructor() {
        auto original = TimeOffset(3600); // +01:00:00
        offset = original;

        // The copied offset should be equal to the original
        REQUIRE(offset.isLocalTime() == original.isLocalTime());
        REQUIRE(offset.isUTC() == original.isUTC());
        REQUIRE(offset.totalSeconds() == original.totalSeconds());
        REQUIRE(offset.isNegative() == original.isNegative());
        REQUIRE(offset.hours() == original.hours());
        REQUIRE(offset.minutes() == original.minutes());
        REQUIRE(offset.seconds() == original.seconds());
        REQUIRE(offset.toText() == original.toText());
    }

    void testCopyAssignment() {
        const TimeOffset offset1(-7200); // -02:00:00
        offset = offset1;

        // After copy assignment, offset2 should be equal to offset1
        REQUIRE(offset.isLocalTime() == offset1.isLocalTime());
        REQUIRE(offset.isUTC() == offset1.isUTC());
        REQUIRE(offset.totalSeconds() == offset1.totalSeconds());
        REQUIRE(offset.isNegative() == offset1.isNegative());
        REQUIRE(offset.hours() == offset1.hours());
        REQUIRE(offset.minutes() == offset1.minutes());
        REQUIRE(offset.seconds() == offset1.seconds());
        REQUIRE(offset.toText() == offset1.toText());
    }

    void testIsLocalTime() {
        const TimeOffset localOffset;
        const TimeOffset utcOffset(0);
        const TimeOffset positiveOffset(3600);
        const TimeOffset negativeOffset(-3600);

        // Only the default constructor should represent local time
        REQUIRE(localOffset.isLocalTime());

        // Other offsets should not represent local time
        REQUIRE_FALSE(utcOffset.isLocalTime());
        REQUIRE_FALSE(positiveOffset.isLocalTime());
        REQUIRE_FALSE(negativeOffset.isLocalTime());
    }

    void testIsUTC() {
        const TimeOffset localOffset;
        const TimeOffset utcOffset(0);
        const TimeOffset positiveOffset(3600);
        const TimeOffset negativeOffset(-3600);

        // Only the UTC offset should represent UTC
        REQUIRE(utcOffset.isUTC());

        // Other offsets should not represent UTC
        REQUIRE_FALSE(localOffset.isUTC());
        REQUIRE_FALSE(positiveOffset.isUTC());
        REQUIRE_FALSE(negativeOffset.isUTC());
    }

    void testTotalSeconds() {
        const TimeOffset localOffset;
        const TimeOffset utcOffset(0);
        const TimeOffset positiveOffset(5400);   // +01:30:00
        const TimeOffset negativeOffset(-8100);  // -02:15:00

        // Local time should return zero
        REQUIRE(localOffset.totalSeconds() == std::chrono::seconds{0});

        // UTC should return zero
        REQUIRE(utcOffset.totalSeconds() == std::chrono::seconds{0});

        // Positive offset should return the correct total seconds
        REQUIRE(positiveOffset.totalSeconds() == std::chrono::seconds{5400});

        // Negative offset should return the correct total seconds
        REQUIRE(negativeOffset.totalSeconds() == std::chrono::seconds{-8100});
    }

    void testIsNegative() {
        const TimeOffset localOffset;
        const TimeOffset utcOffset(0);
        const TimeOffset positiveOffset(3600);
        const TimeOffset negativeOffset(-3600);

        // Only negative offsets should return true
        REQUIRE_FALSE(localOffset.isNegative());
        REQUIRE_FALSE(utcOffset.isNegative());
        REQUIRE_FALSE(positiveOffset.isNegative());
        REQUIRE(negativeOffset.isNegative());
    }

    void testAccessors() {
        const TimeOffset localOffset;
        const TimeOffset utcOffset(0);
        const TimeOffset positiveOffset(3661);   // +01:01:01
        const TimeOffset negativeOffset(-7322);  // -02:02:02

        // Local time should have zero hours, minutes, and seconds
        REQUIRE(localOffset.hours() == 0);
        REQUIRE(localOffset.minutes() == 0);
        REQUIRE(localOffset.seconds() == 0);

        // UTC should have zero hours, minutes, and seconds
        REQUIRE(utcOffset.hours() == 0);
        REQUIRE(utcOffset.minutes() == 0);
        REQUIRE(utcOffset.seconds() == 0);

        // Positive offset should have correct hours, minutes, and seconds
        REQUIRE(positiveOffset.hours() == 1);
        REQUIRE(positiveOffset.minutes() == 1);
        REQUIRE(positiveOffset.seconds() == 1);

        // Negative offset should have correct hours, minutes, and seconds (always positive)
        REQUIRE(negativeOffset.hours() == 2);
        REQUIRE(negativeOffset.minutes() == 2);
        REQUIRE(negativeOffset.seconds() == 2);
    }

    void testToText() {
        const TimeOffset localOffset;
        const TimeOffset utcOffset(0);
        const TimeOffset positiveOffset(5400);    // +01:30:00
        const TimeOffset negativeOffset(-8100);   // -02:15:00
        const TimeOffset positiveWithSeconds(3661); // +01:01:01
        const TimeOffset negativeWithSeconds(-7322); // -02:02:02

        REQUIRE(localOffset.toText().empty());
        REQUIRE(utcOffset.toText() == "z");
        REQUIRE(positiveOffset.toText() == "+01:30");
        REQUIRE(negativeOffset.toText() == "-02:15");
        REQUIRE(positiveOffset.toText(Hours) == "+01");
        REQUIRE(negativeOffset.toText(Hours) == "-02");
        REQUIRE(positiveOffset.toText(Minutes) == "+01:30");
        REQUIRE(negativeOffset.toText(Minutes) == "-02:15");
        REQUIRE(positiveOffset.toText(Seconds) == "+01:30:00");
        REQUIRE(negativeOffset.toText(Seconds) == "-02:15:00");

        REQUIRE(positiveWithSeconds.toText() == "+01:01:01");
        REQUIRE(negativeWithSeconds.toText() == "-02:02:02");
        REQUIRE(positiveWithSeconds.toText(Hours) == "+01");
        REQUIRE(negativeWithSeconds.toText(Hours) == "-02");
        REQUIRE(positiveWithSeconds.toText(Minutes) == "+01:01");
        REQUIRE(negativeWithSeconds.toText(Minutes) == "-02:02");
        REQUIRE(positiveWithSeconds.toText(Seconds) == "+01:01:01");
        REQUIRE(negativeWithSeconds.toText(Seconds) == "-02:02:02");
    }

    void testConstructorFromHMSNegativeZero() {
        constexpr bool isNegative = true;
        constexpr int hours = 0;
        constexpr int minutes = 0;
        constexpr int seconds = 0;

        // Even if isNegative is true, zero offset should not be negative
        TimeOffset offset(isNegative, hours, minutes, seconds);

        // The offset should represent UTC
        REQUIRE(offset.isUTC());

        // isNegative should be false since the total seconds are zero
        REQUIRE_FALSE(offset.isNegative());

        // toText should return "z"
        REQUIRE(offset.toText() == "z");
    }
};

