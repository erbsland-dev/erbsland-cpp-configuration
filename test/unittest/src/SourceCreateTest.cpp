// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/Source.hpp>
#include <../../../src/erbsland/conf/impl/source/FileSource.hpp>
#include <../../../src/erbsland/conf/impl/source/StringSource.hpp>


using namespace el::conf;
using namespace std::filesystem;


TESTED_TARGETS(Source)
class SourceCreateTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testFromFileWithString() {
        auto filePath = createTestFile(u8"test");
        auto source = Source::fromFile(String{filePath.string()});
        REQUIRE(source != nullptr);
        REQUIRE_EQUAL(source->name(), String{u8"file"});
        REQUIRE_EQUAL(source->path(), String{filePath.string()});
        REQUIRE_EQUAL(source->identifier()->toText(), (String(u8"file:") + filePath.string()));
        REQUIRE_FALSE(source->isOpen());
        REQUIRE(dynamic_cast<impl::FileSource*>(source.get()) != nullptr);
    }

    void testFromFileWithPath() {
        auto filePath = createTestFile(u8"data");
        auto source = Source::fromFile(filePath);
        REQUIRE(source != nullptr);
        REQUIRE(source->name() == u8"file");
        REQUIRE(source->path() == String{filePath.string()});
        REQUIRE(source->identifier()->toText() == String(u8"file:") + filePath.string());
        REQUIRE_FALSE(source->isOpen());
        REQUIRE(dynamic_cast<impl::FileSource*>(source.get()) != nullptr);
    }

    void testFromStringWithString() {
        auto source = Source::fromString(String{u8"abc"});
        REQUIRE(source != nullptr);
        REQUIRE(source->name() == u8"text");
        REQUIRE(source->path().empty());
        REQUIRE(source->identifier()->toText() == u8"text");
        REQUIRE_FALSE(source->isOpen());
        REQUIRE(dynamic_cast<impl::StringSource*>(source.get()) != nullptr);
    }

    void testFromStringWithStdStringRValue() {
        auto source = Source::fromString(std::string{"xyz"});
        REQUIRE(source != nullptr);
        REQUIRE(source->name() == u8"text");
        REQUIRE(source->path().empty());
        REQUIRE(source->identifier()->toText() == u8"text");
        REQUIRE_FALSE(source->isOpen());
        REQUIRE(dynamic_cast<impl::StringSource*>(source.get()) != nullptr);
    }

    void testFromStringWithStdStringConst() {
        std::string text{"hello"};
        auto source = Source::fromString(text);
        REQUIRE(source != nullptr);
        REQUIRE(source->name() == u8"text");
        REQUIRE(source->path().empty());
        REQUIRE(source->identifier()->toText() == u8"text");
        REQUIRE_FALSE(source->isOpen());
        REQUIRE(dynamic_cast<impl::StringSource*>(source.get()) != nullptr);
    }
};
