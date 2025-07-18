// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"

#include <thread>
#include <future>


TESTED_TARGETS(Lexer) TAGS(DateTime)
class LexerStandardDateTimeTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    enum class Part : uint8_t {
        Year,
        Month,
        Day,
        Hour,
        Minute,
        Second,
        Millisecond,
        Microsecond,
        Nanosecond,
        TimeZoneHour,
        TimeZoneSeconds,
    };

    struct ReplacementValue {
        std::string text; // The text to replace
        int64_t value; // The value representation.
    };

    using ReplacementValues = std::vector<ReplacementValue>;

    struct Replacements {
        Part part; // The part to replace
        std::string defaultText; // The default text.
        int64_t defaultValue; // The default value.
        ReplacementValues values; // The replacements to apply.
    };

    struct PatternIterator {
        std::size_t index; // The index of the pattern position.
        std::size_t length; // The length of the pattern.
        std::size_t iteration = 0; // The current iteration.
        const Replacements &replacements; // reference to the replacements
    };

    using PatternIterators = std::vector<PatternIterator>;

    struct TestData {
        std::string pattern;
        std::string valueText;
        bool expectError = false;
        DateTime expectedDateTime;
    };
    using TestDataList = std::vector<TestData>;

    struct AsyncTestResult {
        bool success = false;
        DateTime actualDateTime;
        std::string errorMessage;
    };

    struct RunningTest {
        TestData testData;
        std::future<AsyncTestResult> future;
    };

    inline static const auto dateTimePatterns = std::vector<std::string>{
        "YYYY-mm-DD HH:MM",
        "YYYY-mm-DD HH:MMz",
        "YYYY-mm-DD HH:MMZ",
        "YYYY-mm-DD HH:MMJJJ",
        "YYYY-mm-DD HH:MMKKKKKK",
        "YYYY-mm-DD HH:MM:SS",
        "YYYY-mm-DD HH:MM:SSz",
        "YYYY-mm-DD HH:MM:SSZ",
        "YYYY-mm-DD HH:MM:SSJJJ",
        "YYYY-mm-DD HH:MM:SSKKKKKK",
        "YYYY-mm-DD HH:MM:SS.EEE",
        "YYYY-mm-DD HH:MM:SS.EEEz",
        "YYYY-mm-DD HH:MM:SS.EEEZ",
        "YYYY-mm-DD HH:MM:SS.EEEJJJ",
        "YYYY-mm-DD HH:MM:SS.EEEKKKKKK",
        "YYYY-mm-DD HH:MM:SS.FFFFFF",
        "YYYY-mm-DD HH:MM:SS.FFFFFFz",
        "YYYY-mm-DD HH:MM:SS.FFFFFFJJJ",
        "YYYY-mm-DD HH:MM:SS.FFFFFFKKKKKK",
        "YYYY-mm-DD HH:MM:SS.GGGGGGGGG",
        "YYYY-mm-DD HH:MM:SS.GGGGGGGGGz",
        "YYYY-mm-DD HH:MM:SS.GGGGGGGGGJJJ",
        "YYYY-mm-DD HH:MM:SS.GGGGGGGGGKKKKKK",
        "YYYY-mm-DDtHH:MM",
        "YYYY-mm-DDtHH:MMz",
        "YYYY-mm-DDtHH:MMZ",
        "YYYY-mm-DDtHH:MMJJJ",
        "YYYY-mm-DDtHH:MMKKKKKK",
        "YYYY-mm-DDtHH:MM:SS",
        "YYYY-mm-DDtHH:MM:SSz",
        "YYYY-mm-DDtHH:MM:SSZ",
        "YYYY-mm-DDtHH:MM:SSJJJ",
        "YYYY-mm-DDtHH:MM:SSKKKKKK",
        "YYYY-mm-DDtHH:MM:SS.EEE",
        "YYYY-mm-DDtHH:MM:SS.EEEz",
        "YYYY-mm-DDtHH:MM:SS.EEEZ",
        "YYYY-mm-DDtHH:MM:SS.EEEJJJ",
        "YYYY-mm-DDtHH:MM:SS.EEEKKKKKK",
        "YYYY-mm-DDtHH:MM:SS.FFFFFF",
        "YYYY-mm-DDtHH:MM:SS.FFFFFFz",
        "YYYY-mm-DDtHH:MM:SS.FFFFFFJJJ",
        "YYYY-mm-DDtHH:MM:SS.FFFFFFKKKKKK",
        "YYYY-mm-DDtHH:MM:SS.GGGGGGGGG",
        "YYYY-mm-DDtHH:MM:SS.GGGGGGGGGz",
        "YYYY-mm-DDtHH:MM:SS.GGGGGGGGGJJJ",
        "YYYY-mm-DDtHH:MM:SS.GGGGGGGGGKKKKKK",
        "YYYY-mm-DDTHH:MM",
        "YYYY-mm-DDTHH:MMz",
        "YYYY-mm-DDTHH:MMZ",
        "YYYY-mm-DDTHH:MMJJJ",
        "YYYY-mm-DDTHH:MMKKKKKK",
        "YYYY-mm-DDTHH:MM:SS",
        "YYYY-mm-DDTHH:MM:SSz",
        "YYYY-mm-DDTHH:MM:SSZ",
        "YYYY-mm-DDTHH:MM:SSJJJ",
        "YYYY-mm-DDTHH:MM:SSKKKKKK",
        "YYYY-mm-DDTHH:MM:SS.EEE",
        "YYYY-mm-DDTHH:MM:SS.EEEz",
        "YYYY-mm-DDTHH:MM:SS.EEEZ",
        "YYYY-mm-DDTHH:MM:SS.EEEJJJ",
        "YYYY-mm-DDTHH:MM:SS.EEEKKKKKK",
        "YYYY-mm-DDTHH:MM:SS.FFFFFF",
        "YYYY-mm-DDTHH:MM:SS.FFFFFFz",
        "YYYY-mm-DDTHH:MM:SS.FFFFFFJJJ",
        "YYYY-mm-DDTHH:MM:SS.FFFFFFKKKKKK",
        "YYYY-mm-DDTHH:MM:SS.GGGGGGGGG",
        "YYYY-mm-DDTHH:MM:SS.GGGGGGGGGz",
        "YYYY-mm-DDTHH:MM:SS.GGGGGGGGGJJJ",
        "YYYY-mm-DDTHH:MM:SS.GGGGGGGGGKKKKKK",
    };

    inline static const auto patternElements = std::vector<std::string>{
        "YYYY", "mm", "DD", "HH", "MM", "SS", "EEE", "FFFFFF", "GGGGGGGGG", "JJJ", "KKKKKK"
    };

    static constexpr int64_t cErrorValue = std::numeric_limits<int64_t>::max();

    inline static const auto patternReplacements = std::map<std::string, Replacements>{
        {
            "YYYY", {
                Part::Year,
                "2025",
                2025,
                {
                    {"0001", 1},
                    {"1234", 1234},
                    {"2026", 2026},
                    {"9999", 9999},
                }
            }
        },
        {
            "mm", {
                Part::Month,
                "04",
                4,
                {
                    {"01", 1},
                    {"09", 9},
                    {"12", 12},
                    {"00", cErrorValue},
                    {"13", cErrorValue},
                }
            }
        },
        {
            "DD", {
                Part::Day,
                "21",
                21,
                {
                    {"01", 1},
                    {"17", 17},
                    {"30", 30},
                    {"00", cErrorValue},
                    {"32", cErrorValue},
                }
            }
        },
        {
            "HH", {
                Part::Hour,
                "19",
                19,
                {
                    {"00", 0},
                    {"23", 23},
                    {"24", cErrorValue},
                }
            }
        },
        {
            "MM", {
                Part::Minute,
                "37",
                37,
                {
                    {"00", 0},
                    {"59", 59},
                    {"60", cErrorValue},
                }
            }
        },
        {
            "SS", {
                Part::Second,
                "03",
                3,
                {
                    {"00", 0},
                    {"59", 59},
                    {"60", cErrorValue},
                }
            }
        },
        {
            "EEE", {
                Part::Nanosecond,
                "293",
                293'000'000,
                {
                    {"000", 0},
                    {"009", 9'000'000},
                    {"900", 900'000'000},
                    {"283", 283'000'000},
                    {"999", 999'000'000},
                }
            }
        },
        {
            "FFFFFF", {
                Part::Nanosecond,
                "092783",
                92'783'000,
                {
                    {"000000", 0},
                    {"000009", 9'000},
                    {"000090", 90'000},
                    {"293283", 293'283'000},
                    {"999999", 999'999'000},
                }
            }
        },
        {
            "GGGGGGGGG", {
                Part::Nanosecond,
                "187652813",
                187'652'813,
                {
                    {"000000000", 0},
                    {"000000009", 9},
                    {"000000090", 90},
                    {"270164589", 270'164'589},
                    {"999999999", 999'999'999},
                }
            }
        },
        {
            "JJJ", {
                Part::TimeZoneHour,
                "+02",
                2,
                {
                    {"+00", 0},
                    {"+19", 19},
                    {"+23", 23},
                    {"+24", cErrorValue},
                    {"-00", -0},
                    {"-19", -19},
                    {"-23", -23},
                    {"-24", cErrorValue},
                }
            }
        },
        {
            "KKKKKK", {
                Part::TimeZoneSeconds,
                "-02:30",
                -2 * 3600 - 30 * 60,
                {
                    {"+00:00", 0 * 60},
                    {"+00:59", 59 * 60},
                    {"+00:59", 59 * 60},
                    {"+23:59", 23 * 3600 + 59 * 60},
                    {"+23:59", 23 * 3600 + 59 * 60},
                    {"-00:00", -0 * 60},
                    {"-00:59", -59 * 60},
                    {"-23:59", -23 * 3600 - 59 * 60},
                    {"+00:60", cErrorValue},
                    {"-00:60", cErrorValue},
                    {"+24:00", cErrorValue},
                    {"-24:00", cErrorValue},
                }
            }
        },
    };

    static auto iteratorsForPattern(const std::string &pattern) -> PatternIterators {
        PatternIterators iterators;
        for (const auto &patterElement : patternElements) {
            if (const auto pos = pattern.find(patterElement); pos != std::string::npos) {
                iterators.emplace_back(
                    PatternIterator{
                        pos,
                        patterElement.size(),
                        0,
                        patternReplacements.at(patterElement)
                    }
                );
            }
        }
        return iterators;
    }

    static void incrementIterators(PatternIterators &iterators) {
        for (auto &iterator : iterators) {
            if (iterator.iteration < iterator.replacements.values.size()) {
                iterator.iteration += 1;
                return;
            }
        }
    }

    static bool iteratorsAtEnd(const PatternIterators &iterators) {
        return iterators.back().iteration == iterators.back().replacements.values.size() - 1;
    }

    static bool hasErrorInIterators(const PatternIterators &iterators) {
        for (const auto &iterator : iterators) {
            if (iterator.iteration >= iterator.replacements.values.size()) {
                continue;
            }
            if (iterator.replacements.values.at(iterator.iteration).value == cErrorValue) {
                return true;
            }
        }
        return false;
    }

    static auto createTextFromIterators(const std::string &pattern, PatternIterators &iterators) -> std::string {
        auto text = pattern;
        for (const auto &[index, length, iteration, replacements] : iterators) {
            std::string replacementText;
            if (iteration >= replacements.values.size()) {
                replacementText = replacements.defaultText;
            } else {
                replacementText = replacements.values.at(iteration).text;
            }
            text.replace(index, length, replacementText);
        }
        return text;
    }

    auto updateDateTimePart(const DateTime &dateTime, Part part, int64_t value) -> DateTime {
        auto year = dateTime.date().year();
        auto month = dateTime.date().month();
        auto day = dateTime.date().day();
        auto hour = dateTime.time().hour();
        auto minute = dateTime.time().minute();
        auto second = dateTime.time().second();
        auto nanosecond = dateTime.time().secondFraction();
        auto offset = dateTime.time().offset();
        switch (part) {
            case Part::Year: year = static_cast<int>(value); break;
            case Part::Month: month = static_cast<int>(value); break;
            case Part::Day: day = static_cast<int>(value); break;
            case Part::Hour: hour = static_cast<int>(value); break;
            case Part::Minute: minute = static_cast<int>(value); break;
            case Part::Second: second = static_cast<int>(value); break;
            case Part::Nanosecond: nanosecond = std::chrono::nanoseconds{value}; break;
            case Part::TimeZoneHour:
                offset = TimeOffset{value < 0, std::abs(static_cast<int>(value)), offset.minutes(), 0};
                break;
            case Part::TimeZoneSeconds:
                offset = TimeOffset{static_cast<int>(value)};
                break;
        default:
            break;
        }
        auto newDate = Date{year, month, day};
        auto newTime = Time{hour, minute, second, static_cast<int>(nanosecond.count()), offset};
        return DateTime{newDate, newTime};
    }

    auto dateTimeForTextAndIterator(const std::string &valueText, const PatternIterators &iterators) -> DateTime {
        DateTime dateTime{Date{1, 1, 1}, Time{0, 0, 0, 0, TimeOffset{}}}; // localtime
        if (valueText.find_first_of("zZ") != std::string::npos) {
            dateTime = DateTime{Date{1, 1, 1}, Time{0, TimeOffset::utc()}}; // UTC
        }
        for (const auto &iterator : iterators) {
            const auto part = iterator.replacements.part;
            int64_t value;
            if (iterator.iteration >= iterator.replacements.values.size()) {
                value = iterator.replacements.defaultValue;
            } else {
                value = iterator.replacements.values.at(iterator.iteration).value;
            }
            dateTime = updateDateTimePart(dateTime, part, value);
        }
        return dateTime;
    }

    void verifyDateTimeWithPattern(const std::string &pattern) {
        auto iterators = iteratorsForPattern(pattern);
        std::string valueText;
        DateTime expectedDateTime;
        try {
            while (!iteratorsAtEnd(iterators)) {
                valueText = createTextFromIterators(pattern, iterators);
                if (hasErrorInIterators(iterators)) {
                    verifyErrorInValue(String{valueText}, ErrorCategory::Syntax);
                } else {
                    expectedDateTime = dateTimeForTextAndIterator(valueText, iterators);
                    verifyValidValue(String{valueText}, TokenType::DateTime, expectedDateTime);
                }
                incrementIterators(iterators);
            }
        } catch (const el::AssertFailed&) {
            consoleWriteLine(std::format("valueText={} expectedDateTime={}", valueText, expectedDateTime));
            throw;
        }
    }

    auto buildTestList() -> TestDataList {
        TestDataList testDataList;
        for (const auto &pattern : dateTimePatterns) {
            auto iterators = iteratorsForPattern(pattern);
            while (!iteratorsAtEnd(iterators)) {
                const auto valueText = createTextFromIterators(pattern, iterators);
                if (hasErrorInIterators(iterators)) {
                    testDataList.emplace_back(TestData{pattern, valueText, true, {}});
                } else {
                    auto expectedTime = dateTimeForTextAndIterator(valueText, iterators);
                    testDataList.emplace_back(TestData{pattern, valueText, false, expectedTime});
                }
                incrementIterators(iterators);
            }
        }
        return testDataList;
    }

    auto verifyValueAsync(const TestData &testData) -> AsyncTestResult {
        String doc;
        doc.reserve(250);
        doc.append(cFastPrefix);
        doc.append(testData.valueText);
        doc.append(cFastSuffix);
        auto source = Source::fromString(doc);
        source->open();
        auto decoder = CharStream::create(source);
        auto lexer = Lexer::create(decoder);
        auto tokens = lexer->tokens();
        auto tokenIterator = tokens.begin();
        auto tokenIteratorEnd = tokens.end();
        auto requireNextToken = [&](TokenType expectedTokenType) {
            if (tokenIterator == tokenIteratorEnd) {
                throw std::runtime_error("Unexpected end of token iterator");
            }
            auto token = *tokenIterator;
            if (token.type() != expectedTokenType) {
                throw std::runtime_error("Unexpected token type");
            }
            ++tokenIterator;
        };
        requireNextToken(TokenType::SectionMapOpen);
        requireNextToken(TokenType::RegularName);
        requireNextToken(TokenType::SectionMapClose);
        requireNextToken(TokenType::LineBreak);
        requireNextToken(TokenType::RegularName);
        requireNextToken(TokenType::NameValueSeparator);
        requireNextToken(TokenType::Spacing);
        try {
            auto valueToken = *tokenIterator;
            ++tokenIterator;
            if (testData.expectError) {
                return {
                    .success=false,
                    .errorMessage="Expected error, but got a regular token."};
            }
            if (valueToken.type() != TokenType::DateTime) {
                return {
                    .success=false,
                    .errorMessage="Expected time token, but got something else."};
            }
            auto actualDateTime = std::get<DateTime>(valueToken.content());
            requireNextToken(TokenType::LineBreak);
            return {
                .success=true,
                .actualDateTime=actualDateTime};
        } catch (const Error &error) {
            if (testData.expectError) {
                return {
                    .success=true,
                    .errorMessage=error.what()};
            }
            return {
                .success=false,
                .errorMessage=error.what()};
        }
    }

    void testDateTimeFast() {
        const auto testDataList = buildTestList();
        std::vector<RunningTest> runningTests;
        for (auto const &testData : testDataList) {
            runningTests.emplace_back(testData, std::async(std::launch::async, [this, &testData]{
                try {
                    return verifyValueAsync(testData);
                } catch (const std::runtime_error &e) {
                    return AsyncTestResult{.success=false, .actualDateTime={}, .errorMessage=e.what()};
                }
            }));
        }
        for (auto &runningTest : runningTests) {
            auto testResult = runningTest.future.get();
            if (!testResult.success) {
                consoleWriteLine(std::format(
                    "Test failed: pattern={} valueText={} expectedError={} expected={} actual={} error={}",
                    runningTest.testData.pattern,
                    runningTest.testData.valueText,
                    runningTest.testData.expectError,
                    runningTest.testData.expectedDateTime.toText().toCharString(),
                    testResult.actualDateTime.toText().toCharString(),
                    testResult.errorMessage));
            }
            REQUIRE(testResult.success);
            if (!runningTest.testData.expectError) {
                REQUIRE(runningTest.testData.expectedDateTime == testResult.actualDateTime);
            }
        }
    }

    SKIP_BY_DEFAULT() TAGS(Slow)
    void testDateTimeSlow() {
        // Slow and sequential test for debugging.
        for (const auto &dateTimePattern : dateTimePatterns) {
            try {
                verifyDateTimeWithPattern(dateTimePattern);
            } catch (el::AssertFailed&) {
                consoleWriteLine(std::format("pattern={}", dateTimePattern));
                throw;
            }
        }
    }
};


