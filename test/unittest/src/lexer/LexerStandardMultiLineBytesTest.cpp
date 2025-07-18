// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Bytes MultiLine)
class LexerStandardMultiLineBytesTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    struct Line {
        String actualContent; // The actual line content
        Bytes expectedContent; // The expected line content
        bool withComment = false; // Whether the line ends with a comment
    };
    using Lines = std::vector<Line>;

    enum class MultiLineStyle : uint8_t {
        Plain,
        WithCommentAfterOpenBracket,
        NoIndentOnEmptyLines
    };
    static constexpr auto cMultiLineStyles = std::array<MultiLineStyle, 3>{
        MultiLineStyle::Plain,
        MultiLineStyle::WithCommentAfterOpenBracket,
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

    [[nodiscard]] static auto createBytesValueText(
        const Lines &testLines,
        PrefixFormat prefixFormat,
        MultiLineStyle multiLineStyle) -> String {

        String result = u8"<<<";
        const auto indent = indentForPrefix(prefixFormat);
        switch (multiLineStyle) {
        case MultiLineStyle::Plain:
        case MultiLineStyle::NoIndentOnEmptyLines:
            result += u8"\n";
            for (const auto &line : testLines) {
                if (multiLineStyle != MultiLineStyle::NoIndentOnEmptyLines || !line.actualContent.empty()) {
                    result += indent;
                    result += line.actualContent;
                    if (line.withComment) {
                        result += cSimpleComment;
                    }
                }
                result += u8"\n";
            }
            result += indent;
            result += u8">>>";
            break;
        case MultiLineStyle::WithCommentAfterOpenBracket:
            result += u8" ";
            result += cSimpleComment;
            result += u8"\n";
            for (const auto &line : testLines) {
                result += indent;
                result += line.actualContent;
                if (line.withComment) {
                    result += cSimpleComment;
                }
                result += u8"\n";
            }
            result += indent;
            result += u8">>>";
            break;
        default:
            throw std::logic_error("MultiLine style not implemented");
        }
        return result;
    }

    void verifyMultiLinePrefix(PrefixFormat prefixFormat, MultiLineStyle multiLineStyle) {
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        switch (multiLineStyle) {
        case MultiLineStyle::Plain:
        case MultiLineStyle::NoIndentOnEmptyLines:
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
            break;
        case MultiLineStyle::WithCommentAfterOpenBracket:
            WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8" "));
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
            const auto &[actualContent, expectedContent, withComment] = testLines[i];
            if (multiLineStyle != MultiLineStyle::NoIndentOnEmptyLines || !actualContent.empty()) {
                WITH_CONTEXT(requireNextToken(TokenType::Indentation, indent));
            }
            if (!actualContent.empty()) {
                WITH_CONTEXT(requireNextBytesToken(TokenType::MultiLineBytes, expectedContent, actualContent));
            }
            if (withComment) {
                WITH_CONTEXT(requireNextToken(TokenType::Comment, cSimpleComment));
            }
            WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        }
    }

    void verifyMultiLineSuffix(PrefixFormat prefixFormat) {
        const auto indent = indentForPrefix(prefixFormat);
        WITH_CONTEXT(requireNextToken(TokenType::Indentation, indent));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesClose, u8">>>"));
    }

    void verifyMultiLineBytes(const Lines &testLines, PrefixFormat prefixFormat, MultiLineStyle multiLineStyle) {
        WITH_CONTEXT(verifyMultiLinePrefix(prefixFormat, multiLineStyle))
        WITH_CONTEXT(verifyMultiLineLines(testLines, prefixFormat, multiLineStyle));
        WITH_CONTEXT(verifyMultiLineSuffix(prefixFormat));
    }

    /// Verify valid multi-line bytes-data.
    ///
    /// Expects a vector of lines and automatically iterates over many combinations of indentation styles.
    /// If the first line starts with a space or tab, only next line formats are tried.
    ///
    /// @param testLines The lines to test.
    ///
    void verifyValidMultiLineBytes(const Lines &testLines) {
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
                    auto valueText = createBytesValueText(testLines, prefixFormat, multiLineStyle);
                    setupTokenIteratorForValueTest(valueText, prefixFormat, suffixFormat);
                    WITH_CONTEXT(verifyPrefix(prefixFormat));
                    WITH_CONTEXT(verifyMultiLineBytes(testLines, prefixFormat, multiLineStyle));
                    WITH_CONTEXT(verifySuffix(suffixFormat));
                }
            }
        }
    }

    void testEmpty() {
        WITH_CONTEXT(verifyValidMultiLineBytes({}));
        auto testLines = Lines{
                {
                    .actualContent=u8"",
                    .expectedContent=Bytes{}
                }
        };
        WITH_CONTEXT(verifyValidMultiLineBytes(testLines));
    }

    void testSingleLine() {
        auto testLines = Lines{
            {
                // pragma: allowlist nextline secret
                .actualContent=u8"0123456789abcdef",
                // pragma: allowlist nextline secret
                .expectedContent=Bytes::fromHex("0123456789abcdef")
            }
        };
        WITH_CONTEXT(verifyValidMultiLineBytes(testLines));
        testLines = Lines{
            {
                .actualContent=u8"92",
                .expectedContent=Bytes::fromHex("92"),
                .withComment=true
            }
        };
        WITH_CONTEXT(verifyValidMultiLineBytes(testLines));
        testLines = Lines{
            {
                .actualContent=u8"01 23 45 67 89 ab cd ef",
                .expectedContent=Bytes::fromHex("0123456789abcdef")
            }
        };
        WITH_CONTEXT(verifyValidMultiLineBytes(testLines));
        testLines = Lines{
            {
                .actualContent=u8"01\t23\t    \t45\t67\t89\tab\tcd\t\tef\t",
                .expectedContent=Bytes::fromHex("0123456789abcdef")
            }
        };
        WITH_CONTEXT(verifyValidMultiLineBytes(testLines));
        testLines = Lines{
            {
                .actualContent=u8"2244aacc    ",
                .expectedContent=Bytes::fromHex("2244aacc"),
                .withComment=true
            }
        };
        WITH_CONTEXT(verifyValidMultiLineBytes(testLines));
    }

    void testMultipleLines() {
        auto testLines = Lines{
            {
                // pragma: allowlist nextline secret
                .actualContent=u8"5dbedb567716e8bbe38cbc536be2340f6112e81ab8caf164a81d0e01ad78332fa48a788bb76abf6c7322c99f0761c5d99a16c9740faa5155ce2b0aa9e09980b9",
                // pragma: allowlist nextline secret
                .expectedContent=Bytes::fromHex("5dbedb567716e8bbe38cbc536be2340f6112e81ab8caf164a81d0e01ad78332fa48a788bb76abf6c7322c99f0761c5d99a16c9740faa5155ce2b0aa9e09980b9")
            },
            {
                // pragma: allowlist nextline secret
                .actualContent=u8"6e45f3ce2d849fffa192de1d986529bfcfd3ca07d74df6a9496e624d91b5f2891b2e92e3241bf42defbf54944d6b557c68b31c2a0e59f08eea6768e33d163201",
                // pragma: allowlist nextline secret
                .expectedContent=Bytes::fromHex("6e45f3ce2d849fffa192de1d986529bfcfd3ca07d74df6a9496e624d91b5f2891b2e92e3241bf42defbf54944d6b557c68b31c2a0e59f08eea6768e33d163201")
            },
            {
                // pragma: allowlist nextline secret
                .actualContent=u8"7953a222a5acc6439368ef12a512ce5ce137b8c9a0641e15f591f1bfbf7cf17b352e2509be97aa2c52cf14fce36048e1b1b18a3b27da7296218e9935fbcef766",
                // pragma: allowlist nextline secret
                .expectedContent=Bytes::fromHex("7953a222a5acc6439368ef12a512ce5ce137b8c9a0641e15f591f1bfbf7cf17b352e2509be97aa2c52cf14fce36048e1b1b18a3b27da7296218e9935fbcef766")
            },
            {
                // pragma: allowlist nextline secret
                .actualContent=u8"f814bdef0bfcafa32cc445b0c0dc1df26abbf8bdf36bb0b5562cea825092153901a815a49f45f8c8753536286ee7fb4ac6f3e6eb724ad6923945f678295e97d7",
                // pragma: allowlist nextline secret
                .expectedContent=Bytes::fromHex("f814bdef0bfcafa32cc445b0c0dc1df26abbf8bdf36bb0b5562cea825092153901a815a49f45f8c8753536286ee7fb4ac6f3e6eb724ad6923945f678295e97d7")
            },
            {
                // pragma: allowlist nextline secret
                .actualContent=u8"e50baf69608f79a000059d23f7728d764692281610e11101bcdd8f642f05cdd5ef3c8bd5b6bea8122dd364c0c85a08beea2b1aa3671d6a00a2542ca856c6d7f9",
                // pragma: allowlist nextline secret
                .expectedContent=Bytes::fromHex("e50baf69608f79a000059d23f7728d764692281610e11101bcdd8f642f05cdd5ef3c8bd5b6bea8122dd364c0c85a08beea2b1aa3671d6a00a2542ca856c6d7f9")
            },
            {
                // pragma: allowlist nextline secret
                .actualContent=u8"6fa01ee7ed745a8cfc78de4280f07bc836fd4faedc17d4721f6b13c6c2ed19699ddc4b6641a0cb67254e9f0fc067e60f977cd14918800e624bf6d30f9ca5e75f",
                // pragma: allowlist nextline secret
                .expectedContent=Bytes::fromHex("6fa01ee7ed745a8cfc78de4280f07bc836fd4faedc17d4721f6b13c6c2ed19699ddc4b6641a0cb67254e9f0fc067e60f977cd14918800e624bf6d30f9ca5e75f")
            },
            {
                // pragma: allowlist nextline secret
                .actualContent=u8"2bf97e6f1ba7d477564830cd1d49691ccec8f520eb805375ed36d2ad11c6d917e31d7394292b076627cbf99cfba3eadb351d647410a1ce81472d733437459ea2",
                // pragma: allowlist nextline secret
                .expectedContent=Bytes::fromHex("2bf97e6f1ba7d477564830cd1d49691ccec8f520eb805375ed36d2ad11c6d917e31d7394292b076627cbf99cfba3eadb351d647410a1ce81472d733437459ea2")
            },
        };
        WITH_CONTEXT(verifyValidMultiLineBytes(testLines));
    }

    void testInvalidAndIncomplete() {
        setupTokenIterator("[section]\nvalue: <<<");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));

        setupTokenIterator("[section]\nvalue: <<<    ");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));

        setupTokenIterator("[section]\nvalue: <<<hex    ");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesFormat, u8"hex"));
        WITH_CONTEXT(requireNextToken(TokenType::Spacing, u8"    "));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));

        setupTokenIterator("[section]\nvalue: <<<base64\n    >>>");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireError(ErrorCategory::Unsupported));

        setupTokenIterator("[section]\nvalue: <<<$base64\n    >>>");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireError(ErrorCategory::Syntax));

        setupTokenIterator("[section]\nvalue: <<<a0123456789abcdef\n    >>>");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireError(ErrorCategory::LimitExceeded));

        setupTokenIterator("[section]\nvalue: <<<\n");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));

        setupTokenIterator("[section]\nvalue: <<<\n    ");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Indentation, u8"    "));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));

        setupTokenIterator("[section]\nvalue: <<<\n    aa");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Indentation, u8"    "));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytes, u8"aa"));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));

        setupTokenIterator("[section]\nvalue: <<<\n    aa\n");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Indentation, u8"    "));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytes, u8"aa"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));

        setupTokenIterator("[section]\nvalue: <<<\n    aa\n    ");
        WITH_CONTEXT(verifyPrefix(PrefixFormat::SameLine));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytesOpen, u8"<<<"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Indentation, u8"    "));
        WITH_CONTEXT(requireNextToken(TokenType::MultiLineBytes, u8"aa"));
        WITH_CONTEXT(requireNextToken(TokenType::LineBreak, u8"\n"));
        WITH_CONTEXT(requireNextToken(TokenType::Indentation, u8"    "));
        WITH_CONTEXT(requireError(ErrorCategory::UnexpectedEnd));
    }
};


