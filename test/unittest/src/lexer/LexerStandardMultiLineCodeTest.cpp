// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Code MultiLine)
class LexerStandardMultiLineCodeTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    struct Line {
        String actualContent; // The actual line content
        String actualTrailing; // The trailing whitespace on the line
        String expectedContent; // The expected line content
    };
    using Lines = std::vector<Line>;

    enum class MultiLineStyle : uint8_t {
        Plain,
        WithCommentAfterOpenBracket,
        WithLanguageIdentifier,
        WithLanguageIdentifierAndComment,
        NoIndentOnEmptyLines
    };
    static constexpr auto cMultiLineStyles = std::array<MultiLineStyle, 5>{
        MultiLineStyle::Plain,
        MultiLineStyle::WithCommentAfterOpenBracket,
        MultiLineStyle::WithLanguageIdentifier,
        MultiLineStyle::WithLanguageIdentifierAndComment,
        MultiLineStyle::NoIndentOnEmptyLines
    };

    [[nodiscard]] static auto indentForPrefix(PrefixFormat prefixFormat) -> String {
        switch (prefixFormat) {
        case PrefixFormat::SameLine:
        case PrefixFormat::NextLinePattern1:
        case PrefixFormat::NextLinePattern1withComment:
            return cValueOnNextLineIndentationPattern1;
        case PrefixFormat::NextLinePattern2:
            return cValueOnNextLineIndentationPattern2;
        case PrefixFormat::NextLinePattern3:
            return cValueOnNextLineIndentationPattern3;
        default:
            throw std::logic_error("Prefix format not implemented.");
        }
    }

    [[nodiscard]] static auto createValueText(
        const Lines &testLines,
        const String &bracket,
        PrefixFormat prefixFormat,
        MultiLineStyle multiLineStyle) -> String {

        String result = bracket;
        const auto indent = indentForPrefix(prefixFormat);
        switch (multiLineStyle) {
        case MultiLineStyle::Plain:
        case MultiLineStyle::NoIndentOnEmptyLines:
            result += u8"\n";
            break;
        case MultiLineStyle::WithCommentAfterOpenBracket:
            result += cValueOnSameLineSpacing;
            result += cSimpleComment;
            result += u8"\n";
            break;
        case MultiLineStyle::WithLanguageIdentifier:
            result += u8"cpp\n";
            break;
        case MultiLineStyle::WithLanguageIdentifierAndComment:
            result += u8"cpp";
            result += cValueOnSameLineSpacing;
            result += cSimpleComment;
            result += u8"\n";
            break;
        default:
            throw std::logic_error("MultiLine style not implemented");
        }
        for (const auto &line : testLines) {
            if (multiLineStyle != MultiLineStyle::NoIndentOnEmptyLines || !line.actualContent.empty() || !line.actualTrailing.empty()) {
                result += indent;
                result += line.actualContent;
                result += line.actualTrailing;
            }
            result += u8"\n";
        }
        result += indent;
        result += bracket;
        return result;
    }

    void verifyMultiLinePrefix(PrefixFormat prefixFormat, MultiLineStyle multiLineStyle) {
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineCodeOpen, u8"```"));
        switch (multiLineStyle) {
        case MultiLineStyle::Plain:
        case MultiLineStyle::NoIndentOnEmptyLines:
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
            break;
        case MultiLineStyle::WithCommentAfterOpenBracket:
            WITH_CONTEXT(requireNextToken(TokenType::Spacing, cValueOnSameLineSpacing));
            WITH_CONTEXT(requireNextToken(TokenType::Comment, cSimpleComment));
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
            break;
        case MultiLineStyle::WithLanguageIdentifier:
            WITH_CONTEXT(requireNextToken(TokenType::MultiLineCodeLanguage, u8"cpp"));
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
            break;
        case MultiLineStyle::WithLanguageIdentifierAndComment:
            WITH_CONTEXT(requireNextToken(TokenType::MultiLineCodeLanguage, u8"cpp"));
            WITH_CONTEXT(requireNextToken(TokenType::Spacing, cValueOnSameLineSpacing));
            WITH_CONTEXT(requireNextToken(TokenType::Comment, cSimpleComment));
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
            break;
        default:
            throw std::logic_error("MultiLine style not implemented");
        }
    }

    void verifyMultiLineLines(const Lines &testLines, PrefixFormat prefixFormat, MultiLineStyle multiLineStyle) {
        const auto indent = indentForPrefix(prefixFormat);
        for (std::size_t i = 0; i < testLines.size(); ++i) {
            const auto &[actualContent, actualTrailing, expectedContent] = testLines[i];
            if (multiLineStyle != MultiLineStyle::NoIndentOnEmptyLines || !actualContent.empty() || !actualTrailing.empty()) {
                WITH_CONTEXT(requireNextToken(TokenType::Indentation, indent));
            }
            if (!actualContent.empty()) {
                WITH_CONTEXT(requireNextStringToken(TokenType::MultiLineCode, expectedContent, actualContent));
            }
            if (!actualTrailing.empty()) {
                WITH_CONTEXT(requireNextToken(TokenType::Spacing, actualTrailing));
            }
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        }
    }

    void verifyMultiLineSuffix(PrefixFormat prefixFormat) {
        const auto indent = indentForPrefix(prefixFormat);
        WITH_CONTEXT(requireNextToken(TokenType::Indentation, indent));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineCodeClose, u8"```"));
    }

    void verifyMultiLineCode(const Lines &testLines, PrefixFormat prefixFormat, MultiLineStyle multiLineStyle) {
        WITH_CONTEXT(verifyMultiLinePrefix(prefixFormat, multiLineStyle))
        WITH_CONTEXT(verifyMultiLineLines(testLines, prefixFormat, multiLineStyle));
        WITH_CONTEXT(verifyMultiLineSuffix(prefixFormat));
    }

    /// Verify valid multi-line tests.
    ///
    /// Expects a vector of lines and automatically iterates over many combinations of indentation styles.
    /// If the first line starts with a space or tab, only next line formats are tried.
    ///
    /// @param testLines The lines to test.
    ///
    void verifyValidMultiLineCode(const Lines &testLines) {
        const auto bracket = String{u8"```"};
        for (auto prefixFormat : cPrefixFormats) {
            // Skip the same-line test if the first line starts with spacing.
            if (!testLines.empty() &&
                !testLines[0].actualContent.empty() &&
                (testLines[0].actualContent[0] == u8' ' || testLines[0].actualContent[0] == u8'\t') &&
                prefixFormat == PrefixFormat::SameLine) {
                continue;
            }
            for (auto suffixFormat : cSuffixPatterns) {
                for (auto multiLineStyle : cMultiLineStyles) {
                    auto valueText = createValueText(testLines, bracket, prefixFormat, multiLineStyle);
                    setupTokenIteratorForValueTest(valueText, prefixFormat, suffixFormat);
                    WITH_CONTEXT(verifyPrefix(prefixFormat));
                    WITH_CONTEXT(verifyMultiLineCode(testLines, prefixFormat, multiLineStyle));
                    WITH_CONTEXT(verifySuffix(suffixFormat));
                }
            }
        }
    }

    void testEmpty() {
        WITH_CONTEXT(verifyValidMultiLineCode({}));
    }

    void testSingleLine() {
        const auto testLines = Lines{
            {
                .actualContent=u8"This is a single line of text.",
                .actualTrailing=u8"",
                .expectedContent=u8"This is a single line of text."
            }
        };
        WITH_CONTEXT(verifyValidMultiLineCode(testLines));
    }

    void testEmptyLineMiddle() {
        const auto testLines = Lines{
            {
                .actualContent=u8"The first line of text",
                .actualTrailing=u8"     ",
                .expectedContent=u8"The first line of text"
            },
            {
                .actualContent=u8"",
                .actualTrailing=u8"",
                .expectedContent=u8""
            },
            {
                .actualContent=u8"The last line of text",
                .actualTrailing=u8"\t",
                .expectedContent=u8"The last line of text"
            }
        };
        WITH_CONTEXT(verifyValidMultiLineCode(testLines));
    }

    void testEmptyLineFirst() {
        const auto testLines = Lines{
            {
                .actualContent=u8"",
                .actualTrailing=u8"",
                .expectedContent=u8""
            },
            {
                .actualContent=u8"The second line of text",
                .actualTrailing=u8"   \t ",
                .expectedContent=u8"The second line of text"
            },
            {
                .actualContent=u8"The last line of text",
                .actualTrailing=u8"\t  ",
                .expectedContent=u8"The last line of text"
            }
        };
        WITH_CONTEXT(verifyValidMultiLineCode(testLines));
    }

    void testEmptyLineLast() {
        const auto testLines = Lines{
            {
                .actualContent=u8"The first line of text",
                .actualTrailing=u8"",
                .expectedContent=u8"The first line of text"
            },
            {
                .actualContent=u8"The second line of text",
                .actualTrailing=u8"",
                .expectedContent=u8"The second line of text"
            },
            {
                .actualContent=u8"",
                .actualTrailing=u8"",
                .expectedContent=u8""
            },
        };
        WITH_CONTEXT(verifyValidMultiLineCode(testLines));
    }

    void testEscapeSequences() {
        // In code, all escape sequences are ignored.
        const auto testLines = Lines{
            {
                .actualContent=u8R"(\"\n\r\$\u{41}●🄴\u0041\\)",
                .actualTrailing=u8"",
                .expectedContent=u8R"(\"\n\r\$\u{41}●🄴\u0041\\)"
            },
            {
                .actualContent=u8"`` this is not the end",
                .actualTrailing=u8"",
                .expectedContent=u8"`` this is not the end"
            },
            {
                .actualContent=u8"`",
                .actualTrailing=u8"",
                .expectedContent=u8"`"
            },
        };
        WITH_CONTEXT(verifyValidMultiLineCode(testLines));
    }

    void testSpacing() {
        const auto testLines = Lines{
            {
                .actualContent=u8"    text",
                .actualTrailing=u8"    ",
                .expectedContent=u8"    text"
            },
            {
                .actualContent=u8"        text",
                .actualTrailing=u8"        ",
                .expectedContent=u8"        text"
            },
            {
                .actualContent=u8"  text",
                .actualTrailing=u8"  ",
                .expectedContent=u8"  text"
            },
        };
        WITH_CONTEXT(verifyValidMultiLineCode(testLines));
    }

    void testIgnoredIndentedEndSequence() {
        const auto testLines = Lines{
            {
                .actualContent=u8"text",
                .actualTrailing=u8"",
                .expectedContent=u8"text"
            },
            {
                .actualContent=u8" ```",
                .actualTrailing=u8"",
                .expectedContent=u8" ```"
            },
            {
                .actualContent=u8"text",
                .actualTrailing=u8"",
                .expectedContent=u8"text"
            },
            {
                .actualContent=u8"\t```",
                .actualTrailing=u8"",
                .expectedContent=u8"\t```"
            },
        };
        WITH_CONTEXT(verifyValidMultiLineCode(testLines));
    }

    void testIndentationError() {
        // Simulate an error when the indentation of the second line differs from the previous one.
        setupTokenIterator(cSectionLine + cValueStart + cValueOnSameLineSpacing + u8"```\n    text\n  text\n    ```\n");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineCodeOpen, u8"```"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Indentation, u8"    "));
        WITH_CONTEXT(requireNextStringToken(TokenType::MultiLineCode, u8"text", u8"text"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireError(ErrorCategory::Indentation));
    }

    void testEndInText() {
        // Simulate an error when the document ends in the middle of the text.
        setupTokenIterator(cSectionLine + cValueStart + cValueOnSameLineSpacing + u8"```\n    text");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineCodeOpen, u8"```"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Indentation, u8"    "));
        WITH_CONTEXT(requireNextStringToken(TokenType::MultiLineCode, u8"text", u8"text"));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));
    }
};


