// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ValueTestHelper.hpp"


TESTED_TARGETS(Document Value NamePath)
class ValueNamingTest final : public UNITTEST_SUBCLASS(ValueTestHelper) {
public:
    TESTED_TARGETS(name)
    void testName() {
        setupTemplate1("1");
        value = doc->value("main");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->name(), Name::createRegular(u8"main"));
        value = doc->value("main.value1");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->name(), Name::createRegular(u8"value1"));
        value = doc->value("main.sub.sub.a.value");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->name(), Name::createRegular(u8"value"));
        value = doc->value("list[1]");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->name(), Name::createIndex(1));
        value = doc->value("main.text.\"second\"");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->name(), Name::createText(u8"second"));
        value = doc->value("main.sub_text.\"third\"");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->name(), Name::createText(u8"third"));
        value = doc->value("main.value_list");
        REQUIRE_NOTHROW(value = value->asValueList().at(1));
        REQUIRE_EQUAL(value->name(), Name::createIndex(1));
        value = doc->value("main.value_matrix");
        REQUIRE_NOTHROW(value = value->asValueList().at(2));
        REQUIRE_NOTHROW(value = value->asValueList().at(1));
        REQUIRE_EQUAL(value->name(), Name::createIndex(1));
    }

    TESTED_TARGETS(namePath)
    void testNamePath() {
        setupTemplate1("1");
        value = doc->value("main");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main"));
        value = doc->value("main.value1");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.value1"));
        value = doc->value("main.sub.sub.a.value");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.sub.sub.a.value"));
        value = doc->value("list[1]");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"list[1]"));
        value = doc->value("main.text.\"second\"");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.text.\"second\""));
        value = doc->value("main.sub_text.\"third\"");
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.sub_text.\"third\""));
        value = doc->value("main.value_list");
        REQUIRE_NOTHROW(value = value->asValueList().at(1));
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.value_list[1]"));
        value = doc->value("main.value_matrix");
        REQUIRE_NOTHROW(value = value->asValueList().at(2));
        REQUIRE_NOTHROW(value = value->asValueList().at(1));
        REQUIRE_EQUAL(value->namePath().toText(), String(u8"main.value_matrix[2][1]"));
    }
};

