// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(ByteCount)
class LexerStandardByteCountTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    void testValidByteCounts() {
        // Simple valid usages
        WITH_CONTEXT(verifyValidInteger(u8R"(0kb)", 0));
        WITH_CONTEXT(verifyValidInteger(u8R"(1 kb)", 1000));
        WITH_CONTEXT(verifyValidInteger(u8R"(1 kib)", 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(10 MB)", 10000000));
        WITH_CONTEXT(verifyValidInteger(u8R"(10 MiB)", 10 * 1024 * 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(999 gb)", 999000000000ULL));
        WITH_CONTEXT(verifyValidInteger(u8R"(2 giB)", 2ULL * 1024 * 1024 * 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(1 Tb)", 1000000000000ULL));
        WITH_CONTEXT(verifyValidInteger(u8R"(5 tib)", 5ULL * 1024 * 1024 * 1024 * 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(1 PB)", 1000000000000000ULL));
        WITH_CONTEXT(verifyValidInteger(u8R"(2 PiB)", 2ULL * 1024 * 1024 * 1024 * 1024 * 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(3 Eb)", 1000000000000000000ULL * 3));
        WITH_CONTEXT(verifyValidInteger(u8R"(1 EiB)", 1024ULL*1024*1024*1024*1024*1024));
        // Suffixes and case
        WITH_CONTEXT(verifyValidInteger(u8R"(100 KiB)", 100 * 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(900 kib)", 900 * 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(1 KIB)", 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(1 kb)", 1000));
        WITH_CONTEXT(verifyValidInteger(u8R"(1 KB)", 1000));
        // Test optional plus or minus sign and spaces
        WITH_CONTEXT(verifyValidInteger(u8R"(+123 mb)", +123000000));
        WITH_CONTEXT(verifyValidInteger(u8R"(-3 Gb)", -3000000000LL));
        WITH_CONTEXT(verifyValidInteger(u8R"(+1 MiB)", +1024 * 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(-1 MiB)", -1024 * 1024));
        // Test digit separators
        WITH_CONTEXT(verifyValidInteger(u8R"(10'000 mb)", 10000000000ULL));
        WITH_CONTEXT(verifyValidInteger(u8R"(1'024 KiB)", 1024 * 1024));
        // No space between number and suffix, and with space
        WITH_CONTEXT(verifyValidInteger(u8R"(42Gb)", 42000000000ULL));
        WITH_CONTEXT(verifyValidInteger(u8R"(42 GiB)", 42ULL * 1024 * 1024 * 1024));
        // Lowercase, uppercase, and mixed case accepted
        WITH_CONTEXT(verifyValidInteger(u8R"(7 mib)", 7ULL * 1024 * 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(7 MiB)", 7ULL * 1024 * 1024));
        WITH_CONTEXT(verifyValidInteger(u8R"(7 MiB)", 7ULL * 1024 * 1024));
    }

    void testInvalidByteCounts() {
        // Out of range examples for 64-bit signed integers
        WITH_CONTEXT(
            verifyErrorInValue(u8R"(9223372036854775808 kb)", ErrorCategory::LimitExceeded)); // decimal part too large
        WITH_CONTEXT(verifyErrorInValue(u8R"(1 yb)", ErrorCategory::LimitExceeded)); // Exceeds 64-bit
        WITH_CONTEXT(verifyErrorInValue(u8R"(1 YiB)", ErrorCategory::LimitExceeded)); // Exceeds 64-bit

        // Overflow when applying factor
        WITH_CONTEXT(verifyErrorInValue(u8R"(9223372036854777 kb)", ErrorCategory::LimitExceeded));
        // 9223372036854777000 > max 64-bit
        WITH_CONTEXT(verifyErrorInValue(u8R"(9223372036854 GiB)", ErrorCategory::LimitExceeded));
        // 9223372036854*1024^3 > max 64-bit

        // Too many digits (max 19 for 64-bit, ignoring separators)
        WITH_CONTEXT(verifyErrorInValue(u8R"(12345678901234567890 mb)", ErrorCategory::LimitExceeded));
        WITH_CONTEXT(verifyErrorInValue(u8R"(123456789012345678901 KiB)", ErrorCategory::LimitExceeded));

        // Decimal integer rules: leading zeros
        WITH_CONTEXT(verifyErrorInValue(u8R"(0001 kb)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(01 KiB)", ErrorCategory::Syntax));

        // Invalid digit separator: at start or end
        WITH_CONTEXT(verifyErrorInValue(u8R"('1000 mb)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(1000' mb)", ErrorCategory::Syntax));
        // Consecutive digit separators
        WITH_CONTEXT(verifyErrorInValue(u8R"(100''000 kb)", ErrorCategory::Syntax));

        // Unknown suffix or partial matches
        WITH_CONTEXT(verifyErrorInValue(u8R"(10 blabla)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(5 k)", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8R"(5 Mi)", ErrorCategory::Syntax));
        // Space inside the suffix is not allowed
        WITH_CONTEXT(verifyErrorInValue(u8R"(5 Ki B)", ErrorCategory::Syntax));
    }
};
