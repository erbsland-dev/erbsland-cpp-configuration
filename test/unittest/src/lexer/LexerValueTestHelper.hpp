// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "LexerTestHelper.hpp"


class LexerValueTestHelper : public LexerTestHelper {
public:
    inline static const auto cSectionLine = String{u8"[section]\n"};
    inline static const auto cValueStart = String{u8"value:"};
    inline static const auto cValueOnSameLineSpacing = String{u8" "};
    inline static const auto cValueOnNextLineSpacing = String{u8"\n"};
    inline static const auto cSimpleComment = String{u8"# comment"};
    inline static const auto cValueOnNextLineIndentationPattern1 = String{u8"    "};
    inline static const auto cValueOnNextLineIndentationPattern2 = String{u8"\t"};
    inline static const auto cValueOnNextLineIndentationPattern3 = String{u8" \t \t"};
    inline static const auto cFastPrefix = String{u8"[section]\nvalue: "};
    inline static const auto cFastSuffix = String{u8"\n"};

    enum class PrefixFormat : uint8_t {
        SameLine,
        NextLinePattern1,
        NextLinePattern2,
        NextLinePattern3,
        NextLinePattern1withComment,
    };

    static auto to_string(const PrefixFormat prefixFormat) -> std::string {
        switch (prefixFormat) {
            case PrefixFormat::SameLine:
                return "SameLine";
            case PrefixFormat::NextLinePattern1:
                return "NextLinePattern1";
            case PrefixFormat::NextLinePattern2:
                return "NextLinePattern2";
            case PrefixFormat::NextLinePattern3:
                return "NextLinePattern3";
            case PrefixFormat::NextLinePattern1withComment:
                return "NextLinePattern1withComment";
        }
        return {};
    }

    static constexpr auto cPrefixFormats = std::array<PrefixFormat, 5>{
        PrefixFormat::SameLine,
        PrefixFormat::NextLinePattern1,
        PrefixFormat::NextLinePattern2,
        PrefixFormat::NextLinePattern3,
        PrefixFormat::NextLinePattern1withComment,
    };

    enum class SuffixFormat : uint8_t {
        EndOfDocument,
        LineBreak,
        Comment,
        SpaceAndComment
    };

    static auto to_string(const SuffixFormat suffixFormat) -> std::string {
        switch (suffixFormat) {
            case SuffixFormat::EndOfDocument:
                return "EndOfDocument";
            case SuffixFormat::LineBreak:
                return "LineBreak";
            case SuffixFormat::Comment:
                return "Comment";
            case SuffixFormat::SpaceAndComment:
                return "SpaceAndComment";
        }
        return {};
    }

    static constexpr auto cSuffixPatterns = std::array<SuffixFormat, 4>{
        SuffixFormat::EndOfDocument,
        SuffixFormat::LineBreak,
        SuffixFormat::Comment,
        SuffixFormat::SpaceAndComment
    };

    static void documentPrefix(String &doc, PrefixFormat prefixFormat) {
        doc.reserve(1024);
        doc += cSectionLine;
        doc += cValueStart;
        switch (prefixFormat) {
        case PrefixFormat::SameLine:
            doc += cValueOnSameLineSpacing;
            break;
        case PrefixFormat::NextLinePattern1:
            doc += cValueOnNextLineSpacing;
            doc += cValueOnNextLineIndentationPattern1;
            break;
        case PrefixFormat::NextLinePattern2:
            doc += cValueOnNextLineSpacing;
            doc += cValueOnNextLineIndentationPattern2;
            break;
        case PrefixFormat::NextLinePattern3:
            doc += cValueOnNextLineSpacing;
            doc += cValueOnNextLineIndentationPattern3;
            break;
        case PrefixFormat::NextLinePattern1withComment:
            doc += u8" ";
            doc += cSimpleComment;
            doc += cValueOnNextLineSpacing;
            doc += cValueOnNextLineIndentationPattern1;
            break;
        default:
            throw std::logic_error("Invalid prefix format.");
        }
    }

    static void documentSuffix(String &doc, const SuffixFormat suffixFormat) {
        switch (suffixFormat) {
        case SuffixFormat::EndOfDocument:
            break;
        case SuffixFormat::LineBreak:
            doc.append(u8"\n");
            break;
        case SuffixFormat::Comment:
            doc.append(cSimpleComment);
            break;
        case SuffixFormat::SpaceAndComment:
            doc.append(u8" ");
            doc.append(cSimpleComment);
            break;
        default:
            throw std::logic_error("Invalid suffix format.");
        }
    }

    void setupTokenIteratorForValueTest(const String &valueText, const PrefixFormat prefixFormat, const SuffixFormat suffixFormat) {
        String doc;
        doc.reserve(250);
        documentPrefix(doc, prefixFormat);
        doc.append(valueText);
        documentSuffix(doc, suffixFormat);
        setupTokenIterator(doc);
    }

    void setupTokenIteratorForMassValueTest(const String &valueText) {
        String doc = cFastPrefix;
        doc.append(valueText);
        doc.append(cFastSuffix);
        setupTokenIteratorFast(doc);
    }

    static auto indentForPrefix(PrefixFormat prefixFormat) -> String {
        switch (prefixFormat) {
        case PrefixFormat::NextLinePattern1:
        case PrefixFormat::NextLinePattern1withComment:
            return cValueOnNextLineIndentationPattern1;
        case PrefixFormat::NextLinePattern2:
            return cValueOnNextLineIndentationPattern2;
        case PrefixFormat::NextLinePattern3:
            return cValueOnNextLineIndentationPattern3;
        default:
            throw std::logic_error("Invalid indentation pattern.");
        }
    }

    void verifyPrefix(PrefixFormat prefixFormat) {
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapOpen, u8"["));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"section", u8"section"));
        WITH_CONTEXT(requireNextToken(TokenType::SectionMapClose, u8"]"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextStringToken(TokenType::RegularName, u8"value", u8"value"));
        WITH_CONTEXT(requireNextToken(TokenType::NameValueSeparator, u8":"));
        switch (prefixFormat) {
        case PrefixFormat::SameLine:
            WITH_CONTEXT(requireNextToken(TokenType::Spacing, cValueOnSameLineSpacing));
            break;
        case PrefixFormat::NextLinePattern1:
        case PrefixFormat::NextLinePattern2:
        case PrefixFormat::NextLinePattern3:
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
            WITH_CONTEXT(requireNextToken(TokenType::Indentation, indentForPrefix(prefixFormat)));
            break;
        case PrefixFormat::NextLinePattern1withComment:
            WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
            WITH_CONTEXT(requireNextToken(TokenType::Comment, cSimpleComment));
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
            WITH_CONTEXT(requireNextToken(TokenType::Indentation, cValueOnNextLineIndentationPattern1));
            break;
        default:
            throw std::logic_error("Invalid indentation pattern.");
        }
    }

    void verifySameLinePrefixFast() {
        requireNextToken(TokenType::SectionMapOpen, u8"[");
        requireNextStringToken(TokenType::RegularName, u8"section", u8"section");
        requireNextToken(TokenType::SectionMapClose, u8"]");
        requireNextToken(TokenType::LineBreak, u8"\n");
        requireNextStringToken(TokenType::RegularName, u8"value", u8"value");
        requireNextToken(TokenType::NameValueSeparator, u8":");
        requireNextToken(TokenType::Spacing, cValueOnSameLineSpacing);
    }

    void verifySuffix(SuffixFormat suffixFormat) {
        switch (suffixFormat) {
        case SuffixFormat::EndOfDocument:
            WITH_CONTEXT(requireEndOfData());
            break;
        case SuffixFormat::LineBreak:
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
            WITH_CONTEXT(requireEndOfData());
            break;
        case SuffixFormat::Comment:
            WITH_CONTEXT(requireNextToken(TokenType::Comment, cSimpleComment));
            WITH_CONTEXT(requireEndOfData());
            break;
        case SuffixFormat::SpaceAndComment:
            WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
            WITH_CONTEXT(requireNextToken(TokenType::Comment, cSimpleComment));
            WITH_CONTEXT(requireEndOfData());
        }
    }

    void verifyNewLineSuffixFast() {
        requireNextToken(TokenType::LineBreak, u8"\n");
        requireEndOfData();
    }

    template<typename T>
    void verifyValidValue(const String &valueText, TokenType tokenType, const T &expectedValue) {
        for (auto prefixFormat : cPrefixFormats) {
            for (auto suffixFormat : cSuffixPatterns) {
                setupTokenIteratorForValueTest(valueText, prefixFormat, suffixFormat);
                WITH_CONTEXT(verifyPrefix(prefixFormat));
                WITH_CONTEXT(requireNextValueToken<T>(tokenType, expectedValue, valueText));
                WITH_CONTEXT(verifySuffix(suffixFormat));
            }
        }
    }

    template<typename T>
    void verifyValidValueFaster(const String &valueText, const TokenType tokenType, const T &expectedValue) {
        setupTokenIteratorForMassValueTest(valueText);
        verifySameLinePrefixFast();
        requireNextValueToken<T>(tokenType, expectedValue, valueText);
        verifyNewLineSuffixFast();
    }

    void verifyValidInteger(const String &valueText, const Integer &expectedValue) {
        verifyValidValue<Integer>(valueText, TokenType::Integer, expectedValue);
    }

    void verifyValidBoolean(const String &valueText, const bool &expectedValue) {
        verifyValidValue<bool>(valueText, TokenType::Boolean, expectedValue);
    }

    void verifyValidText(const String &valueText, const String &expectedValue) {
        verifyValidValue<String>(valueText, TokenType::Text, expectedValue);
    }

    void verifyValidCode(const String &valueText, const String &expectedValue) {
        verifyValidValue<String>(valueText, TokenType::Code, expectedValue);
    }

    void verifyValidRegEx(const String &valueText, const String &expectedValue) {
        verifyValidValue<String>(valueText, TokenType::RegEx, expectedValue);
    }

    void verifyValidFloat(const String &valueText, const Float &expectedValue) {
        verifyValidValue<Float>(valueText, TokenType::Float, expectedValue);
    }

    void verifyValidTime(const String &valueText, const Time &expectedValue) {
        verifyValidValue<Time>(valueText, TokenType::Time, expectedValue);
    }

    void verifyValidDate(const String &valueText, const Date &expectedValue) {
        verifyValidValue<Date>(valueText, TokenType::Date, expectedValue);
    }

    void verifyValidDateTime(const String &valueText, const DateTime &expectedValue) {
        verifyValidValue<DateTime>(valueText, TokenType::DateTime, expectedValue);
    }

    void verifyValidTimeDelta(const String &valueText, const TimeDelta &expectedValue) {
        verifyValidValue<TimeDelta>(valueText, TokenType::TimeDelta, expectedValue);
    }

    void verifyValidByteData(const String &valueText, const Bytes &expectedValue) {
        verifyValidValue<Bytes>(valueText, TokenType::Bytes, expectedValue);
    }

    void verifyErrorInValue(const String &valueText, const ErrorCategory expectedError) {
        for (auto prefixFormat : cPrefixFormats) {
            for (auto suffixFormat : cSuffixPatterns) {
                setupTokenIteratorForValueTest(valueText, prefixFormat, suffixFormat);
                WITH_CONTEXT(verifyPrefix(prefixFormat));
                WITH_CONTEXT(requireError(expectedError));
            }
        }
    }

    void verifyErrorInValue(const String &valueText, std::initializer_list<ErrorCategory> expectedErrors) {
        for (auto prefixFormat : cPrefixFormats) {
            for (auto suffixFormat : cSuffixPatterns) {
                setupTokenIteratorForValueTest(valueText, prefixFormat, suffixFormat);
                WITH_CONTEXT(verifyPrefix(prefixFormat));
                WITH_CONTEXT(requireError(expectedErrors));
            }
        }
    }
};

