// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "erbsland/conf/impl/utilities/InternalError.hpp"

#include <erbsland/conf/impl/value/DocumentBuilder.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;
using impl::DocumentBuilder;


TESTED_TARGETS(DocumentBuilder)
class DocumentBuilderImplTest final : public el::UnitTest {
public:
    using ExpectedValueMap = std::map<String, String>;

    DocumentBuilder builder;
    DocumentPtr doc;

    const Location location{{}, Position{1, 1}}; // a placeholder location.

    auto additionalErrorMessages() -> std::string override {
        try {
            std::string result;
            if (doc == nullptr) {
                doc = builder.getDocumentAndReset();
            }
            if (doc != nullptr) {
                auto flatMap = doc->toFlatValueMap();
                result += "State of the last document 'doc':\n";
                for (const auto &[namePath, value] : flatMap) {
                    result += namePath.toText().toCharString() + ": " + value->toTestText().toCharString() + "\n";
                }
            }
            return result;
        } catch (...) {
            return "Exception while creating additional error messages.";
        }
    }

    void setUp() override {
        builder.reset();
        doc = nullptr;
    }

    void verifyValueMap(const ExpectedValueMap &expectedValueMap) {
        doc = builder.getDocumentAndReset();
        REQUIRE(doc != nullptr);
        auto flatMap = doc->toFlatValueMap();
        // First, convert and verify all name paths.
        auto actualValues = std::map<String, String>{};
        for (auto it = flatMap.begin(); it != flatMap.end(); ++it) {
            auto namePathText = it->first.toText();
            runWithContext(SOURCE_LOCATION(), [&]() {
                REQUIRE(expectedValueMap.contains(namePathText));
            }, [&]() -> std::string {
                return std::format("Unexpected additional value: {} = {}", namePathText, it->second->toTestText());
            });
            actualValues[namePathText] = it->second->toTestText();
        }
        // Now test if all expected values are part of the document.
        for (const auto &[expectedNamePath, expectedValueText] : expectedValueMap) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                REQUIRE(actualValues.contains(expectedNamePath));
            }, [&]() -> std::string {
                return std::format("Missing value: {} = {}", expectedNamePath, expectedValueText);
            });
            const auto actualValueText = actualValues[expectedNamePath];
            if (expectedValueText.find(u8"Float(") == 0) {
                // special handling for floating point values.
                const auto expectedFloat = std::stof(
                    expectedValueText.substr(6, expectedValueText.size() - 7).toCharString());
                REQUIRE(actualValueText.find(u8"Float(") == 0);
                const auto actualFloat = std::stof(
                    actualValueText.substr(6, actualValueText.size() - 7).toCharString());
                REQUIRE_LESS(std::abs(actualFloat - expectedFloat), std::numeric_limits<double>::epsilon());
            } else {
                REQUIRE_EQUAL(actualValueText, expectedValueText);
            }
        }
    }

    void testConstruction() {
        // As we will use a member variable for all following tests, this test constructs and destructs
        // an instance to verify the memory handling.
        std::weak_ptr<Value> weakValue;
        {
            impl::DocumentBuilder builder;
            builder.addSectionMap(NamePath::fromText(u8"main"), location);
            auto value = impl::Value::createInteger(1);
            weakValue = value;
            builder.addValue(NamePath::fromText("main.value_1"), value, location);
            auto doc = builder.getDocumentAndReset();
            REQUIRE(doc != nullptr);
            REQUIRE(doc->value(NamePath::fromText("main.value_1"))->type() == ValueType::Integer);
            // destruct...
        }
        REQUIRE(weakValue.expired()); // no leaks.
    }

    void testBasics() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addValue(
            NamePath::fromText("main.value_1"),
            impl::Value::createInteger(1),
            location));
        REQUIRE_NOTHROW(builder.addValue(
            NamePath::fromText("value_2"),
            impl::Value::createInteger(2),
            location));
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.value_1", u8"Integer(1)"},
            {u8"main.value_2", u8"Integer(2)"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testEmptyDocument() {
        auto expectedValueMap = ExpectedValueMap{
            // empty map
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testAllTypes() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_1"), impl::Value::createInteger(12345), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_2"), impl::Value::createBoolean(true), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_3"), impl::Value::createFloat(123.456), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_4"), impl::Value::createText(u8"😆"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_5"), impl::Value::createDate(Date{2025, 12, 26}), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_6"), impl::Value::createTime(Time{22, 11, 33, 123456000, TimeOffset::utc()}), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_7"), impl::Value::createDateTime(
            DateTime{Date{2025, 12, 26}, Time{22, 11, 33, 123456000, TimeOffset::utc()}}), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_8"), impl::Value::createBytes(
            Bytes::fromHex("0102aabbcc")), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_9"), impl::Value::createTimeDelta(
            TimeDelta{TimeUnit::Hours, 5}), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_10"), impl::Value::createRegEx(
            RegEx{u8"abc", false}), location));
        std::vector<impl::ValuePtr> valueList;
        valueList.emplace_back(impl::Value::createInteger(1));
        valueList.emplace_back(impl::Value::createInteger(2));
        valueList.emplace_back(impl::Value::createInteger(3));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.value_11"), impl::Value::createValueList(std::move(valueList)), location));
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.value_1", u8"Integer(12345)"},
            {u8"main.value_2", u8"Boolean(true)"},
            {u8"main.value_3", u8"Float(123.456)"},
            {u8"main.value_4", u8"Text(\"\\u{1f606}\")"},
            {u8"main.value_5", u8"Date(2025-12-26)"},
            {u8"main.value_6", u8"Time(22:11:33.123456z)"},
            {u8"main.value_7", u8"DateTime(2025-12-26 22:11:33.123456z)"},
            {u8"main.value_8", u8"Bytes(0102aabbcc)"},
            {u8"main.value_9", u8"TimeDelta(5,hour)"},
            {u8"main.value_10", u8"RegEx(\"abc\")"},
            {u8"main.value_11", u8"ValueList()"},
            {u8"main.value_11[0]", u8"Integer(1)"},
            {u8"main.value_11[1]", u8"Integer(2)"},
            {u8"main.value_11[2]", u8"Integer(3)"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testNestedSections() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.server"), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.server.filter"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("value_1"), impl::Value::createInteger(1), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.client"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("value_2"), impl::Value::createInteger(2), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.server.handler"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("value_3"), impl::Value::createInteger(3), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"web"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("value_4"), impl::Value::createInteger(4), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"web.pages"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("value_5"), impl::Value::createInteger(5), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.server.value_6"), impl::Value::createInteger(6), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.client.value_7"), impl::Value::createInteger(7), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.server.handler.value_8"), impl::Value::createInteger(8), location));
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.server", u8"SectionWithNames()"},
            {u8"main.server.value_6", u8"Integer(6)"},
            {u8"main.server.filter", u8"SectionWithNames()"},
            {u8"main.server.filter.value_1", u8"Integer(1)"},
            {u8"main.client", u8"SectionWithNames()"},
            {u8"main.client.value_2", u8"Integer(2)"},
            {u8"main.client.value_7", u8"Integer(7)"},
            {u8"main.server.handler", u8"SectionWithNames()"},
            {u8"main.server.handler.value_3", u8"Integer(3)"},
            {u8"main.server.handler.value_8", u8"Integer(8)"},
            {u8"web", u8"SectionWithNames()"},
            {u8"web.value_4", u8"Integer(4)"},
            {u8"web.pages", u8"SectionWithNames()"},
            {u8"web.pages.value_5", u8"Integer(5)"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testSectionList() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addSectionList(NamePath::fromText(u8"main.server"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("value_1"), impl::Value::createInteger(1), location));
        REQUIRE_NOTHROW(builder.addSectionList(NamePath::fromText(u8"main.server"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("value_2"), impl::Value::createInteger(2), location));
        REQUIRE_NOTHROW(builder.addSectionList(NamePath::fromText(u8"main.server"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("value_3"), impl::Value::createInteger(3), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.server.value_4"), impl::Value::createInteger(4), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText("main.server.details"), location));
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.server", u8"SectionList()"},
            {u8"main.server[0]", u8"SectionWithNames()"},
            {u8"main.server[0].value_1", u8"Integer(1)"},
            {u8"main.server[1]", u8"SectionWithNames()"},
            {u8"main.server[1].value_2", u8"Integer(2)"},
            {u8"main.server[2]", u8"SectionWithNames()"},
            {u8"main.server[2].value_3", u8"Integer(3)"},
            {u8"main.server[2].value_4", u8"Integer(4)"},
            {u8"main.server[2].details", u8"SectionWithNames()"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testIntermediateConversion() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"one.two.three.four"), location));
        // one, two and three are intermediate sections at this point
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"one"), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"one.two.three"), location));
        auto expectedValueMap = ExpectedValueMap{
            {u8"one", u8"SectionWithNames()"},
            {u8"one.two", u8"IntermediateSection()"},
            {u8"one.two.three", u8"SectionWithNames()"},
            {u8"one.two.three.four", u8"SectionWithNames()"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testCommonErrors() {
        // Adding values before any section is created.
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("main"), impl::Value::createInteger(1), location));
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("main.server"), impl::Value::createInteger(1), location));
        // Invalid name paths.
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath{}, location));
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath::fromText("main[5]"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath::fromText("main.\"\"[5]"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath{}, location));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath::fromText("main[5]"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath::fromText("main.\"\"[5]"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath::fromText("main.\"text\""), location));
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath{}, impl::Value::createInteger(1), location));
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("main[1]"), impl::Value::createInteger(1), location));
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("main.\"\"[2]"), impl::Value::createInteger(1), location));
        // Adding a value to a non-existing section.
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.server"), location));
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("main.one.two.three"), impl::Value::createInteger(1), location));
        // after all these errors, no additional elements should be created.
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"IntermediateSection()"},
            {u8"main.server", u8"SectionWithNames()"},
        };
        verifyValueMap(expectedValueMap);
    }

    void testNameConflicts() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.server"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath::fromText("main.server"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath::fromText("main"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath::fromText("main.server"), location));
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("main.server"), impl::Value::createInteger(1), location));
        // after all errors, only the initial two elements should exist.
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"IntermediateSection()"},
            {u8"main.server", u8"SectionWithNames()"},
        };
        verifyValueMap(expectedValueMap);
    }

    void testNameConflicts2() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.server"), impl::Value::createInteger(1), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath::fromText("main.server"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath::fromText("main.server.section"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath::fromText("main.server"), location));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath::fromText("main.server.section"), location));
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("main.server.value"), impl::Value::createInteger(1), location));
        // after all errors, only the initial two elements should exist.
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.server", u8"Integer(1)"},
        };
        verifyValueMap(expectedValueMap);
    }

    void testTextNames() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.text"), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.text.\"Value 1\""), impl::Value::createInteger(1), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.text.\"Value 2\""), impl::Value::createInteger(2), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.text.\"Value 3\""), impl::Value::createInteger(3), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.sub.\"Section 1\""), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.sub.\"Section 2\""), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("value_4"), impl::Value::createInteger(4), location));
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"IntermediateSection()"},
            {u8"main.text", u8"SectionWithTexts()"},
            {u8"main.text.\"Value 1\"", u8"Integer(1)"},
            {u8"main.text.\"Value 2\"", u8"Integer(2)"},
            {u8"main.text.\"Value 3\"", u8"Integer(3)"},
            {u8"main.sub", u8"SectionWithTexts()"},
            {u8"main.sub.\"Section 1\"", u8"SectionWithNames()"},
            {u8"main.sub.\"Section 2\"", u8"SectionWithNames()"},
            {u8"main.sub.\"Section 2\".value_4", u8"Integer(4)"},
        };
        verifyValueMap(expectedValueMap);
    }

    void testTextNameSectionErrors() {
        // Text sections must not be added to the document itself.
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath::fromText(u8"\"Text\""), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.text"), location));
        // Must not mix text names with regular names.
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath::fromText(u8"main.\"Text\""), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.text.\"Text\""), location));
        // Must not mix text names with regular names.
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath::fromText(u8"main.text.regular"), location));
        // Subsections aren't allowed for text sections.
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath::fromText(u8"main.text.\"Text\".regular"), location));
        // Section list must not have text names.
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath::fromText(u8"main.text.\"Text2\""), location));
        // make sure only valid elements got added.
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"IntermediateSection()"},
            {u8"main.text", u8"SectionWithTexts()"},
            {u8"main.text.\"Text\"", u8"SectionWithNames()"},
        };
        verifyValueMap(expectedValueMap);
    }

    void testTextNameValueErrors() {
        // Regular values must not be added to the root, this is also true for text names.
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("\"Text\""), impl::Value::createInteger(1), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main.text"), location));
        // Mixing regular with text names is not allowed.
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("main.\"Text\""), impl::Value::createInteger(1), location));
        REQUIRE_NOTHROW(builder.addValue(NamePath::fromText("main.text.\"Value 1\""), impl::Value::createInteger(1), location));
        // Mixing regular with text names is not allowed.
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath::fromText("main.text.value_2"), impl::Value::createInteger(1), location));
        // make sure only valid elements got added.
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.text", u8"SectionWithTexts()"},
            {u8"main.text.\"Value 1\"", u8"Integer(1)"},
        };
        verifyValueMap(expectedValueMap);
    }

    void testAddingInvalidValueTypes() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        // 'addValue' must only accept values.
        REQUIRE_THROWS(builder.addValue(NamePath::fromText("main.section"), impl::Value::createSectionWithNames(), location));
        REQUIRE_THROWS(builder.addValue(NamePath::fromText("main.section"), impl::Value::createIntermediateSection(), location));
        REQUIRE_THROWS(builder.addValue(NamePath::fromText("main.section"), impl::Value::createSectionList(), location));
        REQUIRE_THROWS(builder.addValue(NamePath::fromText("main.section"), impl::Value::createSectionWithTexts(), location));
        REQUIRE_THROWS(builder.addValue(NamePath::fromText("main.section"), nullptr, location));

        struct UndefinedValue : impl::Value {
            UndefinedValue() = default;
            [[nodiscard]] auto type() const noexcept -> ValueType override {
                return ValueType::Undefined;
            }
            [[nodiscard]] auto deepCopy() const -> impl::ValuePtr override {
                impl::throwInternalError("not implemented");
            }
        };
        REQUIRE_THROWS(builder.addValue(NamePath::fromText("main.section"), std::make_shared<UndefinedValue>(), location));
    }

    void testAddingIntUsingTemplates() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_1"), static_cast<int8_t>(1)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_2"), static_cast<uint8_t>(2)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_3"), static_cast<int16_t>(3)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_4"), static_cast<uint16_t>(4)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_5"), static_cast<int32_t>(5)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_6"), static_cast<uint32_t>(6)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_7"), static_cast<int64_t>(7)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_8"), static_cast<uint64_t>(8)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_9"), static_cast<int>(9)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_10"), static_cast<unsigned int>(10)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_11"), static_cast<long>(11)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_12"), static_cast<unsigned long>(12)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_13"), static_cast<short>(13)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_14"), static_cast<unsigned short>(14)));

        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.value_1", u8"Integer(1)"},
            {u8"main.value_2", u8"Integer(2)"},
            {u8"main.value_3", u8"Integer(3)"},
            {u8"main.value_4", u8"Integer(4)"},
            {u8"main.value_5", u8"Integer(5)"},
            {u8"main.value_6", u8"Integer(6)"},
            {u8"main.value_7", u8"Integer(7)"},
            {u8"main.value_8", u8"Integer(8)"},
            {u8"main.value_9", u8"Integer(9)"},
            {u8"main.value_10", u8"Integer(10)"},
            {u8"main.value_11", u8"Integer(11)"},
            {u8"main.value_12", u8"Integer(12)"},
            {u8"main.value_13", u8"Integer(13)"},
            {u8"main.value_14", u8"Integer(14)"},
        };
        verifyValueMap(expectedValueMap);
    }

    void testAddingFloatsUsingTemplates() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_1"), static_cast<float>(1.1)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_2"), static_cast<double>(2.2)));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_3"), static_cast<long double>(3.3)));

        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.value_1", u8"Float(1.1)"},
            {u8"main.value_2", u8"Float(2.2)"},
            {u8"main.value_3", u8"Float(3.3)"},
        };
        verifyValueMap(expectedValueMap);
    }

    void testAddingRegEx() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_1"), RegEx{u8"abc", false}));

        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.value_1", u8"RegEx(\"abc\")"},
        };
        verifyValueMap(expectedValueMap);
    }

    void testAddingValuesUsingTemplates() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main"), location));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_1"), 12345));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_2"), true));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_3"), 123.456));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_4"), String{u8"😆"}));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_5"), Date{2025, 12, 26}));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_6"), Time{22, 11, 33, 123456000, TimeOffset::utc()}));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_7"), DateTime{Date{2025, 12, 26}, Time{22, 11, 33, 123456000, TimeOffset::utc()}}));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_8"), Bytes::fromHex("0102aabbcc")));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_9"), TimeDelta{TimeUnit::Hours, 5}));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_10"), std::u8string(u8"abc")));
        REQUIRE_NOTHROW(builder.addValueT(NamePath::fromText("main.value_11"), std::string("abc")));
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"SectionWithNames()"},
            {u8"main.value_1", u8"Integer(12345)"},
            {u8"main.value_2", u8"Boolean(true)"},
            {u8"main.value_3", u8"Float(123.456)"},
            {u8"main.value_4", u8"Text(\"\\u{1f606}\")"},
            {u8"main.value_5", u8"Date(2025-12-26)"},
            {u8"main.value_6", u8"Time(22:11:33.123456z)"},
            {u8"main.value_7", u8"DateTime(2025-12-26 22:11:33.123456z)"},
            {u8"main.value_8", u8"Bytes(0102aabbcc)"},
            {u8"main.value_9", u8"TimeDelta(5,hour)"},
            {u8"main.value_10", u8"Text(\"abc\")"},
            {u8"main.value_11", u8"Text(\"abc\")"},
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }
};

