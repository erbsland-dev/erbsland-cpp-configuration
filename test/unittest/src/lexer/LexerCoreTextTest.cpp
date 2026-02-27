// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Text)
class LexerCoreTextTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    void testBasicText() {
        WITH_CONTEXT(verifyValidText(u8R"("")", u8""));
        WITH_CONTEXT(verifyValidText(u8R"("text")", u8"text"));
        WITH_CONTEXT(verifyValidText(u8R"("    text")", u8"    text"));
        WITH_CONTEXT(verifyValidText(u8R"("text    ")", u8"text    "));
        WITH_CONTEXT(verifyValidText(u8R"("    te    xt    ")", u8"    te    xt    "));
        WITH_CONTEXT(verifyValidText(u8R"(" \tte\t xt \t")", u8" \tte\t xt \t"));
        WITH_CONTEXT(verifyValidText(u8R"("😄➟←Æ×∃⚫︎")", u8"😄➟←Æ×∃⚫︎"));
    }

    void testEscapeSequences() {
        // test escape locations
        WITH_CONTEXT(verifyValidText(u8"\"text\\n\"", u8"text\n"));
        WITH_CONTEXT(verifyValidText(u8"\"\\ntext\"", u8"\ntext"));
        WITH_CONTEXT(verifyValidText(u8"\"te\\nxt\"", u8"te\nxt"));
        WITH_CONTEXT(verifyValidText(u8"\"\\\\\\\\te\\\\\\\\xt\\\\\\\\\"", u8"\\\\te\\\\xt\\\\"));
        WITH_CONTEXT(verifyValidText(u8"\"\\\"\\\"te\\\"\\\"xt\\\"\\\"\"", u8"\"\"te\"\"xt\"\""));
        WITH_CONTEXT(verifyValidText(u8"\"\\$\\$te\\$\\$xt\\$\\$\"", u8"$$te$$xt$$"));
        WITH_CONTEXT(verifyValidText(u8"\"\\n\\nte\\n\\nxt\\n\\n\"", u8"\n\nte\n\nxt\n\n"));
        WITH_CONTEXT(verifyValidText(u8"\"\\N\\Nte\\N\\Nxt\\N\\N\"", u8"\n\nte\n\nxt\n\n"));
        WITH_CONTEXT(verifyValidText(u8"\"\\r\\Rte\\r\\rxt\\r\\r\"", u8"\r\rte\r\rxt\r\r"));
        WITH_CONTEXT(verifyValidText(u8"\"\\t\\Tte\\t\\txt\\t\\t\"", u8"\t\tte\t\txt\t\t"));
        WITH_CONTEXT(verifyValidText(u8"\"\\u0020\\U0020te\\u0020\\u0020xt\\u0020\\u0020\"", u8"  te  xt  "));
        WITH_CONTEXT(verifyValidText(u8"\"\\u{20}\\U{20}te\\u{20}\\u{20}xt\\u{20}\\u{20}\"", u8"  te  xt  "));

        // test range checks
        WITH_CONTEXT(verifyValidText(u8R"("\u{a}")", u8"\n"));
        WITH_CONTEXT(verifyValidText(u8R"("\u{20}")", u8" "));
        WITH_CONTEXT(verifyValidText(u8R"("\u{020}")", u8" "));
        WITH_CONTEXT(verifyValidText(u8R"("\u{0020}")", u8" "));
        WITH_CONTEXT(verifyValidText(u8R"("\u{25cf}")", u8"●"));
        WITH_CONTEXT(verifyValidText(u8R"("\u{25CF}")", u8"●"));
        WITH_CONTEXT(verifyValidText(u8R"("\U{25cf}")", u8"●"));
        WITH_CONTEXT(verifyValidText(u8R"("\U{25CF}")", u8"●"));
        WITH_CONTEXT(verifyValidText(u8R"("\u{00020}")", u8" "));
        WITH_CONTEXT(verifyValidText(u8R"("\u{000020}")", u8" "));
        WITH_CONTEXT(verifyValidText(u8R"("\u{0000020}")", u8" "));
        WITH_CONTEXT(verifyValidText(u8R"("\u{00000020}")", u8" "));
        WITH_CONTEXT(verifyValidText(u8R"("\u{0010ffff}")", u8"\U0010ffff"));

        // test errors
        WITH_CONTEXT(verifyErrorInValue(u8R"("\u{}")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\u{0}")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\u{00}")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\u{ffffffff}")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\u{00110000}")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\ua")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\u0a")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\u00a")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\uatext")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\u0atext")", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"("\u00atext")", ErrorCategory::Syntax));
    }

    void testBasicCode() {
        WITH_CONTEXT(verifyValidCode(u8R"(``)", u8""));
        WITH_CONTEXT(verifyValidCode(u8R"(`text`)", u8"text"));
        WITH_CONTEXT(verifyValidCode(u8R"(`    text`)", u8"    text"));
        WITH_CONTEXT(verifyValidCode(u8R"(`text    `)", u8"text    "));
        WITH_CONTEXT(verifyValidCode(u8R"(`    te    xt    `)", u8"    te    xt    "));
        WITH_CONTEXT(verifyValidCode(u8R"(` \tte\t xt \t`)", u8R"( \tte\t xt \t)"));
        WITH_CONTEXT(verifyValidCode(u8R"(`😄➟←Æ×∃⚫︎`)", u8"😄➟←Æ×∃⚫︎"));
        WITH_CONTEXT(verifyValidCode(u8R"(`\`)", u8R"(\)"));
        WITH_CONTEXT(verifyValidCode(u8R"(`""`)", u8R"("")"));
        WITH_CONTEXT(verifyValidCode(u8R"(`\n\t\u0020\u{20}\$\"`)", u8R"(\n\t\u0020\u{20}\$\")"));
    }
};


