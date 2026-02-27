// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/impl/constants/Limits.hpp>
#include <erbsland/conf/Name.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <unordered_set>


using namespace el::conf;


TESTED_TARGETS(Name)
class NameTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    Name name;

    void testEmpty() {
        name = {};
        REQUIRE(name.empty());
        REQUIRE(name.isRegular());
        REQUIRE(name.asText().empty());
        REQUIRE(name.asIndex() == 0);
        REQUIRE(name.pathTextSize() == 0);
        REQUIRE(name.toPathText().empty());
        REQUIRE_EQUAL(name.type(), NameType::Regular);
    }

    void testCreateRegular() {
        name = Name::createRegular(u8"server");
        REQUIRE(name.isRegular());
        REQUIRE(name.asText() == String{u8"server"});
        REQUIRE(name.asIndex() == 0);
        REQUIRE(name.pathTextSize() == 6);
        REQUIRE(name.toPathText() == String{u8"server"});
        REQUIRE_EQUAL(name.type(), NameType::Regular);
    }

    void testCreateRegularMeta() {
        name = Name::createRegular(u8"@version");
        REQUIRE(name.isRegular());
        REQUIRE(name.asText() == String{u8"@version"});
        REQUIRE(name.asIndex() == 0);
        REQUIRE(name.pathTextSize() == 8);
        REQUIRE(name.toPathText() == String{u8"@version"});
        REQUIRE_EQUAL(name.type(), NameType::Regular);
    }

    void testCreateText() {
        // move
        name = Name::createText(u8"text");
        REQUIRE(name.isText());
        REQUIRE(name.asText() == String{u8"text"});
        REQUIRE(name.asIndex() == 0);
        REQUIRE(name.pathTextSize() == 6); // "text"
        REQUIRE(name.toPathText() == String{u8"\"text\""});
        REQUIRE_EQUAL(name.type(), NameType::Text);

        // copy
        const auto text = String{u8"text"};
        name = Name::createText(text);
        REQUIRE(name.isText());
        REQUIRE(name.asText() == String{u8"text"});
        REQUIRE(name.asIndex() == 0);
        REQUIRE(name.pathTextSize() == 6); // "text"
        REQUIRE(name.toPathText() == String{u8"\"text\""});
        REQUIRE_EQUAL(name.type(), NameType::Text);
    }

    void testCreateIndex() {
        name = Name::createIndex(42);
        REQUIRE(name.isIndex());
        REQUIRE_EQUAL(name.asText(), String{u8"42"});
        REQUIRE_EQUAL(name.asIndex(), 42);
        REQUIRE_EQUAL(name.pathTextSize(), 4);
        REQUIRE_EQUAL(name.toPathText(), String{u8"[42]"});
        REQUIRE_EQUAL(name.type(), NameType::Index);
    }

    void testCreateTextIndex() {
        name = Name::createTextIndex(3);
        REQUIRE(name.isTextIndex());
        REQUIRE_EQUAL(name.asText(), String{u8"3"});
        REQUIRE_EQUAL(name.asIndex(), 3);
        REQUIRE_EQUAL(name.pathTextSize(), 5);
        REQUIRE_EQUAL(name.toPathText(), String{u8"\"\"[3]"});
        REQUIRE_EQUAL(name.type(), NameType::TextIndex);
    }

    void testComparison() {
        name = Name::createRegular(u8"server");
        REQUIRE(name == Name::createRegular(u8"server"));
        REQUIRE(name != Name::createRegular(u8"server1"));
        REQUIRE(name != Name::createRegular(u8"server2"));
        REQUIRE(name != Name::createIndex(42));
        REQUIRE(name != Name::createText(u8"server"));
        REQUIRE(name != Name::createTextIndex(3));
    }

    void testHash() {
        name = Name::createRegular(u8"server");
        REQUIRE_EQUAL(name.hash(), Name::createRegular(u8"server").hash());
        REQUIRE_NOT_EQUAL(name.hash(), Name::createRegular(u8"server1").hash());

        REQUIRE_EQUAL(std::hash<Name>{}(name), std::hash<Name>{}(Name::createRegular(u8"server")));

        std::unordered_set<Name> names;
        names.insert(Name::createRegular(u8"server"));
        names.insert(Name::createRegular(u8"server2"));
        names.insert(Name::createText(u8"server"));
        names.insert(Name::createIndex(32));
        REQUIRE(names.contains(Name::createRegular(u8"server")));
        REQUIRE(names.contains(Name::createRegular(u8"server2")));
        REQUIRE(names.contains(Name::createText(u8"server")));
        REQUIRE(names.contains(Name::createIndex(32)));
    }

    void testFormat() {
        name = Name::createRegular(u8"server");
        auto text = std::format("*{}*", name);
        REQUIRE_EQUAL(text, std::string("*server*"));
    }

    void testNormalize() {
        auto text = Name::normalize(u8"A Valid Name 99 12");
        REQUIRE_EQUAL(text, String{u8"a_valid_name_99_12"});
        name = Name::createRegular(u8"Example Name123");
        REQUIRE_EQUAL(name.asText(), String{u8"example_name123"});
        name = Name::createText(u8"Example Name123");
        REQUIRE_EQUAL(name.asText(), String{u8"Example Name123"});
    }

    void testMalformedRegularNames() {
        // empty names aren't allowed.
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8""));
        // names must not exceed maximum length.
        String longName(limits::maxNameLength + 1, u8'a');
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(longName));
        // names must not start with space or underscore.
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"_name"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8" name"));
        // names must not end with space or underscore.
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"name_"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"name "));
        // name must not contain more than one word separator.
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"one__two"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"one  two"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"one _two"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"one_ two"));
        // name must not start with a decimal digit.
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"0name"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"9name"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"@0name"));
        // name must be limited to a-z, 0-9, _ and space.
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"name[]name"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"[name]"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"name.name"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8".name"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"name."));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"näme"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"äbc"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"name→name"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"→name"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"name→"));
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"nａme")); // full-width 'a'
        // encoding errors.
        REQUIRE_THROWS_AS(Error, name = Name::createRegular(u8"name\xffname"));
    }

    void testMalformedTextNames() {
        // empty text-names aren't allowed.
        REQUIRE_THROWS_AS(Error, name = Name::createText(u8""));
        // names must not exceed the maximum length.
        String longName(limits::maxLineLength + 20, u8'a'); // +20 because of detection tolerance.
        REQUIRE_THROWS_AS(Error, name = Name::createText(longName));
        // Illegal code-points: zero is not allowed
        REQUIRE_THROWS_AS(Error, name = Name::createText(
            String{u8"Name \x00 Name", 11}));
    }

    void testInternalView() {
        // testing the internal view, ensure debugging tools work flawlessly.
        name = Name::createRegular(u8"server");
        auto text = internalView(name)->toString();
        REQUIRE(text.contains(u8"Regular"));
        REQUIRE(text.contains(u8"server"));
        name = Name::createText(u8"server");
        text = internalView(name)->toString();
        REQUIRE(text.contains(u8"Text"));
        REQUIRE(text.contains(u8"server"));
        name = Name::createIndex(1234);
        text = internalView(name)->toString();
        REQUIRE(text.contains(u8"Index"));
        REQUIRE(text.contains(u8"1234"));
    }

    void testNameOrder() {
        WITH_CONTEXT(requireAllOperators(
            Name::createRegular(u8"anna"), Name::createRegular(u8"bert"), Name::createRegular(u8"zoe"),
            Name::createRegular(u8"anna"), Name::createRegular(u8"bert"), Name::createRegular(u8"zoe")));

    }
};
