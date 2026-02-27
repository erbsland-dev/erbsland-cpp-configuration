// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/TimeUnit.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <unordered_map>
#include <stdexcept>
#include <array>
#include <string>


using namespace el::conf;


TESTED_TARGETS(TimeUnit)
class TimeUnitTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultConstructor() {
        TimeUnit unit;

        // The default unit should be Seconds
        REQUIRE(unit == TimeUnit::Seconds);
        REQUIRE_FALSE(unit != TimeUnit::Seconds);
        REQUIRE_FALSE(unit < TimeUnit::Seconds);
        REQUIRE_FALSE(TimeUnit::Seconds < unit);
    }

    void testParameterizedConstructor() {
        TimeUnit nanosec(TimeUnit::Nanoseconds);
        TimeUnit microsec(TimeUnit::Microseconds);
        TimeUnit millisec(TimeUnit::Milliseconds);
        TimeUnit sec(TimeUnit::Seconds);
        TimeUnit min(TimeUnit::Minutes);
        TimeUnit hour(TimeUnit::Hours);
        TimeUnit day(TimeUnit::Days);
        TimeUnit week(TimeUnit::Weeks);
        TimeUnit month(TimeUnit::Months);
        TimeUnit year(TimeUnit::Years);

        REQUIRE(nanosec == TimeUnit::Nanoseconds);
        REQUIRE(microsec == TimeUnit::Microseconds);
        REQUIRE(millisec == TimeUnit::Milliseconds);
        REQUIRE(sec == TimeUnit::Seconds);
        REQUIRE(min == TimeUnit::Minutes);
        REQUIRE(hour == TimeUnit::Hours);
        REQUIRE(day == TimeUnit::Days);
        REQUIRE(week == TimeUnit::Weeks);
        REQUIRE(month == TimeUnit::Months);
        REQUIRE(year == TimeUnit::Years);
    }

    void testOperators() {
        WITH_CONTEXT(requireAllOperators<TimeUnit, TimeUnit>(
            TimeUnit{TimeUnit::Nanoseconds}, TimeUnit{TimeUnit::Microseconds}, TimeUnit{TimeUnit::Years},
            TimeUnit{TimeUnit::Nanoseconds}, TimeUnit{TimeUnit::Microseconds}, TimeUnit{TimeUnit::Years}
        ));
        WITH_CONTEXT(requireAllOperators<TimeUnit, TimeUnit::Enum>(
            TimeUnit{TimeUnit::Nanoseconds}, TimeUnit{TimeUnit::Microseconds}, TimeUnit{TimeUnit::Years},
            TimeUnit::Nanoseconds, TimeUnit::Microseconds, TimeUnit::Years
        ));
        WITH_CONTEXT(requireAllOperators<TimeUnit::Enum, TimeUnit>(
            TimeUnit::Nanoseconds, TimeUnit::Microseconds, TimeUnit::Years,
            TimeUnit{TimeUnit::Nanoseconds}, TimeUnit{TimeUnit::Microseconds}, TimeUnit{TimeUnit::Years}
        ));
    }

    void testOrder() {
        WITH_CONTEXT(requireStrictOrder(std::array<TimeUnit, 10>{
            TimeUnit{TimeUnit::Nanoseconds},
            TimeUnit{TimeUnit::Microseconds},
            TimeUnit{TimeUnit::Milliseconds},
            TimeUnit{TimeUnit::Seconds},
            TimeUnit{TimeUnit::Minutes},
            TimeUnit{TimeUnit::Hours},
            TimeUnit{TimeUnit::Days},
            TimeUnit{TimeUnit::Weeks},
            TimeUnit{TimeUnit::Months},
            TimeUnit{TimeUnit::Years}
        }));
        WITH_CONTEXT(requireStrictOrder(TimeUnit::all()));
    }

    void testConversionToEnum() {
        TimeUnit unit(TimeUnit::Hours);
        TimeUnit::Enum enumVal = static_cast<TimeUnit::Enum>(unit);

        REQUIRE(enumVal == TimeUnit::Hours);
    }

    void testTextLong() {
        TimeUnit nanosec(TimeUnit::Nanoseconds);
        TimeUnit microsec(TimeUnit::Microseconds);
        TimeUnit millisec(TimeUnit::Milliseconds);
        TimeUnit sec(TimeUnit::Seconds);
        TimeUnit min(TimeUnit::Minutes);
        TimeUnit hour(TimeUnit::Hours);
        TimeUnit day(TimeUnit::Days);
        TimeUnit week(TimeUnit::Weeks);
        TimeUnit month(TimeUnit::Months);
        TimeUnit year(TimeUnit::Years);

        REQUIRE(nanosec.toTextLong() == u8"Nanoseconds");
        REQUIRE(microsec.toTextLong() == u8"Microseconds");
        REQUIRE(millisec.toTextLong() == u8"Milliseconds");
        REQUIRE(sec.toTextLong() == u8"Seconds");
        REQUIRE(min.toTextLong() == u8"Minutes");
        REQUIRE(hour.toTextLong() == u8"Hours");
        REQUIRE(day.toTextLong() == u8"Days");
        REQUIRE(week.toTextLong() == u8"Weeks");
        REQUIRE(month.toTextLong() == u8"Months");
        REQUIRE(year.toTextLong() == u8"Years");
    }

    void testTextShort() {
        TimeUnit nanosec(TimeUnit::Nanoseconds);
        TimeUnit microsec(TimeUnit::Microseconds);
        TimeUnit millisec(TimeUnit::Milliseconds);
        TimeUnit sec(TimeUnit::Seconds);
        TimeUnit min(TimeUnit::Minutes);
        TimeUnit hour(TimeUnit::Hours);
        TimeUnit day(TimeUnit::Days);
        TimeUnit week(TimeUnit::Weeks);
        TimeUnit month(TimeUnit::Months);
        TimeUnit year(TimeUnit::Years);

        REQUIRE(nanosec.toTextShort() == u8"ns");
        REQUIRE(microsec.toTextShort() == u8"µs");
        REQUIRE(millisec.toTextShort() == u8"ms");
        REQUIRE(sec.toTextShort() == u8"s");
        REQUIRE(min.toTextShort() == u8"m");
        REQUIRE(hour.toTextShort() == u8"h");
        REQUIRE(day.toTextShort() == u8"d");
        REQUIRE(week.toTextShort().empty());
        REQUIRE(month.toTextShort().empty());
        REQUIRE(year.toTextShort().empty());
    }

    void testSecondFactor() {
        TimeUnit nanosec(TimeUnit::Nanoseconds);
        TimeUnit microsec(TimeUnit::Microseconds);
        TimeUnit millisec(TimeUnit::Milliseconds);
        TimeUnit sec(TimeUnit::Seconds);
        TimeUnit min(TimeUnit::Minutes);
        TimeUnit hour(TimeUnit::Hours);
        TimeUnit day(TimeUnit::Days);
        TimeUnit week(TimeUnit::Weeks);
        TimeUnit month(TimeUnit::Months);
        TimeUnit year(TimeUnit::Years);

        REQUIRE(nanosec.secondFactor() == 1e-9);
        REQUIRE(microsec.secondFactor() == 1e-6);
        REQUIRE(millisec.secondFactor() == 1e-3);
        REQUIRE(sec.secondFactor() == 1.0);
        REQUIRE(min.secondFactor() == 60.0);
        REQUIRE(hour.secondFactor() == 3'600.0);
        REQUIRE(day.secondFactor() == 86'400.0);
        REQUIRE(week.secondFactor() == 604'800.0);
        REQUIRE(month.secondFactor() == 2'628'000.0); // Approximate average (30.44 days)
        REQUIRE(year.secondFactor() == 31'557'600.0); // Approximate average (365.25 days)
    }

    void testNanosecondsFactor() {
        TimeUnit nanosec(TimeUnit::Nanoseconds);
        TimeUnit microsec(TimeUnit::Microseconds);
        TimeUnit millisec(TimeUnit::Milliseconds);
        TimeUnit sec(TimeUnit::Seconds);
        TimeUnit min(TimeUnit::Minutes);
        TimeUnit hour(TimeUnit::Hours);
        TimeUnit day(TimeUnit::Days);
        TimeUnit week(TimeUnit::Weeks);
        TimeUnit month(TimeUnit::Months);
        TimeUnit year(TimeUnit::Years);

        REQUIRE(nanosec.nanosecondsFactor() == 1ll);
        REQUIRE(microsec.nanosecondsFactor() == 1000ll);
        REQUIRE(millisec.nanosecondsFactor() == 1'000'000ll);
        REQUIRE(sec.nanosecondsFactor() == 1'000'000'000ll);
        REQUIRE(min.nanosecondsFactor() == 60'000'000'000ll);
        REQUIRE(hour.nanosecondsFactor() == 3'600'000'000'000ll);
        REQUIRE(day.nanosecondsFactor() == 86'400'000'000'000ll);
        REQUIRE(week.nanosecondsFactor() == 604'800'000'000'000ll);
        REQUIRE(month.nanosecondsFactor() == 2'628'000'000'000'000ll); // Approximate average
        REQUIRE(year.nanosecondsFactor() == 31'557'600'000'000'000ll); // Approximate average
    }

    void testAllMethod() {
        const auto& allUnits = TimeUnit::all();

        // There should be exactly 10 units
        REQUIRE(allUnits.size() == 10);

        // Define the expected order
        std::array<TimeUnit::Enum, 10> expectedOrder = {
            TimeUnit::Nanoseconds,
            TimeUnit::Microseconds,
            TimeUnit::Milliseconds,
            TimeUnit::Seconds,
            TimeUnit::Minutes,
            TimeUnit::Hours,
            TimeUnit::Days,
            TimeUnit::Weeks,
            TimeUnit::Months,
            TimeUnit::Years
        };

        // Verify each unit in order
        for (size_t i = 0; i < expectedOrder.size(); ++i) {
            REQUIRE(static_cast<TimeUnit::Enum>(allUnits[i]) == expectedOrder[i]);
        }
    }

    void testHashSpecialization() {
        std::unordered_map<TimeUnit, std::string> unitMap;

        // Insert all TimeUnits with their textLong() as values
        for (const auto& unit : TimeUnit::all()) {
            unitMap[unit] = unit.toTextLong().toCharString();
        }

        // Verify that all inserted keys are present with correct values
        for (const auto& unit : TimeUnit::all()) {
            auto it = unitMap.find(unit);
            REQUIRE(it != unitMap.end());
            REQUIRE(it->second == unit.toTextLong().toCharString());
        }

        // Verify that hashing works for specific units
        TimeUnit sec(TimeUnit::Seconds);
        REQUIRE(unitMap.find(sec) != unitMap.end());
        REQUIRE(unitMap[sec] == "Seconds");
    }

    void testEnumerationCompleteness() {
        const auto& allUnits = TimeUnit::all();

        // Ensure all enumeration values are present
        std::unordered_map<TimeUnit::Enum, bool> enumPresence;
        for (const auto& unit : allUnits) {
            enumPresence[static_cast<TimeUnit::Enum>(unit)] = true;
        }

        for (size_t i = 0; i < 10; ++i) {
            REQUIRE(enumPresence[static_cast<TimeUnit::Enum>(i)] == true);
        }
    }
};

