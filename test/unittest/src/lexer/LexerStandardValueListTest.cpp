// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(ValueList)
class LexerStandardValueListTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:

    enum class ListStyle : uint8_t {
        Compact,
        SpaceAfterComma,
        SpaceBeforeAndAfterComma,
        MultiLine,
    };

    using LexerValueTestHelper::to_string;
    static auto to_string(const ListStyle listStyle) -> std::string {
        switch (listStyle) {
        case ListStyle::Compact:
            return "Compact";
        case ListStyle::SpaceAfterComma:
            return "SpaceAfterComma";
        case ListStyle::SpaceBeforeAndAfterComma:
            return "SpaceBeforeAndAfterComma";
        case ListStyle::MultiLine:
            return "MultiLine";
        }
        return {};
    }

    static constexpr auto cListStyles = std::array<ListStyle, 4>{
        ListStyle::Compact,
        ListStyle::SpaceAfterComma,
        ListStyle::SpaceBeforeAndAfterComma,
        ListStyle::MultiLine,
    };

    struct TestValue {
        String valueText;
        TokenType expectedTokenType;
        bool isList = false;
    };

    using TestValueList = std::vector<TestValue>;

    auto createValueText(const TestValueList &testValueList, ListStyle listStyle, PrefixFormat prefixFormat) -> String {
        String result;
        for (const auto &testValue : testValueList) {
            if (!result.empty()) {
                switch (listStyle) {
                case ListStyle::Compact:
                    result += u8",";
                    break;
                case ListStyle::SpaceAfterComma:
                    result += u8", ";
                    break;
                case ListStyle::SpaceBeforeAndAfterComma:
                    result += u8" , ";
                    break;
                case ListStyle::MultiLine:
                    break;
                default:
                    throw std::logic_error("List style not implemented");
                }
            }
            if (listStyle == ListStyle::MultiLine) {
                if (!result.empty()) {
                    result += u8"\n";
                    result += indentForPrefix(prefixFormat);
                }
                result += u8"* ";
            }
            result += testValue.valueText;
        }
        return result;
    }

    void verifyListTokens(const TestValueList &testValueList, ListStyle listStyle, PrefixFormat prefixFormat) {
        for (std::size_t i = 0; i < testValueList.size(); ++i) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                const auto &[valueText, expectedTokenType, isList] = testValueList[i];
                if (listStyle == ListStyle::MultiLine) {
                    WITH_CONTEXT(requireNextToken(TokenType::MultiLineValueListSeparator, u8"*"));
                    if (isList) {
                        // This is a list of lists - and for the test we use three values of the same type.
                        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
                        WITH_CONTEXT(requireNextToken(expectedTokenType));
                        WITH_CONTEXT(requireNextToken(TokenType::ValueListSeparator, u8","));
                        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
                        WITH_CONTEXT(requireNextToken(expectedTokenType));
                        WITH_CONTEXT(requireNextToken(TokenType::ValueListSeparator, u8","));
                        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
                        WITH_CONTEXT(requireNextToken(expectedTokenType));
                    } else {
                        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
                        WITH_CONTEXT(requireNextToken(expectedTokenType, valueText));
                    }
                    if (i < testValueList.size() - 1) {
                        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
                        WITH_CONTEXT(requireNextToken(TokenType::Indentation, indentForPrefix(prefixFormat)));
                    }
                } else {
                    WITH_CONTEXT(requireNextToken(expectedTokenType, valueText));
                    if (i < testValueList.size() - 1) {
                        switch (listStyle) {
                        case ListStyle::Compact:
                            WITH_CONTEXT(requireNextToken(TokenType::ValueListSeparator, u8","));
                            break;
                        case ListStyle::SpaceAfterComma:
                            WITH_CONTEXT(requireNextToken(TokenType::ValueListSeparator, u8","));
                            WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
                            break;
                        case ListStyle::SpaceBeforeAndAfterComma:
                            WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
                            WITH_CONTEXT(requireNextToken(TokenType::ValueListSeparator, u8","));
                            WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
                            break;
                        default:
                            throw std::logic_error("List style not implemented");
                        }
                    }
                }
            }, [&]() {
                return std::format("i == {}", i);
            });
        }
    }

    void verifyValidValueList(const TestValueList &testValueList) {
        for (auto listStyle : cListStyles) {
            for (auto prefixFormat : cPrefixFormats) {
                if (listStyle == ListStyle::MultiLine && prefixFormat == PrefixFormat::SameLine) {
                    continue;
                }
                for (auto suffixFormat : cSuffixPatterns) {
                    runWithContext(SOURCE_LOCATION(), [&]() {
                        auto valueText = createValueText(testValueList, listStyle, prefixFormat);
                        setupTokenIteratorForValueTest(valueText, prefixFormat, suffixFormat);
                        WITH_CONTEXT(verifyPrefix(prefixFormat));
                        WITH_CONTEXT(verifyListTokens(testValueList, listStyle, prefixFormat));
                        WITH_CONTEXT(verifySuffix(suffixFormat));
                    }, [&]() {
                        return std::format("Failed at: suffix={} prefix={}, listStyle={}",
                            to_string(suffixFormat), to_string(prefixFormat), to_string(listStyle));
                    });
                }
            }
        }
    }

    void verifyListOfLists(const TestValueList &testValueList) {
        constexpr auto listStyle = ListStyle::MultiLine;
        constexpr auto prefixFormat = PrefixFormat::NextLinePattern1;
        for (auto suffixFormat : cSuffixPatterns) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                auto valueText = createValueText(testValueList, listStyle, prefixFormat);
                setupTokenIteratorForValueTest(valueText, prefixFormat, suffixFormat);
                WITH_CONTEXT(verifyPrefix(prefixFormat));
                WITH_CONTEXT(verifyListTokens(testValueList, listStyle, prefixFormat));
                WITH_CONTEXT(verifySuffix(suffixFormat));
            }, [&]() {
                return std::format("Failed at: suffix={} prefix={}, listStyle={}",
                    to_string(suffixFormat), to_string(prefixFormat), to_string(listStyle));
            });
        }
    }

    void testIntegerList() {
        const auto testValueList = TestValueList{
            {
                .valueText = u8"1",
                .expectedTokenType = TokenType::Integer
            },
            {
                .valueText = u8"2",
                .expectedTokenType = TokenType::Integer
            },
            {
                .valueText = u8"3",
                .expectedTokenType = TokenType::Integer
            }
        };
        verifyValidValueList(testValueList);
    }

    void testTextList() {
        const auto testValueList = TestValueList{
            {
                .valueText = u8"\"one\"",
                .expectedTokenType = TokenType::Text
            },
            {
                .valueText = u8"\"two\"",
                .expectedTokenType = TokenType::Text
            },
            {
                .valueText = u8"\"three\"",
                .expectedTokenType = TokenType::Text
            }
        };
        verifyValidValueList(testValueList);
    }

    void testBooleanList() {
        const auto testValueList = TestValueList{
            {
                .valueText = u8"true",
                .expectedTokenType = TokenType::Boolean
            },
            {
                .valueText = u8"off",
                .expectedTokenType = TokenType::Boolean
            },
            {
                .valueText = u8"enabled",
                .expectedTokenType = TokenType::Boolean
            }
        };
        verifyValidValueList(testValueList);
    }

    void testMixedList() {
        const auto testValueList = TestValueList{
            {
                .valueText = u8"192.21",
                .expectedTokenType = TokenType::Float
            },
            {
                .valueText = u8"false",
                .expectedTokenType = TokenType::Boolean
            },
            {
                .valueText = u8"\"text\"",
                .expectedTokenType = TokenType::Text
            }
        };
        verifyValidValueList(testValueList);
    }

    void testListOfLists() {
        const auto testValueList = TestValueList{
            {
                .valueText = u8"1, 2, 3",
                .expectedTokenType = TokenType::Integer,
                .isList = true
            },
            {
                .valueText = u8"\"one\", \"two\", \"three\"",
                .expectedTokenType = TokenType::Text,
                .isList = true
            },
            {
                .valueText = u8"true, false, enabled",
                .expectedTokenType = TokenType::Boolean,
                .isList = true
            },
            {
                .valueText = u8"192.21, 2.21, 0.0",
                .expectedTokenType = TokenType::Float,
                .isList = true
            }
        };
        verifyListOfLists(testValueList);
    }
};


