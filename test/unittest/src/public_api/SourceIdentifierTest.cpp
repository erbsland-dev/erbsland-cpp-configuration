// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/SourceIdentifier.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;


TESTED_TARGETS(SourceIdentifier)
class SourceIdentifierTest final : public el::UnitTest {
public:
    void testCreateAndAccessors() {
        auto fileId = SourceIdentifier::createForFile(u8"config.elcl");
        REQUIRE(fileId->name() == u8"file");
        REQUIRE(fileId->path() == u8"config.elcl");

        auto textId = SourceIdentifier::create(u8"text", u8"");
        REQUIRE(textId->name() == u8"text");
        REQUIRE(textId->path().empty());
    }

    void testEqualityOperators() {
        auto id1 = SourceIdentifier::createForFile(u8"a.elcl");
        auto id2 = SourceIdentifier::createForFile(u8"a.elcl");
        auto id3 = SourceIdentifier::createForFile(u8"b.elcl");
        auto textId = SourceIdentifier::create(u8"text", u8"");

        REQUIRE(*id1 == *id2);
        REQUIRE(*id1 != *id3);
        REQUIRE(*id1 != *textId);
    }

    void testToText() {
        auto id = SourceIdentifier::createForFile(u8"path.elcl");
        REQUIRE(id->toText() == u8"file:path.elcl");

        auto textId = SourceIdentifier::create(u8"text", u8"");
        REQUIRE(textId->toText() == u8"text");

        auto idNoName = SourceIdentifier::create(u8"", u8"path.elcl");
        REQUIRE(idNoName->toText() == u8"unknown:path.elcl");

        auto idNoPath = SourceIdentifier::createForFile(u8"");
        REQUIRE(idNoPath->toText() == u8"file");
    }

    void testAreEqualHelper() {
        auto id1 = SourceIdentifier::createForFile(u8"a.elcl");
        auto id2 = SourceIdentifier::createForFile(u8"a.elcl");
        auto textId = SourceIdentifier::create(u8"text", u8"");
        REQUIRE(SourceIdentifier::areEqual(id1, id2));
        REQUIRE(SourceIdentifier::areEqual(SourceIdentifierPtr{}, SourceIdentifierPtr{}));
        REQUIRE_FALSE(SourceIdentifier::areEqual(id1, SourceIdentifierPtr{}));
        REQUIRE_FALSE(SourceIdentifier::areEqual(id1, textId));
    }
};

