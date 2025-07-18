// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Integer)
class LexerCoreIntegerTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    void testDecimalInteger() {
        // valid values.
        WITH_CONTEXT(verifyValidInteger(u8"0", 0));
        WITH_CONTEXT(verifyValidInteger(u8"-0", 0));
        WITH_CONTEXT(verifyValidInteger(u8"1", 1));
        WITH_CONTEXT(verifyValidInteger(u8"-1", -1));
        WITH_CONTEXT(verifyValidInteger(u8"1234567890", 1'234'567'890));
        WITH_CONTEXT(verifyValidInteger(u8"-5'239", -5'239));
        WITH_CONTEXT(verifyValidInteger(u8"-9223372036854775808", std::numeric_limits<int64_t>::min()));
        WITH_CONTEXT(verifyValidInteger(u8"9223372036854775807", std::numeric_limits<int64_t>::max()));
        WITH_CONTEXT(verifyValidInteger(u8"-9'223'372'036'854'775'808", std::numeric_limits<int64_t>::min()));
        WITH_CONTEXT(verifyValidInteger(u8"9'223'372'036'854'775'807", std::numeric_limits<int64_t>::max()));

        // limits exceeded
        WITH_CONTEXT(verifyErrorInValue(u8"-9223372036854775809", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8"9223372036854775808", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8"-1000000000000000000000", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8"1000000000000000000000", ErrorCategory::LimitExceeded));

        // wrong syntax to prevent confusing with octal-values.
        WITH_CONTEXT(verifyErrorInValue(u8"00", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"01", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"-00", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"-01", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"02938", ErrorCategory::Syntax));

        // Problems with digit separators
        WITH_CONTEXT(verifyErrorInValue(u8"'123", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"123'", {ErrorCategory::Syntax, ErrorCategory::UnexpectedEnd}));
        WITH_CONTEXT(verifyErrorInValue(u8"1''23", ErrorCategory::Syntax));
    }

    void testHexadecimalInteger() {
        // valid values.
        WITH_CONTEXT(verifyValidInteger(u8"0x0", 0));
        WITH_CONTEXT(verifyValidInteger(u8"0x00", 0));
        WITH_CONTEXT(verifyValidInteger(u8"0x0000000000000000", 0));
        WITH_CONTEXT(verifyValidInteger(u8"-0x0", 0));
        WITH_CONTEXT(verifyValidInteger(u8"0x1", 1));
        WITH_CONTEXT(verifyValidInteger(u8"0xa", 0xa));
        WITH_CONTEXT(verifyValidInteger(u8"0x0123456789abcdef", 0x0123456789abcdef));
        WITH_CONTEXT(verifyValidInteger(u8"0x0123456789ABCDEF", 0x0123456789abcdef));
        WITH_CONTEXT(verifyValidInteger(u8"-0x0123456789abcdef", -0x0123456789abcdef));
        WITH_CONTEXT(verifyValidInteger(u8"-0x0123456789ABCDEF", -0x0123456789abcdef));
        WITH_CONTEXT(verifyValidInteger(u8"0x0123'4567'89ab'cdef", 0x0123456789abcdef));
        WITH_CONTEXT(verifyValidInteger(u8"-0x8000000000000000", std::numeric_limits<int64_t>::min()));
        WITH_CONTEXT(verifyValidInteger(u8"0x7fffffffffffffff", std::numeric_limits<int64_t>::max()));

        // limits exceeded
        WITH_CONTEXT(verifyErrorInValue(u8"0x00000000000000000000000000000000", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8"-0x8000000000000001", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8"0x8000000000000000", ErrorCategory::LimitExceeded));

        // syntax problems
        WITH_CONTEXT(verifyErrorInValue(u8"0xabcdefg", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"0x'0000", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"0x0000'", {ErrorCategory::Syntax, ErrorCategory::UnexpectedEnd}));
        WITH_CONTEXT(verifyErrorInValue(u8"0x00''00", ErrorCategory::Syntax));
    }

    void testBinaryInteger() {
        // valid values.
        WITH_CONTEXT(verifyValidInteger(u8"0b0", 0));
        WITH_CONTEXT(verifyValidInteger(u8"0b00", 0));
        WITH_CONTEXT(verifyValidInteger(u8"0b0000000000000000", 0));
        WITH_CONTEXT(verifyValidInteger(u8"-0b0", 0));
        WITH_CONTEXT(verifyValidInteger(u8"0b1", 1));
        WITH_CONTEXT(verifyValidInteger(u8"0b10", 2));
        WITH_CONTEXT(verifyValidInteger(u8"-0b1000000000000000000000000000000000000000000000000000000000000000", std::numeric_limits<int64_t>::min()));
        WITH_CONTEXT(verifyValidInteger(u8"0b0111111111111111111111111111111111111111111111111111111111111111", std::numeric_limits<int64_t>::max()));
        WITH_CONTEXT(verifyValidInteger(u8"-0b1010'1000'1111'0010", -0b1010'1000'1111'0010));

        // limits exceeded
        WITH_CONTEXT(verifyErrorInValue(u8"0b1000000000000000000000000000000000000000000000000000000000000000", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8"-0b1000000000000000000000000000000000000000000000000000000000000001", ErrorCategory::LimitExceeded));

        // syntax problems.
        WITH_CONTEXT(verifyErrorInValue(u8"0b102", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"0b'0000", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"0b0000'", {ErrorCategory::Syntax, ErrorCategory::UnexpectedEnd}));
        WITH_CONTEXT(verifyErrorInValue(u8"0b00''00", ErrorCategory::Syntax));
    }
};


