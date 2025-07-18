// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Boolean)
class LexerCoreBooleanTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    void testBoolean() {
        // valid values.
        WITH_CONTEXT(verifyValidBoolean(u8"true", true));
        WITH_CONTEXT(verifyValidBoolean(u8"yes", true));
        WITH_CONTEXT(verifyValidBoolean(u8"enabled", true));
        WITH_CONTEXT(verifyValidBoolean(u8"on", true));
        WITH_CONTEXT(verifyValidBoolean(u8"false", false));
        WITH_CONTEXT(verifyValidBoolean(u8"no", false));
        WITH_CONTEXT(verifyValidBoolean(u8"disabled", false));
        WITH_CONTEXT(verifyValidBoolean(u8"off", false));

        WITH_CONTEXT(verifyValidBoolean(u8"TRUE", true));
        WITH_CONTEXT(verifyValidBoolean(u8"YES", true));
        WITH_CONTEXT(verifyValidBoolean(u8"ENABLED", true));
        WITH_CONTEXT(verifyValidBoolean(u8"ON", true));
        WITH_CONTEXT(verifyValidBoolean(u8"FALSE", false));
        WITH_CONTEXT(verifyValidBoolean(u8"NO", false));
        WITH_CONTEXT(verifyValidBoolean(u8"DISABLED", false));
        WITH_CONTEXT(verifyValidBoolean(u8"OFF", false));

        WITH_CONTEXT(verifyValidBoolean(u8"TrUe", true));
        WITH_CONTEXT(verifyValidBoolean(u8"YeS", true));
        WITH_CONTEXT(verifyValidBoolean(u8"EnAbLed", true));
        WITH_CONTEXT(verifyValidBoolean(u8"On", true));
        WITH_CONTEXT(verifyValidBoolean(u8"FaLsE", false));
        WITH_CONTEXT(verifyValidBoolean(u8"No", false));
        WITH_CONTEXT(verifyValidBoolean(u8"DiSaBlEd", false));
        WITH_CONTEXT(verifyValidBoolean(u8"OfF", false));
    }

    void testInvalidBoolean() {
        WITH_CONTEXT(verifyErrorInValue(u8"truee", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"true0", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"false'", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"tru", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"fals", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"tr", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"fa", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"t", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"f", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"tru", ErrorCategory::Syntax));
    }
};


