// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/impl/char/CharStream.hpp>
#include <erbsland/conf/Position.hpp>
#include <erbsland/conf/Source.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;
using impl::Char;
using impl::CharClass;
using impl::CharStream;
using impl::CharStreamPtr;
using impl::DecodedChar;


TESTED_TARGETS(Decoder DecodedChar)
class CharStreamTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    CharStreamPtr decoder;
    DecodedChar decodedChar;

    void tearDown() override {
        cleanUpTestFileDirectory();
    }

    void testConstruction() {
        const auto testFile = createTestFile("[main]");
        auto source = Source::fromFile(testFile);
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
    }

    void requireMatchingAsciiData(const String &testData) {
        Position pos{1, 1};
        for (auto testCharacter : testData) {
            decodedChar = decoder->next();
            if (testCharacter == u8'\n') {
                REQUIRE(decodedChar == CharClass::LineBreak);
                REQUIRE(decodedChar.raw() == U'\n');
                REQUIRE(decodedChar.position() == pos);
                pos.nextLine();
            } else {
                REQUIRE(decodedChar.raw() == static_cast<char32_t>(testCharacter));
                String decodedString;
                decodedChar.appendTo(decodedString);
                auto expectedStr = String(1, testCharacter);
                REQUIRE_EQUAL(decodedString, expectedStr);
                REQUIRE(decodedChar.position() == pos);
                pos.nextColumn();
            }
        }
        decodedChar = decoder->next();
        REQUIRE(decodedChar == Char::EndOfData);
        // At end, each further call of `next` shall return end of data.
        REQUIRE_NOTHROW(decodedChar = decoder->next());
        REQUIRE(decodedChar == Char::EndOfData);
    }

    void testBasicAsciiFileDecode() {
        String testData = u8"[main]\nkey: \"test\"\r\nlast";
        const auto testFile = createTestFile(String{testData});
        auto source = Source::fromFile(testFile);
        REQUIRE(source != nullptr);
        REQUIRE_NOTHROW(source->open());
        REQUIRE(source->name() == u8"file");
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        WITH_CONTEXT(requireMatchingAsciiData(testData))
    }

    void testBasicAsciiStringDecode() {
        String testData = u8"[main]\nkey: \"test\"\r\nlast";
        // std::u8string
        auto source = Source::fromString(String{testData});
        REQUIRE(source != nullptr);
        REQUIRE_NOTHROW(source->open());
        REQUIRE(source->name() == u8"text");
        REQUIRE(source->path().empty());
        REQUIRE(source->identifier()->toText() == u8"text");
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        WITH_CONTEXT(requireMatchingAsciiData(testData))
        // std::string / move
        source = Source::fromString(String(testData).toCharString());
        REQUIRE(source != nullptr);
        REQUIRE_NOTHROW(source->open());
        REQUIRE(source->name() == u8"text");
        REQUIRE(source->path().empty());
        REQUIRE(source->identifier()->toText() == u8"text");
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        WITH_CONTEXT(requireMatchingAsciiData(testData))
        // std::string / copy
        const auto charString = String(testData).toCharString();
        source = Source::fromString(charString);
        REQUIRE(source != nullptr);
        REQUIRE_NOTHROW(source->open());
        REQUIRE(source->name() == u8"text");
        REQUIRE(source->path().empty());
        REQUIRE(source->identifier()->toText() == u8"text");
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        WITH_CONTEXT(requireMatchingAsciiData(testData))
    }

    void testValidUtf8Sequences() {
        // × = C3 97 = U+00D7
        // ← = E2 86 90 = U+2190
        // 😄 = F0 9F 98 84 = U+1F604
        const auto testData = String{u8"×←😄"};
        const auto testFile = createTestFile(testData);
        auto source = Source::fromFile(testFile);
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == 0x00D7U);
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == 0x2190U);
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == 0x1F604U);
        decodedChar = decoder->next();
        REQUIRE(decodedChar == Char::EndOfData);
    }
    void testBom() {
        Bytes const content = Bytes::convertFrom({
            0xEFU, 0xBBU, 0xBFU, // utf-8 BOM
            0x41U, // a
            0x42U, // b
        });
        const auto testFile = createTestFile(content);
        auto source = Source::fromFile(testFile);
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == 0x41U);
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == 0x42U);
        decodedChar = decoder->next();
        REQUIRE(decodedChar == Char::EndOfData);
    }

    template<typename T>
    void requireErrorAfterValidA(const T &content, ErrorCategory expectedErrorCategory) {
        const auto testFile = createTestFile(content);
        auto source = Source::fromFile(testFile);
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == 0x41U);
        try {
            decodedChar = decoder->next();
            REQUIRE(false);
        } catch (Error &e) {
            REQUIRE(e.category() == expectedErrorCategory);
        }
    }

    void testInvalidUtf8Sequences() {
        std::vector<std::byte> content = {
            std::byte(0x41), // a
            std::byte(0b11110100U), // => 1'0011'1111'1111'1111'1111 = 0x13FFFF
            std::byte(0b10111111U), // Error, because it exceeds the valid unicode range.
            std::byte(0b10111111U),
            std::byte(0b10111111U),
            std::byte(0x41), // a
        };
        WITH_CONTEXT(requireErrorAfterValidA(Bytes{content}, ErrorCategory::Encoding))
        content = {
            std::byte(0x41), // A
            std::byte(0b11110100U), // 4 byte sequence
            std::byte(0b11111111U), // Invalid followup byte
            std::byte(0b10111111U),
            std::byte(0b10111111U),
            std::byte(0x41), // A
        };
        WITH_CONTEXT(requireErrorAfterValidA(Bytes{content}, ErrorCategory::Encoding))
        content = {
            std::byte(0x41), // A
            std::byte(0b11110100U), // 4 byte sequence
            std::byte(0b10000001U), // ok
            std::byte(0b11000000U), // not ok.
            std::byte(0b10000000U), // ok
            std::byte(0x41), // A
        };
        WITH_CONTEXT(requireErrorAfterValidA(Bytes{content}, ErrorCategory::Encoding))
        content = {
            std::byte(0x41), // A
            std::byte(0b11110100U), // 4 byte sequence
            std::byte(0b10000001U), // ok
            std::byte(0b10000000U), // ok
            std::byte(0b00111111U), // not ok.
            std::byte(0x41), // A
        };
        WITH_CONTEXT(requireErrorAfterValidA(Bytes{content}, ErrorCategory::Encoding))
        content = {
            std::byte(0x41), // A
            std::byte(0b11110100U), // 4 byte sequence
            std::byte(0b10000001U), // ok
            std::byte(0b10000000U), // ok
            // last byte is missing.
        };
        WITH_CONTEXT(requireErrorAfterValidA(Bytes{content}, ErrorCategory::Encoding))
        content = {
            std::byte(0x41), // A
            std::byte(0b10000001U), // not ok, follow-up byte without start byte.
            std::byte(0x41), // A
            std::byte(0x41), // A
        };
        WITH_CONTEXT(requireErrorAfterValidA(Bytes{content}, ErrorCategory::Encoding))
        content = {
            std::byte(0x41), // A
            std::byte(0b11101101U), // low surrogate U+D800, not ok!
            std::byte(0b10100000U),
            std::byte(0b10000000U),
            std::byte(0x41), // A
        };
        WITH_CONTEXT(requireErrorAfterValidA(Bytes{content}, ErrorCategory::Encoding))
        content = {
            std::byte(0x41), // A
            std::byte(0b11101101U), // high surrogate U+DFFF, not ok!
            std::byte(0b10111111U),
            std::byte(0b10111111U),
            std::byte(0x41), // A
        };
        WITH_CONTEXT(requireErrorAfterValidA(Bytes{content}, ErrorCategory::Encoding))
        content = {
            std::byte(0x41), // A
            std::byte(0xEFU), // BOM in the middle of the document is not allowed.
            std::byte(0xBBU),
            std::byte(0xBFU),
            std::byte(0x41), // A
        };
        WITH_CONTEXT(requireErrorAfterValidA(Bytes{content}, ErrorCategory::Encoding))
    }

    void testInvalidControlCharacters() {
        for (char8_t c = u8'\x00'; c != u8'\x1f'; ++c) {
            String content = u8"A";
            content += c;
            content += u8"A";
            if (c == u8'\x0a' || c == u8'\x0d' || c == u8'\x09') {
                if (c == u8'\x0d') {
                    continue; // don't test CR alone.
                }
                auto source = Source::fromString(String{content});
                REQUIRE_NOTHROW(source->open());
                decoder = CharStream::create(source);
                REQUIRE(decoder != nullptr);
                WITH_CONTEXT(requireMatchingAsciiData(content));
            } else {
                WITH_CONTEXT(requireErrorAfterValidA(content, ErrorCategory::Character));
            }
        }
    }

    void testLineBreaks() {
        std::u8string content = u8"A\nA";
        auto source = Source::fromString(String{content});
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'A');
        REQUIRE(decodedChar.position() == Position{1, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{1, 2});
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'A');
        REQUIRE(decodedChar.position() == Position{2, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == Char::EndOfData);

        content = u8"\n\n\nA";
        source = Source::fromString(String{content});
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{1, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{2, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{3, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'A');
        REQUIRE(decodedChar.position() == Position{4, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == Char::EndOfData);

        content = u8"\r\n\r\n\r\nA";
        source = Source::fromString(String{content});
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\r');
        REQUIRE(decodedChar.position() == Position{1, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{1, 2});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\r');
        REQUIRE(decodedChar.position() == Position{2, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{2, 2});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\r');
        REQUIRE(decodedChar.position() == Position{3, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{3, 2});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LetterA);
        REQUIRE(decodedChar.raw() == U'A');
        REQUIRE(decodedChar.position() == Position{4, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == Char::EndOfData);

        content = u8"\n\r\n\nA";
        source = Source::fromString(String{content});
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{1, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\r');
        REQUIRE(decodedChar.position() == Position{2, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{2, 2});
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{3, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'A');
        REQUIRE(decodedChar.position() == Position{4, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == Char::EndOfData);

        content = u8"\n\rA";
        source = Source::fromString(String{content});
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{1, 1});
        // The decoder ignores invalid line-breaks but changes the position correctly.
        // The lexer will raise an error - as it has more context for better error reporting.
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\r');
        REQUIRE(decodedChar.position() == Position{2, 1});

        content = u8"A\n";
        source = Source::fromString(String{content});
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'A');
        REQUIRE(decodedChar.position() == Position{1, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{1, 2});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == Char::EndOfData);

        content = u8"A\r\n";
        source = Source::fromString(String{content});
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LetterA);
        REQUIRE(decodedChar.raw() == U'A');
        REQUIRE(decodedChar.position() == Position{1, 1});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\r');
        REQUIRE(decodedChar.position() == Position{1, 2});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\n');
        REQUIRE(decodedChar.position() == Position{1, 3});
        decodedChar = decoder->next();
        REQUIRE(decodedChar == Char::EndOfData);

        content = u8"A\r";
        source = Source::fromString(String{content});
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        decodedChar = decoder->next();
        REQUIRE(decodedChar.raw() == U'A');
        REQUIRE(decodedChar.position() == Position{1, 1});
        // The decoder ignored the invalid line-ending but changed the position correctly.
        // The lexer will raise an error - as it has more context for better error reporting.
        decodedChar = decoder->next();
        REQUIRE(decodedChar == CharClass::LineBreak);
        REQUIRE(decodedChar.raw() == U'\r');
        REQUIRE(decodedChar.position() == Position{1, 2});
    }
};

