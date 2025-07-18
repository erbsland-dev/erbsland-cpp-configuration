// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/Bytes.hpp>

#include "LexerValueTestHelper.hpp"


TESTED_TARGETS(Lexer) TAGS(Bytes)
class LexerStandardBytesTest final : public UNITTEST_SUBCLASS(LexerValueTestHelper) {
public:
    void testValidSingleLineBytes() {
        // various empty byte sequences.
        WITH_CONTEXT(verifyValidByteData(u8"<>", Bytes{}));
        WITH_CONTEXT(verifyValidByteData(u8"< >", Bytes{}));
        WITH_CONTEXT(verifyValidByteData(u8"<      >", Bytes{}));
        WITH_CONTEXT(verifyValidByteData(u8"<  \t \t \t\t    >", Bytes{}));

        // test zero bytes, that need to be handled correctly (e.g. catching end byte errors.).
        WITH_CONTEXT(verifyValidByteData(u8"<00>", Bytes::fromHex("00")));
        WITH_CONTEXT(verifyValidByteData(u8"<0000>", Bytes::fromHex("0000")));
        WITH_CONTEXT(verifyValidByteData(u8"<00000000>", Bytes::fromHex("00000000")));
        WITH_CONTEXT(verifyValidByteData(u8"<000000ee>", Bytes::fromHex("000000ee")));
        WITH_CONTEXT(verifyValidByteData(u8"<000000ee>", Bytes::fromHex("000000ee")));

        // test all possible digits.
        WITH_CONTEXT(verifyValidByteData(u8"<00112233445566778899aabbccddeeffAABBCCDDEEFF>",
            Bytes::fromHex("00112233445566778899aabbccddeeffaabbccddeeff")));

        // test valid spacing between the bytes.
        WITH_CONTEXT(verifyValidByteData(u8"<    ab12cd34>", Bytes::fromHex("ab12cd34")));
        WITH_CONTEXT(verifyValidByteData(u8"<ab     12cd34>", Bytes::fromHex("ab12cd34")));
        WITH_CONTEXT(verifyValidByteData(u8"<ab 12\tcd 34>", Bytes::fromHex("ab12cd34")));
        WITH_CONTEXT(verifyValidByteData(u8"<ab12 cd34    >", Bytes::fromHex("ab12cd34")));
        WITH_CONTEXT(verifyValidByteData(u8"<\tab12\tcd34\t>", Bytes::fromHex("ab12cd34")));

        // test hex prefix.
        WITH_CONTEXT(verifyValidByteData(u8"<hex:>", Bytes{}));
        WITH_CONTEXT(verifyValidByteData(u8"<hex: >", Bytes{}));
        WITH_CONTEXT(verifyValidByteData(u8"<hex:ffee>", Bytes::fromHex("ffee")));
        WITH_CONTEXT(verifyValidByteData(u8"<hex: ffee>", Bytes::fromHex("ffee")));
        WITH_CONTEXT(verifyValidByteData(u8"<hex: ff    ee   >", Bytes::fromHex("ffee")));
    }

    void testInvalidBytes() {
        // unexpected end (with comment after value, it is a syntax error).
        WITH_CONTEXT(verifyErrorInValue(u8"<", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));
        WITH_CONTEXT(verifyErrorInValue(u8"<h", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));
        WITH_CONTEXT(verifyErrorInValue(u8"<he", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));
        WITH_CONTEXT(verifyErrorInValue(u8"<hex", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));
        WITH_CONTEXT(verifyErrorInValue(u8"<hex:", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));
        WITH_CONTEXT(verifyErrorInValue(u8"<0", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));
        WITH_CONTEXT(verifyErrorInValue(u8"<00", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));
        WITH_CONTEXT(verifyErrorInValue(u8"<    0", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));
        WITH_CONTEXT(verifyErrorInValue(u8"<    00", {ErrorCategory::UnexpectedEnd, ErrorCategory::Syntax}));

        // odd number of hex digits.
        WITH_CONTEXT(verifyErrorInValue(u8"<0>", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"< 0>", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"< 12 34 5>", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"<123 456>", ErrorCategory::Syntax));

        // invalid characters
        WITH_CONTEXT(verifyErrorInValue(u8"<123u56>", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"<123O56>", ErrorCategory::Syntax));
        WITH_CONTEXT(verifyErrorInValue(u8"<hex:h23456>", ErrorCategory::Syntax));

        // unknown format.
        WITH_CONTEXT(verifyErrorInValue(u8"<base64:23456>", ErrorCategory::Unsupported));
    }
};
