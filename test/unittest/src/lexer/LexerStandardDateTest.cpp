// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"

#include <thread>
#include <future>


TESTED_TARGETS(Lexer) TAGS(Date)
class LexerStandardDateTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    enum class Part : uint8_t {
        Year,
        Month,
        Day,
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
        Date expectedDate;
    };
    using TestDataList = std::vector<TestData>;

    struct AsyncTestResult {
        bool success = false;
        Date actualDate;
        std::string errorMessage;
    };

    struct RunningTest {
        TestData testData;
        std::future<AsyncTestResult> future;
    };

    inline static const auto datePatterns = std::vector<std::string>{
        "YYYY-MM-DD"
    };

    inline static const auto patternElements = std::vector<std::string>{
        "YYYY", "MM", "DD"
    };

    static constexpr int64_t cErrorValue = std::numeric_limits<int64_t>::max();

    inline static const auto patternReplacements = std::map<std::string, Replacements>{
        {
            "YYYY", {
                Part::Year, {
                    {"0001", 1},
                    {"1970", 1970},
                    {"2026", 2026},
                    {"9999", 9999},
                    {"0000", cErrorValue},
                }
            }
        },
        {
            "MM", {
                Part::Month, {
                    {"01", 1},
                    {"12", 12},
                    {"13", cErrorValue},
                    {"99", cErrorValue},
                }
            }
        },
        {
            "DD", {
                Part::Day, {
                    {"01", 1},
                    {"14", 14},
                    {"30", 30},
                    {"32", cErrorValue},
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

    auto updateDatePart(const Date &date, Part part, int64_t value) -> Date {
        auto year = date.year();
        auto month = date.month();
        auto day = date.day();
        switch (part) {
        case Part::Year: year = static_cast<int>(value); break;
        case Part::Month: month = static_cast<int>(value); break;
        case Part::Day: day = static_cast<int>(value); break;
        default:
            break;
        }
        return Date{year, month, day};
    }

    auto dateForTextAndIterator(const std::string &valueText, PatternIterators &iterators) -> Date {
        Date date{1, 1, 1};
        for (const auto &iterator : iterators) {
            const auto part = iterator.replacements.part;
            const auto value = iterator.replacements.values.at(iterator.iteration).value;
            date = updateDatePart(date, part, value);
        }
        return date;
    }

    void verifyDateWithPattern(const std::string &pattern) {
        auto iterators = iteratorsForPattern(pattern);
        std::string valueText;
        Date expectedDate;
        try {
            while (!iteratorsAtEnd(iterators)) {
                valueText = createTextFromIterators(pattern, iterators);
                if (hasErrorInIterators(iterators)) {
                    verifyErrorInValue(String{valueText}, ErrorCategory::Syntax);
                } else {
                    expectedDate = dateForTextAndIterator(valueText, iterators);
                    verifyValidValue(String{valueText}, TokenType::Date, expectedDate);
                }
                incrementIterators(iterators);
            }
        } catch (const el::AssertFailed&) {
            consoleWriteLine(std::format("valueText={} expectedDate={}", valueText, expectedDate));
            throw;
        }
    }

    auto buildTestList() -> TestDataList {
        TestDataList testDataList;
        for (const auto &pattern : datePatterns) {
            auto iterators = iteratorsForPattern(pattern);
            while (!iteratorsAtEnd(iterators)) {
                const auto valueText = createTextFromIterators(pattern, iterators);
                if (hasErrorInIterators(iterators)) {
                    testDataList.emplace_back(TestData{pattern, valueText, true, {}});
                } else {
                    auto expectedDate = dateForTextAndIterator(valueText, iterators);
                    testDataList.emplace_back(TestData{pattern, valueText, false, expectedDate});
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
            if (valueToken.type() != TokenType::Date) {
                return {
                    .success=false,
                    .errorMessage="Expected date token, but got something else."};
            }
            auto actualDate = std::get<Date>(valueToken.content());
            requireNextToken(TokenType::LineBreak);
            return {
                .success=true,
                .actualDate=actualDate};
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
                    return AsyncTestResult{.success=false, .actualDate={}, .errorMessage=e.what()};
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
                    runningTest.testData.expectedDate.toText().toCharString(),
                    testResult.actualDate.toText().toCharString(),
                    testResult.errorMessage));
            }
            REQUIRE(testResult.success);
            if (!runningTest.testData.expectError) {
                REQUIRE(runningTest.testData.expectedDate == testResult.actualDate);
            }
        }
    }

    void testEdgeCases() {
        // min/max
        verifyValidDate(u8"0001-01-01", Date{1, 1, 1});
        verifyValidDate(u8"9999-12-31", Date{9999, 12, 31});
        verifyErrorInValue(u8"0000-00-00", ErrorCategory::Syntax);

        // tiny-year leap check (0004 is a leap year in proleptic Gregorian)
        verifyValidDate(u8"0004-02-29", Date{4, 2, 29});
        verifyErrorInValue(u8"0001-02-29", ErrorCategory::Syntax);

        // typical modern-leap years
        verifyValidDate(u8"2000-02-29", Date{2000, 2, 29});      // divisible by 400
        verifyValidDate(u8"2004-02-29", Date{2004, 2, 29});      // divisible by 4
        verifyErrorInValue(u8"2001-02-29", ErrorCategory::Syntax); // non-leap
        verifyErrorInValue(u8"2019-02-29", ErrorCategory::Syntax);

        // century edge cases
        verifyErrorInValue(u8"1900-02-29", ErrorCategory::Syntax); // divisible by 100, not by 400
        verifyErrorInValue(u8"2100-02-29", ErrorCategory::Syntax);
        verifyValidDate(u8"2400-02-29", Date{2400, 2, 29});      // divisible by 400

        // February bounds
        verifyValidDate(u8"2019-02-28", Date{2019, 2, 28});
        verifyErrorInValue(u8"2020-02-30", ErrorCategory::Syntax);

        // month out of range
        verifyErrorInValue(u8"2020-00-10", ErrorCategory::Syntax);
        verifyErrorInValue(u8"2020-13-01", ErrorCategory::Syntax);
    }

    SKIP_BY_DEFAULT() TAGS(Slow)
    void testDateSlow() {
        for (const auto &datePattern : datePatterns) {
            try {
                verifyDateWithPattern(datePattern);
            } catch (el::AssertFailed&) {
                consoleWriteLine(std::format("pattern={}", datePattern));
                throw;
            }
        }
    }
};


