// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/impl/constants/Limits.hpp>
#include <erbsland/conf/Error.hpp>
#include <erbsland/conf/Source.hpp>
#include <erbsland/conf/impl/source/StringSource.hpp>
#include <erbsland/conf/impl/utf8/U8StringView.hpp>


using namespace el::conf;
using impl::StringSource;
using impl::U8StringView;


static constexpr std::size_t cLineBufferSize = 5000;


TESTED_TARGETS(Source)
class StringSourceTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    SourcePtr source{};
    std::array<std::byte, cLineBufferSize> lineBuffer{};
    std::size_t lineLength{0};

    void clearLineBuffer() {
        for (std::size_t i = 0; i < lineBuffer.size(); ++i) {
            lineBuffer[i] = std::byte(0xaa);
        }
    }

    void setUp() override {
        clearLineBuffer();
    }

    void testConstruction() {
        source = Source::fromString(u8"test");
        REQUIRE(source != nullptr);
        REQUIRE(source->name() == u8"text");
        REQUIRE(source->path().empty());
        REQUIRE(source->identifier()->toText() == u8"text");
        REQUIRE_FALSE(source->isOpen());
        REQUIRE_FALSE(source->atEnd());
    }

    void testReadLines() {
        const String content{u8"first line\nsecond line\n"};
        source = Source::fromString(content);
        REQUIRE_NOTHROW(source->open());
        REQUIRE(source->isOpen());
        lineLength = source->readLine(std::span<std::byte>{lineBuffer});
        REQUIRE(lineLength == 11);
        auto actual = U8StringView::fromBytes(Bytes{std::span{lineBuffer.data(), lineLength}});
        REQUIRE(actual == String{u8"first line\n"});
        REQUIRE_FALSE(source->atEnd());
        lineLength = source->readLine(std::span<std::byte>{lineBuffer});
        REQUIRE(lineLength == 12);
        actual = U8StringView::fromBytes(Bytes{std::span{lineBuffer.data(), lineLength}});
        REQUIRE(actual == String{u8"second line\n"});
        REQUIRE(source->atEnd());
        lineLength = source->readLine(std::span<std::byte>{lineBuffer});
        REQUIRE(lineLength == 0);
        REQUIRE_FALSE(source->isOpen());
        REQUIRE(source->atEnd());
    }

    void testReadWithoutOpen() {
        source = Source::fromString(u8"abc");
        REQUIRE_THROWS_AS(Error, lineLength = source->readLine(lineBuffer));
    }

    void testReadAfterClosing() {
        source = Source::fromString(u8"line1\nline2\n");
        REQUIRE_NOTHROW(source->open());
        source->close();
        REQUIRE_THROWS_AS(Error, source->readLine(lineBuffer));

        source = Source::fromString(u8"line1\nline2\n");
        REQUIRE_NOTHROW(source->open());
        REQUIRE_NOTHROW(source->readLine(lineBuffer));
        source->close();
        REQUIRE_THROWS_AS(Error, source->readLine(lineBuffer));
    }

    void testEmptyLines() {
        const String content{u8"first\n\nthird\n"};
        source = Source::fromString(content);
        REQUIRE_NOTHROW(source->open());
        lineLength = source->readLine(lineBuffer);
        REQUIRE_EQUAL(U8StringView::fromBytes(Bytes{std::span{lineBuffer.data(), lineLength}}), String{u8"first\n"});
        lineLength = source->readLine(lineBuffer);
        REQUIRE_EQUAL(U8StringView::fromBytes(Bytes{std::span{lineBuffer.data(), lineLength}}), String{u8"\n"});
        lineLength = source->readLine(lineBuffer);
        REQUIRE_EQUAL(U8StringView::fromBytes(Bytes{std::span{lineBuffer.data(), lineLength}}), String{u8"third\n"});
        REQUIRE(source->atEnd());
    }

    void testZeroLengthInput() {
        source = Source::fromString(u8"");
        REQUIRE_NOTHROW(source->open());
        lineLength = source->readLine(lineBuffer);
        REQUIRE(lineLength == 0);
        REQUIRE_FALSE(source->isOpen());
        REQUIRE(source->atEnd());
    }

    void testMixedLineEndings() {
        const String content{u8"one\ntwo\r\nthree\nfour\r\n"};
        source = Source::fromString(content);
        REQUIRE_NOTHROW(source->open());
        lineLength = source->readLine(lineBuffer);
        REQUIRE_EQUAL(U8StringView::fromBytes(Bytes{std::span{lineBuffer.data(), lineLength}}), String{u8"one\n"});
        lineLength = source->readLine(lineBuffer);
        REQUIRE_EQUAL(U8StringView::fromBytes(Bytes{std::span{lineBuffer.data(), lineLength}}), String{u8"two\r\n"});
        lineLength = source->readLine(lineBuffer);
        REQUIRE_EQUAL(U8StringView::fromBytes(Bytes{std::span{lineBuffer.data(), lineLength}}), String{u8"three\n"});
        lineLength = source->readLine(lineBuffer);
        REQUIRE_EQUAL(U8StringView::fromBytes(Bytes{std::span{lineBuffer.data(), lineLength}}), String{u8"four\r\n"});
        REQUIRE(source->atEnd());
    }

    void testSmallLineBuffer() {
        std::array<std::byte, limits::maxLineLength - 1> smallBuffer{};
        auto span = std::span<std::byte>{smallBuffer.data(), smallBuffer.size()};
        source = Source::fromString(u8"line\n");
        REQUIRE_NOTHROW(source->open());
        REQUIRE_THROWS_AS(Error, source->readLine(span));
    }
};

