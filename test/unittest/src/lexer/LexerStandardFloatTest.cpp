// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Float)
class LexerStandardFloatTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    void testValidFloat() {
        WITH_CONTEXT(verifyValidFloat(u8R"(0.0)",      0.0));
        WITH_CONTEXT(verifyValidFloat(u8R"(.0)",       0.0));
        WITH_CONTEXT(verifyValidFloat(u8R"(0.)",       0.0));
        WITH_CONTEXT(verifyValidFloat(u8R"(+0.)",      0.0));
        WITH_CONTEXT(verifyValidFloat(u8R"(-0.0)",     -0.0));
        WITH_CONTEXT(verifyValidFloat(u8R"(1.0)",      1.0));
        WITH_CONTEXT(verifyValidFloat(u8R"(-1.0)",     -1.0));
        WITH_CONTEXT(verifyValidFloat(u8R"(12345.6789)", 12345.6789));
        WITH_CONTEXT(verifyValidFloat(u8R"(0.0000000000000001)", 1e-16));
        WITH_CONTEXT(verifyValidFloat(u8R"(1e0)",       1e0));
        WITH_CONTEXT(verifyValidFloat(u8R"(1E+10)",     1e10));
        WITH_CONTEXT(verifyValidFloat(u8R"(1E-5)",      1e-5));
        WITH_CONTEXT(verifyValidFloat(u8R"(12.34e56)",  12.34e56));
        WITH_CONTEXT(verifyValidFloat(u8R"(10000000000e-000005)", 10000000000.0e-5));
        WITH_CONTEXT(verifyValidFloat(u8R"(8'283.9e-5)", 8283.9e-5));
        WITH_CONTEXT(verifyValidFloat(u8R"(100'000.000'001)", 100000.000001));
        WITH_CONTEXT(verifyValidFloat(u8R"(nan)",       std::numeric_limits<double>::quiet_NaN()));
        WITH_CONTEXT(verifyValidFloat(u8R"(+NaN)",      std::numeric_limits<double>::quiet_NaN()));
        WITH_CONTEXT(verifyValidFloat(u8R"(-NaN)",     -std::numeric_limits<double>::quiet_NaN()));
        WITH_CONTEXT(verifyValidFloat(u8R"(inf)",       std::numeric_limits<double>::infinity()));
        WITH_CONTEXT(verifyValidFloat(u8R"(+INF)",      std::numeric_limits<double>::infinity()));
        WITH_CONTEXT(verifyValidFloat(u8R"(-inf)",     -std::numeric_limits<double>::infinity()));
    }

    void testInvalidFloat() {
        WITH_CONTEXT(verifyErrorInValue(u8R"(005.293)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(10000000000.00000000001)", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8R"(1.000000000000000000000)", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8R"(12.3.4)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(1.23e1234567)", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8R"(0x1.23p+1)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"('100'000.0)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(100'000'.0)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(100''000.0)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(0.'100'000)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(0.100'000')", {ErrorCategory::Syntax, ErrorCategory::UnexpectedEnd}));
        WITH_CONTEXT(verifyErrorInValue(u8R"(0.100''000)", ErrorCategory::Syntax));
    }
};


