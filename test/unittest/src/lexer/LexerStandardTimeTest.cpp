// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"

#include <thread>
#include <future>


TESTED_TARGETS(Lexer) TAGS(Time)
class LexerStandardTimeTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    enum class Part : uint8_t {
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
        Time expectedTime;
    };
    using TestDataList = std::vector<TestData>;

    struct AsyncTestResult {
        bool success = false;
        Time actualTime;
        std::string errorMessage;
    };

    struct RunningTest {
        TestData testData;
        std::future<AsyncTestResult> future;
    };

    inline static const auto timePatterns = std::vector<std::string>{
        "HH:MM",
        "HH:MMz",
        "HH:MMZ",
        "HH:MMJJJ",
        "HH:MMKKKKKK",
        "HH:MM:SS",
        "HH:MM:SSz",
        "HH:MM:SSZ",
        "HH:MM:SSJJJ",
        "HH:MM:SSKKKKKK",
        "HH:MM:SS.EEE",
        "HH:MM:SS.EEEz",
        "HH:MM:SS.EEEZ",
        "HH:MM:SS.EEEJJJ",
        "HH:MM:SS.EEEKKKKKK",
        "HH:MM:SS.FFFFFF",
        "HH:MM:SS.FFFFFFz",
        "HH:MM:SS.FFFFFFJJJ",
        "HH:MM:SS.FFFFFFKKKKKK",
        "HH:MM:SS.GGGGGGGGG",
        "HH:MM:SS.GGGGGGGGGz",
        "HH:MM:SS.GGGGGGGGGJJJ",
        "HH:MM:SS.GGGGGGGGGKKKKKK",
    };

    inline static const auto patternElements = std::vector<std::string>{
        "HH", "MM", "SS", "EEE", "FFFFFF", "GGGGGGGGG", "JJJ", "KKKKKK"
    };

    static constexpr int64_t cErrorValue = std::numeric_limits<int64_t>::max();

    inline static const auto patternReplacements = std::map<std::string, Replacements>{
        {
            "HH", {
                Part::Hour, {
                    {"00", 0},
                    {"23", 23},
                    {"24", cErrorValue},
                }
            }
        },
        {
            "MM", {
                Part::Minute, {
                    {"00", 0},
                    {"59", 59},
                    {"60", cErrorValue},
                }
            }
        },
        {
            "SS", {
                Part::Second, {
                    {"00", 0},
                    {"59", 59},
                    {"60", cErrorValue},
                }
            }
        },
        {
            "EEE", {
                Part::Nanosecond, {
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
                Part::Nanosecond, {
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
                Part::Nanosecond, {
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
                Part::TimeZoneHour, {
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
                Part::TimeZoneSeconds, {
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
            if (iterator.iteration < (iterator.replacements.values.size() - 1)) {
                iterator.iteration += 1;
                return;
            }
            iterator.iteration = 0;
        }
    }

    static bool iteratorsAtEnd(const PatternIterators &iterators) {
        return iterators.back().iteration == iterators.back().replacements.values.size() - 1;
    }

    static bool hasErrorInIterators(const PatternIterators &iterators) {
        for (const auto &iterator : iterators) {
            if (iterator.replacements.values.at(iterator.iteration).value == cErrorValue) {
                return true;
            }
        }
        return false;
    }

    static auto createTextFromIterators(const std::string &pattern, PatternIterators &iterators) -> std::string {
        auto text = pattern;
        for (const auto &iterator : iterators) {
            text.replace(iterator.index, iterator.length, iterator.replacements.values.at(iterator.iteration).text);
        }
        return text;
    }

    auto updateTimePart(const Time &time, Part part, int64_t value) -> Time {
        auto hour = time.hour();
        auto minute = time.minute();
        auto second = time.second();
        auto nanosecond = time.secondFraction();
        auto offset = time.offset();
        switch (part) {
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
        return Time{hour, minute, second, static_cast<int>(nanosecond.count()), offset};
    }

    auto timeForTextAndIterator(const std::string &valueText, PatternIterators &iterators) -> Time {
        Time time{}; // localtime
        if (valueText.find_first_of("zZ") != std::string::npos) {
            time = Time{0, TimeOffset::utc()}; // UTC
        }
        for (const auto &iterator : iterators) {
            const auto part = iterator.replacements.part;
            const auto value = iterator.replacements.values.at(iterator.iteration).value;
            time = updateTimePart(time, part, value);
        }
        return time;
    }

    void verifyTimeWithPattern(const std::string &pattern) {
        auto iterators = iteratorsForPattern(pattern);
        std::string valueText;
        Time expectedTime;
        try {
            while (!iteratorsAtEnd(iterators)) {
                valueText = createTextFromIterators(pattern, iterators);
                if (hasErrorInIterators(iterators)) {
                    verifyErrorInValue(String{valueText}, ErrorCategory::Syntax);
                } else {
                    expectedTime = timeForTextAndIterator(valueText, iterators);
                    verifyValidValueFaster(String{valueText}, TokenType::Time, expectedTime);
                }
                incrementIterators(iterators);
            }
        } catch (const el::AssertFailed&) {
            consoleWriteLine(std::format("valueText={} expectedTime={}", valueText, expectedTime));
            throw;
        }
    }

    auto buildTestList() -> TestDataList {
        TestDataList testDataList;
        for (const auto &pattern : timePatterns) {
            auto iterators = iteratorsForPattern(pattern);
            while (!iteratorsAtEnd(iterators)) {
                const auto valueText = createTextFromIterators(pattern, iterators);
                if (hasErrorInIterators(iterators)) {
                    testDataList.emplace_back(TestData{pattern, valueText, true, {}});
                } else {
                    auto expectedTime = timeForTextAndIterator(valueText, iterators);
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
            if (valueToken.type() != TokenType::Time) {
                return {
                    .success=false,
                    .errorMessage="Expected time token, but got something else."};
            }
            auto actualTime = std::get<Time>(valueToken.content());
            requireNextToken(TokenType::LineBreak);
            return {
                .success=true,
                .actualTime=actualTime};
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

    void testTimeFast() {
        const auto testDataList = buildTestList();
        std::vector<RunningTest> runningTests;
        for (auto const &testData : testDataList) {
            runningTests.emplace_back(testData, std::async(std::launch::async, [this, &testData]{
                try {
                    return verifyValueAsync(testData);
                } catch (const std::runtime_error &e) {
                    return AsyncTestResult{.success=false, .actualTime={}, .errorMessage=e.what()};
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
                    runningTest.testData.expectedTime.toText().toCharString(),
                    testResult.actualTime.toText().toCharString(),
                    testResult.errorMessage));
            }
            REQUIRE(testResult.success);
            if (!runningTest.testData.expectError) {
                REQUIRE(runningTest.testData.expectedTime == testResult.actualTime);
            }
        }
    }

    SKIP_BY_DEFAULT() TAGS(Slow)
    void testTimeSlow() {
        // Slow and sequential test for debugging.
        // It is slow, because it sequentially tests a wide range of time combinations.
        for (const auto &timePattern : timePatterns) {
            try {
                verifyTimeWithPattern(timePattern);
            } catch (el::AssertFailed&) {
                consoleWriteLine(std::format("pattern={}", timePattern));
                throw;
            }
        }
    }
};


