// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ValueTestHelper.hpp"


using el::conf::Error;
using el::conf::ErrorCategory;
using el::conf::NamePathLike;


TESTED_TARGETS(Document Value)
class ValueChildValueTest final : public UNITTEST_SUBCLASS(ValueTestHelper) {
public:
    String errorText;

    auto additionalErrorMessages() -> std::string override {
        auto result = ValueTestHelper::additionalErrorMessages();
        if (!errorText.empty()) {
            result += "error: ";
            result += errorText.toCharString();
            result += "\n";
        }
        return result;
    }

    void requireError(const ErrorCategory errorCategory, const NamePathLike &namePath) {
        try {
            value = doc->valueOrThrow(namePath);
            REQUIRE(false);
        } catch (const Error &e) {
            errorText = e.toText();
            REQUIRE_EQUAL(e.category(), errorCategory);
        }
    }

    TESTED_TARGETS(valueOrThrow)
    void testValueOrThrow() {
        setupTemplate1("1");
        REQUIRE_NOTHROW(value = doc->valueOrThrow("main"));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(u8"main"));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(String{u8"main"}));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(Name::createRegular(u8"main")));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(NamePath{Name::createRegular(u8"main")}));
        REQUIRE_NOTHROW(value = doc->valueOrThrow("main.sub.sub.a.value"));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(u8"main.sub.sub.a.value"));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(String{u8"main.sub.sub.a.value"}));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(Name::createRegular(u8"main")));
        REQUIRE_NOTHROW(value = value->valueOrThrow(Name::createRegular(u8"sub")));
        REQUIRE_NOTHROW(value = value->valueOrThrow(Name::createRegular(u8"sub")));
        REQUIRE_NOTHROW(value = value->valueOrThrow(Name::createRegular(u8"a")));
        REQUIRE_NOTHROW(value = value->valueOrThrow(Name::createRegular(u8"value")));
        const auto namePath = NamePath{{
            Name::createRegular(u8"main"),
            Name::createRegular(u8"sub"),
            Name::createRegular(u8"sub"),
            Name::createRegular(u8"a"),
            Name::createRegular(u8"value"),
        }};
        REQUIRE_NOTHROW(value = doc->valueOrThrow(namePath));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(u8"main.value_list[2]"));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(u8"main.value_matrix[2][2]"));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(u8"main.text.\"second\""));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(u8"main.text.\"\"[1]"));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(u8"main.sub_text.\"second\""));
        REQUIRE_NOTHROW(value = doc->valueOrThrow(u8"main.sub_text.\"\"[1]"));

        // not found
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8""));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"unknown"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"\"unknown\""));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"[0]"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"\"\"[0]"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.unknown"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.\"unknown\""));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.\"\"[0]"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.value_list.unknown"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.value_list.\"unknown\""));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.value_list.\"\"[0]"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.value_list[1].unknown"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.value_list[1].\"unknown\""));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.value_list[1].\"\"[0]"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.value_matrix[1][2].unknown"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.value_matrix[1][2].\"unknown\""));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.value_matrix[1][2].\"\"[0]"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.text.\"unknown\""));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.text.\"first\".unknown"));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.text.\"first\".\"unknown\""));
        WITH_CONTEXT(requireError(ErrorCategory::ValueNotFound, u8"main.text.\"first\".\"\"[0]"));

        WITH_CONTEXT(requireError(ErrorCategory::Syntax, u8"main.[0]"));
        WITH_CONTEXT(requireError(ErrorCategory::Syntax, u8"main.text.\"first\".[0]"));
        WITH_CONTEXT(requireError(ErrorCategory::Syntax, u8"main..value1"));
    }

    TESTED_TARGETS(value)
    void testValue() {
        setupTemplate1("1");
        REQUIRE(doc->value("main") != nullptr);

        REQUIRE(doc->value(u8"main") != nullptr);
        REQUIRE(doc->value(String{u8"main"}) != nullptr);
        REQUIRE(doc->value(Name::createRegular(u8"main")) != nullptr);
        REQUIRE(doc->value(NamePath{Name::createRegular(u8"main")}) != nullptr);
        REQUIRE(doc->value("main.sub.sub.a.value") != nullptr);
        REQUIRE(doc->value(u8"main.sub.sub.a.value") != nullptr);
        REQUIRE(doc->value(String{u8"main.sub.sub.a.value"}) != nullptr);
        REQUIRE((value = doc->value(Name::createRegular(u8"main"))) != nullptr);
        REQUIRE((value = value->value(Name::createRegular(u8"sub"))) != nullptr);
        REQUIRE((value = value->value(Name::createRegular(u8"sub"))) != nullptr);
        REQUIRE((value = value->value(Name::createRegular(u8"a"))) != nullptr);
        REQUIRE((value = value->value(Name::createRegular(u8"value"))) != nullptr);
        const auto namePath = NamePath{{
            Name::createRegular(u8"main"),
            Name::createRegular(u8"sub"),
            Name::createRegular(u8"sub"),
            Name::createRegular(u8"a"),
            Name::createRegular(u8"value"),
        }};
        REQUIRE(doc->value(namePath) != nullptr);
        REQUIRE(doc->value(u8"main.value_list[2]") != nullptr);
        REQUIRE(doc->value(u8"main.value_matrix[2][2]") != nullptr);
        REQUIRE(doc->value(u8"main.text.\"second\"") != nullptr);
        REQUIRE(doc->value(u8"main.text.\"\"[1]") != nullptr);
        REQUIRE(doc->value(u8"main.sub_text.\"second\"") != nullptr);
        REQUIRE(doc->value(u8"main.sub_text.\"\"[1]") != nullptr);

        // not found
        REQUIRE(doc->value(u8"") == nullptr);
        REQUIRE(doc->value(u8"unknown") == nullptr);
        REQUIRE(doc->value(u8"\"unknown\"") == nullptr);
        REQUIRE(doc->value(u8"[0]") == nullptr);
        REQUIRE(doc->value(u8"\"\"[0]") == nullptr);
        REQUIRE(doc->value(u8"main.unknown") == nullptr);
        REQUIRE(doc->value(u8"main.\"unknown\"") == nullptr);
        REQUIRE(doc->value(u8"main.\"\"[0]") == nullptr);
        REQUIRE(doc->value(u8"main.value_list.unknown") == nullptr);
        REQUIRE(doc->value(u8"main.value_list.\"unknown\"") == nullptr);
        REQUIRE(doc->value(u8"main.value_list.\"\"[0]") == nullptr);
        REQUIRE(doc->value(u8"main.value_list[1].unknown") == nullptr);
        REQUIRE(doc->value(u8"main.value_list[1].\"unknown\"") == nullptr);
        REQUIRE(doc->value(u8"main.value_list[1].\"\"[0]") == nullptr);
        REQUIRE(doc->value(u8"main.value_matrix[1][2].unknown") == nullptr);
        REQUIRE(doc->value(u8"main.value_matrix[1][2].\"unknown\"") == nullptr);
        REQUIRE(doc->value(u8"main.value_matrix[1][2].\"\"[0]") == nullptr);
        REQUIRE(doc->value(u8"main.text.\"unknown\"") == nullptr);
        REQUIRE(doc->value(u8"main.text.\"first\".unknown") == nullptr);
        REQUIRE(doc->value(u8"main.text.\"first\".\"unknown\"") == nullptr);
        REQUIRE(doc->value(u8"main.text.\"first\".\"\"[0]") == nullptr);
        REQUIRE(doc->value(u8"main.[0]") == nullptr);
        REQUIRE(doc->value(u8"main.text.\"first\".[0]") == nullptr);
        REQUIRE(doc->value(u8"main..value1") == nullptr);
    }

    TESTED_TARGETS(hasValue)
    void testHasValue() {
        setupTemplate1("1");
        REQUIRE(doc->hasValue("main") == true);

        REQUIRE(doc->hasValue(u8"main") == true);
        REQUIRE(doc->hasValue(String{u8"main"}) == true);
        REQUIRE(doc->hasValue(Name::createRegular(u8"main")) == true);
        REQUIRE(doc->hasValue(NamePath{Name::createRegular(u8"main")}) == true);
        REQUIRE(doc->hasValue("main.sub.sub.a.value") == true);
        REQUIRE(doc->hasValue(u8"main.sub.sub.a.value") == true);
        REQUIRE(doc->hasValue(String{u8"main.sub.sub.a.value"}) == true);
        const auto namePath = NamePath{{
            Name::createRegular(u8"main"),
            Name::createRegular(u8"sub"),
            Name::createRegular(u8"sub"),
            Name::createRegular(u8"a"),
            Name::createRegular(u8"value"),
        }};
        REQUIRE(doc->hasValue(namePath) == true);
        REQUIRE(doc->hasValue(u8"main.value_list[2]") == true);
        REQUIRE(doc->hasValue(u8"main.value_matrix[2][2]") == true);
        REQUIRE(doc->hasValue(u8"main.text.\"second\"") == true);
        REQUIRE(doc->hasValue(u8"main.text.\"\"[1]") == true);
        REQUIRE(doc->hasValue(u8"main.sub_text.\"second\"") == true);
        REQUIRE(doc->hasValue(u8"main.sub_text.\"\"[1]") == true);

        // not found
        REQUIRE(doc->hasValue(u8"") == false);
        REQUIRE(doc->hasValue(u8"unknown") == false);
        REQUIRE(doc->hasValue(u8"\"unknown\"") == false);
        REQUIRE(doc->hasValue(u8"[0]") == false);
        REQUIRE(doc->hasValue(u8"\"\"[0]") == false);
        REQUIRE(doc->hasValue(u8"main.unknown") == false);
        REQUIRE(doc->hasValue(u8"main.\"unknown\"") == false);
        REQUIRE(doc->hasValue(u8"main.\"\"[0]") == false);
        REQUIRE(doc->hasValue(u8"main.value_list.unknown") == false);
        REQUIRE(doc->hasValue(u8"main.value_list.\"unknown\"") == false);
        REQUIRE(doc->hasValue(u8"main.value_list.\"\"[0]") == false);
        REQUIRE(doc->hasValue(u8"main.value_list[1].unknown") == false);
        REQUIRE(doc->hasValue(u8"main.value_list[1].\"unknown\"") == false);
        REQUIRE(doc->hasValue(u8"main.value_list[1].\"\"[0]") == false);
        REQUIRE(doc->hasValue(u8"main.value_matrix[1][2].unknown") == false);
        REQUIRE(doc->hasValue(u8"main.value_matrix[1][2].\"unknown\"") == false);
        REQUIRE(doc->hasValue(u8"main.value_matrix[1][2].\"\"[0]") == false);
        REQUIRE(doc->hasValue(u8"main.text.\"unknown\"") == false);
        REQUIRE(doc->hasValue(u8"main.text.\"first\".unknown") == false);
        REQUIRE(doc->hasValue(u8"main.text.\"first\".\"unknown\"") == false);
        REQUIRE(doc->hasValue(u8"main.text.\"first\".\"\"[0]") == false);
        REQUIRE(doc->hasValue(u8"main.[0]") == false);
        REQUIRE(doc->hasValue(u8"main.text.\"first\".[0]") == false);
        REQUIRE(doc->hasValue(u8"main..value1") == false);
    }

    TESTED_TARGETS(size)
    void testSize() {
        setupTemplate1("1");
        REQUIRE_EQUAL(doc->size(), 2);
        value = doc->valueOrThrow("main");
        REQUIRE_EQUAL(value->size(), 11);
        value = doc->valueOrThrow(u8"main.sub");
        REQUIRE_EQUAL(value->size(), 1);
        value = doc->valueOrThrow(u8"main.sub.sub");
        REQUIRE_EQUAL(value->size(), 3);
        value = doc->valueOrThrow(u8"main.sub.sub.a.value");
        REQUIRE_EQUAL(value->size(), 0);
        value = doc->valueOrThrow(u8"main.value_list");
        REQUIRE_EQUAL(value->size(), 3);
        value = doc->valueOrThrow(u8"main.value_list[2]");
        REQUIRE_EQUAL(value->size(), 0);
        value = doc->valueOrThrow(u8"main.value_matrix");
        REQUIRE_EQUAL(value->size(), 3);
        value = doc->valueOrThrow(u8"main.value_matrix[2]");
        REQUIRE_EQUAL(value->size(), 3);
        value = doc->valueOrThrow(u8"main.value_matrix[2][2]");
        REQUIRE_EQUAL(value->size(), 0);
        value = doc->valueOrThrow(u8"main.text");
        REQUIRE_EQUAL(value->size(), 3);
        value = doc->valueOrThrow(u8"main.sub_text");
        REQUIRE_EQUAL(value->size(), 3);
        value = doc->valueOrThrow(u8"main.sub_text.\"first\"");
        REQUIRE_EQUAL(value->size(), 1);
    }

    void testBeginAndEnd() {
        setupTemplate1("1");
        auto it = doc->begin();
        REQUIRE(it != doc->end());
        REQUIRE_EQUAL(it->name(), Name::createRegular(u8"main"));
        REQUIRE_EQUAL(it->size(), 11);
        ++it;
        REQUIRE(it != doc->end());
        REQUIRE_EQUAL(it->name(), Name::createRegular(u8"list"));
        ++it;
        REQUIRE(it == doc->end());

        value = doc->valueOrThrow("main.value1");
        it = value->begin();
        REQUIRE(it == value->end());
    }

    void testEmpty() {
        setupTemplate1("1");
        REQUIRE_FALSE(doc->empty());
        value = doc->valueOrThrow("main");
        REQUIRE_FALSE(value->empty());
        value = doc->valueOrThrow(u8"main.sub");
        REQUIRE_FALSE(value->empty());
        value = doc->valueOrThrow(u8"main.sub.sub");
        REQUIRE_FALSE(value->empty());
        value = doc->valueOrThrow(u8"main.sub.sub.a.value");
        REQUIRE(value->empty());
        value = doc->valueOrThrow(u8"main.value_list");
        REQUIRE_FALSE(value->empty());
        value = doc->valueOrThrow(u8"main.value_list[2]");
        REQUIRE(value->empty());
        value = doc->valueOrThrow(u8"main.value_matrix");
        REQUIRE_FALSE(value->empty());
        value = doc->valueOrThrow(u8"main.value_matrix[2]");
        REQUIRE_FALSE(value->empty());
        value = doc->valueOrThrow(u8"main.value_matrix[2][2]");
        REQUIRE(value->empty());
        value = doc->valueOrThrow(u8"main.text");
        REQUIRE_FALSE(value->empty());
        value = doc->valueOrThrow(u8"main.sub_text");
        REQUIRE_FALSE(value->empty());
        value = doc->valueOrThrow(u8"main.sub_text.\"first\"");
        REQUIRE_FALSE(value->empty());
    }

    void testFirstAndLastValue() {
        setupTemplate1("1", "2", "3");
        value = doc->valueOrThrow("main");
        REQUIRE(value != nullptr);
        value = value->firstValue();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->toTestText(), String{u8"Integer(1)"});

        value = doc->valueOrThrow("main.value_list");
        REQUIRE(value != nullptr);
        value = value->lastValue();
        REQUIRE(value != nullptr);
        REQUIRE_EQUAL(value->toTestText(), String{u8"Integer(3)"});

        value = doc->valueOrThrow("main.value1");
        REQUIRE(value != nullptr);
        value = value->firstValue();
        REQUIRE(value == nullptr);

        value = doc->valueOrThrow("main.value1");
        REQUIRE(value != nullptr);
        value = value->lastValue();
        REQUIRE(value == nullptr);
    }
};

