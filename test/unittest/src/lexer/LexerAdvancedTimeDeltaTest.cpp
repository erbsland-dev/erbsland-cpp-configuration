// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"

#include <erbsland/conf/TimeDelta.hpp>


TESTED_TARGETS(Lexer) TAGS(TimeDelta)
class LexerAdvancedTimeDeltaTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    void testIntegerRange() {
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(0s)", TimeDelta{TimeUnit::Seconds, 0}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(1s)", TimeDelta{TimeUnit::Seconds, 1}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(+1s)", TimeDelta{TimeUnit::Seconds, 1}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(-1s)", TimeDelta{TimeUnit::Seconds, -1}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(23'932s)", TimeDelta{TimeUnit::Seconds, 23'932}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(+23'932s)", TimeDelta{TimeUnit::Seconds, 23'932}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(-529'000s)", TimeDelta{TimeUnit::Seconds, -529'000}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(-9223372036854775808s)", TimeDelta{TimeUnit::Seconds, std::numeric_limits<int64_t>::min()}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(9223372036854775807s)", TimeDelta{TimeUnit::Seconds, std::numeric_limits<int64_t>::max()}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(+9223372036854775807s)", TimeDelta{TimeUnit::Seconds, std::numeric_limits<int64_t>::max()}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(0 s)", TimeDelta{TimeUnit::Seconds, 0}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(1 s)", TimeDelta{TimeUnit::Seconds, 1}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(+1 s)", TimeDelta{TimeUnit::Seconds, 1}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(-1 s)", TimeDelta{TimeUnit::Seconds, -1}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(23'932 s)", TimeDelta{TimeUnit::Seconds, 23'932}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(+23'932 s)", TimeDelta{TimeUnit::Seconds, 23'932}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(-529'000 s)", TimeDelta{TimeUnit::Seconds, -529'000}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(-9223372036854775808 s)", TimeDelta{TimeUnit::Seconds, std::numeric_limits<int64_t>::min()}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(9223372036854775807 s)", TimeDelta{TimeUnit::Seconds, std::numeric_limits<int64_t>::max()}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(+9223372036854775807 s)", TimeDelta{TimeUnit::Seconds, std::numeric_limits<int64_t>::max()}));
    }

    void testUnits() {
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 nanosecond)", TimeDelta{TimeUnit::Nanoseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 nanoseconds)", TimeDelta{TimeUnit::Nanoseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 ns)", TimeDelta{TimeUnit::Nanoseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 microsecond)", TimeDelta{TimeUnit::Microseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 microseconds)", TimeDelta{TimeUnit::Microseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 us)", TimeDelta{TimeUnit::Microseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 µs)", TimeDelta{TimeUnit::Microseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 millisecond)", TimeDelta{TimeUnit::Milliseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 milliseconds)", TimeDelta{TimeUnit::Milliseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 ms)", TimeDelta{TimeUnit::Milliseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 second)", TimeDelta{TimeUnit::Seconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 seconds)", TimeDelta{TimeUnit::Seconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 s)", TimeDelta{TimeUnit::Seconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 minute)", TimeDelta{TimeUnit::Minutes, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 minutes)", TimeDelta{TimeUnit::Minutes, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 m)", TimeDelta{TimeUnit::Minutes, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 hour)", TimeDelta{TimeUnit::Hours, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 hours)", TimeDelta{TimeUnit::Hours, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 h)", TimeDelta{TimeUnit::Hours, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 day)", TimeDelta{TimeUnit::Days, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 days)", TimeDelta{TimeUnit::Days, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 d)", TimeDelta{TimeUnit::Days, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 week)", TimeDelta{TimeUnit::Weeks, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 weeks)", TimeDelta{TimeUnit::Weeks, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 w)", TimeDelta{TimeUnit::Weeks, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 month)", TimeDelta{TimeUnit::Months, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 months)", TimeDelta{TimeUnit::Months, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 year)", TimeDelta{TimeUnit::Years, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 years)", TimeDelta{TimeUnit::Years, 123}));
    }

    void testCaseInsensitivity() {
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 Nanosecond)", TimeDelta{TimeUnit::Nanoseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 NANOSECONDS)", TimeDelta{TimeUnit::Nanoseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 nS)", TimeDelta{TimeUnit::Nanoseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 mIcRoSeCoNd)", TimeDelta{TimeUnit::Microseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 MiCrOsEcOnDs)", TimeDelta{TimeUnit::Microseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 Us)", TimeDelta{TimeUnit::Microseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 µS)", TimeDelta{TimeUnit::Microseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 MILLISECOND)", TimeDelta{TimeUnit::Milliseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 MILLISECONDS)", TimeDelta{TimeUnit::Milliseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 MS)", TimeDelta{TimeUnit::Milliseconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 SECOND)", TimeDelta{TimeUnit::Seconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 SECONDS)", TimeDelta{TimeUnit::Seconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 S)", TimeDelta{TimeUnit::Seconds, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 MINUTE)", TimeDelta{TimeUnit::Minutes, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 MINUTES)", TimeDelta{TimeUnit::Minutes, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 M)", TimeDelta{TimeUnit::Minutes, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 HOUR)", TimeDelta{TimeUnit::Hours, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 HOURS)", TimeDelta{TimeUnit::Hours, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 H)", TimeDelta{TimeUnit::Hours, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 DAY)", TimeDelta{TimeUnit::Days, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 DAYS)", TimeDelta{TimeUnit::Days, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 D)", TimeDelta{TimeUnit::Days, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 WEEK)", TimeDelta{TimeUnit::Weeks, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 WEEKS)", TimeDelta{TimeUnit::Weeks, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 W)", TimeDelta{TimeUnit::Weeks, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 MONTH)", TimeDelta{TimeUnit::Months, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 MONTHS)", TimeDelta{TimeUnit::Months, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 YEAR)", TimeDelta{TimeUnit::Years, 123}));
        WITH_CONTEXT(verifyValidTimeDelta(u8R"(123 YEARS)", TimeDelta{TimeUnit::Years, 123}));
    }
};


