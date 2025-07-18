// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Code)
class LexerStandardCodeTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
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


