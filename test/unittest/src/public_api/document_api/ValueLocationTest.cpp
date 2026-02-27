// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ValueTestHelper.hpp"


using el::conf::Position;


TESTED_TARGETS(Document Value Location Position)
class ValueLocationTest final : public UNITTEST_SUBCLASS(ValueTestHelper) {
public:
    TESTED_TARGETS(hasLocation location setLocation)
    void testLocation() {
        setupTemplate1("1");
        for (const auto &[namePath, value] : doc->toFlatValueMap()) {
            REQUIRE(value->hasLocation());
            REQUIRE(!value->location().position().isUndefined());
            REQUIRE_EQUAL(value->location().sourceIdentifier()->name(), String{u8"text"});
            REQUIRE(value->location().sourceIdentifier()->path().empty());
        }
        value = doc->valueOrThrow("main");
        REQUIRE_EQUAL(value->location().position(), Position(1, 1));
        value = doc->valueOrThrow("main.value1");
        REQUIRE_EQUAL(value->location().position(), Position(2, 1));
        value = doc->valueOrThrow("main.value_list");
        REQUIRE_EQUAL(value->location().position(), Position(6, 1));
        value = doc->valueOrThrow("main.sub.sub.a");
        REQUIRE_EQUAL(value->location().position(), Position(16, 1));
        value = doc->valueOrThrow("list");
        REQUIRE_EQUAL(value->location().position(), Position(22, 1));
        value = doc->valueOrThrow("list[0]");
        REQUIRE_EQUAL(value->location().position(), Position(22, 1));
        value = doc->valueOrThrow("list[1]");
        REQUIRE_EQUAL(value->location().position(), Position(24, 1));
        value = doc->valueOrThrow("list[2]");
        REQUIRE_EQUAL(value->location().position(), Position(26, 1));
        value = doc->valueOrThrow("list[2].value");
        REQUIRE_EQUAL(value->location().position(), Position(27, 1));
        value = doc->valueOrThrow("main.text");
        REQUIRE_EQUAL(value->location().position(), Position(28, 1));
        value = doc->valueOrThrow("main.text.\"first\"");
        REQUIRE_EQUAL(value->location().position(), Position(29, 1));
        value = doc->valueOrThrow("main.text.\"second\"");
        REQUIRE_EQUAL(value->location().position(), Position(30, 1));
        value = doc->valueOrThrow("main.sub_text");
        REQUIRE_EQUAL(value->location().position(), Position(32, 1));
        value = doc->valueOrThrow("main.sub_text.\"first\"");
        REQUIRE_EQUAL(value->location().position(), Position(32, 1));

        value = doc->valueOrThrow("main.value_list[0]");
        REQUIRE_EQUAL(value->location().position(), Position(6, 14));
        value = doc->valueOrThrow("main.value_list[1]");
        REQUIRE_EQUAL(value->location().position(), Position(6, 17));
        value = doc->valueOrThrow("main.value_list[2]");
        REQUIRE_EQUAL(value->location().position(), Position(6, 20));

        value = doc->valueOrThrow("main.value_matrix");
        REQUIRE_EQUAL(value->location().position(), Position(8, 1));
        value = doc->valueOrThrow("main.value_matrix[0]");
        REQUIRE_EQUAL(value->location().position(), Position(9, 5));
        value = doc->valueOrThrow("main.value_matrix[0][0]");
        REQUIRE_EQUAL(value->location().position(), Position(9, 7));
        value = doc->valueOrThrow("main.value_matrix[0][1]");
        REQUIRE_EQUAL(value->location().position(), Position(9, 10));
        value = doc->valueOrThrow("main.value_matrix[0][2]");
        REQUIRE_EQUAL(value->location().position(), Position(9, 13));
        value = doc->valueOrThrow("main.value_matrix[1]");
        REQUIRE_EQUAL(value->location().position(), Position(10, 5));
        value = doc->valueOrThrow("main.value_matrix[1][0]");
        REQUIRE_EQUAL(value->location().position(), Position(10, 7));
        value = doc->valueOrThrow("main.value_matrix[1][1]");
        REQUIRE_EQUAL(value->location().position(), Position(10, 10));
        value = doc->valueOrThrow("main.value_matrix[1][2]");
        REQUIRE_EQUAL(value->location().position(), Position(10, 13));
        value = doc->valueOrThrow("main.value_matrix[2]");
        REQUIRE_EQUAL(value->location().position(), Position(11, 5));
        value = doc->valueOrThrow("main.value_matrix[2][0]");
        REQUIRE_EQUAL(value->location().position(), Position(11, 7));
        value = doc->valueOrThrow("main.value_matrix[2][1]");
        REQUIRE_EQUAL(value->location().position(), Position(11, 10));
        value = doc->valueOrThrow("main.value_matrix[2][2]");
        REQUIRE_EQUAL(value->location().position(), Position(11, 13));
    }

    void testValueListKeepsLocation() {
        String documentText = u8"[main]\nvalue = \n    1, 2, 3\n";
        Parser parser;
        REQUIRE_NOTHROW(doc = parser.parseOrThrow(Source::fromString(documentText)));
        REQUIRE_EQUAL(doc->valueOrThrow("main.value")->location().position(), Position(2, 1));
        REQUIRE_EQUAL(doc->valueOrThrow("main.value[0]")->location().position(), Position(3, 5));
    }
};


