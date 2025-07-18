// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(RegEx)
class LexerAdvancedRegexTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    void testBasicRegex() {
        WITH_CONTEXT(verifyValidRegEx(u8R"(//)", u8""));
        WITH_CONTEXT(verifyValidRegEx(u8R"(/text/)", u8"text"));
        WITH_CONTEXT(verifyValidRegEx(u8R"(/    text/)", u8"    text"));
        WITH_CONTEXT(verifyValidRegEx(u8R"(/text    /)", u8"text    "));
        WITH_CONTEXT(verifyValidRegEx(u8R"(/    te    xt    /)", u8"    te    xt    "));
        WITH_CONTEXT(verifyValidRegEx(u8R"(/😄➟←Æ×∃⚫︎/)", u8"😄➟←Æ×∃⚫︎"));
    }

    void testEscapeSequences() {
        // Escape sequences must be passed to the backend, except `\/` that escapes `/`
        WITH_CONTEXT(verifyValidRegEx(u8R"(/text\n/)", u8R"(text\n)"));
        WITH_CONTEXT(verifyValidRegEx(u8R"(/\ntext/)", u8R"(\ntext)"));
        WITH_CONTEXT(verifyValidRegEx(u8R"(/\/text/)", u8R"(/text)"));
        WITH_CONTEXT(verifyValidRegEx(u8R"(/text\//)", u8R"(text/)"));
        WITH_CONTEXT(verifyValidRegEx(u8R"(/\a\b\c\d\e\f\gf\h\i\j\k\0\?\\\"/)", u8R"(\a\b\c\d\e\f\gf\h\i\j\k\0\?\\\")"));
    }

    void testInvalidRegex() {
        WITH_CONTEXT(verifyErrorInValue(u8R"(/text\/)", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));
    }
};


