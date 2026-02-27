// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ValueTestHelper.hpp"


TESTED_TARGETS(Document Value NamePath)
class ValueParentTest final : public UNITTEST_SUBCLASS(ValueTestHelper) {
public:
    TESTED_TARGETS(hasParent parent)
    void testParent() {
        setupTemplate1("1");
        REQUIRE_EQUAL(doc->hasParent(), false);
        value = doc->value("main");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        value = doc->value("main");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        value = doc->value("main.sub.sub.a.value");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.sub.sub.a.value"));
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.sub.sub.a"));
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.sub.sub"));
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.sub"));
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main"));
        value = value->parent(); // now we reached the document.
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), false);
        REQUIRE(value->namePath().empty());
        value = value->parent();
        REQUIRE(value == nullptr);
        value = doc->value("main.text.\"second\"");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.text"));
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main"));
        value = doc->value("main.value_matrix[2][1]");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.value_matrix[2]"));
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.value_matrix"));
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main"));
        value = doc->value("list[2].value");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"list[2]"));
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), true);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"list"));
        value = value->parent();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->hasParent(), false);
    }
};

