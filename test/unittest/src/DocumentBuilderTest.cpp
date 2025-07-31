// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/DocumentBuilder.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;


TESTED_TARGETS(DocumentBuilder)
class DocumentBuilderTest final : public el::UnitTest {
public:
    using ExpectedValueMap = std::map<String, String>;

    DocumentBuilder builder;
    DocumentPtr doc;

    const Location location{{}, Position{1, 1}}; // a placeholder location.

    // NOTE:
    // As the public interface `DocumentBuilder` is just a wrapper around `impl::DocumentBuilder`, this unit test
    // is deliberately limited to this wrapping functionality.
    // The tests check if the calls are correctly passed to the implementation and exceptions are passed back to
    // the user code. See `DocumentBuilderImplTest` and `DocumentBuilderStorageTest` for extensive tests
    // of the builder logic.

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
        // create and destroy a builder and document locally.
        DocumentBuilder builder;
        builder.addSectionMap(u8"main");
        builder.addValue(u8"main.value_1", 1);
        auto doc = builder.getDocumentAndReset();
        REQUIRE(doc != nullptr);
        REQUIRE(doc->value(NamePath::fromText("main.value_1"))->type() == ValueType::Integer);
    }

    void testBasics() {
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main"));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_1", 1));
        REQUIRE_NOTHROW(builder.addValue(u8"value_2", 2));
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
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main"));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_1", 12345));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_2", true));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_3", 123.456));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_4", String{u8"😆"}));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_5", Date{2025, 12, 26}));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_6", Time{22, 11, 33, 123456000, TimeOffset::utc()}));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_7", DateTime{Date{2025, 12, 26}, Time{22, 11, 33, 123456000, TimeOffset::utc()}}));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_8", Bytes::fromHex("0102aabbcc")));
        REQUIRE_NOTHROW(builder.addValue(u8"main.value_9", TimeDelta{TimeUnit::Hours, 5}));
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
        };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testAllTypes2() {
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main"));
        REQUIRE_NOTHROW(builder.addInteger(u8"main.value_1", 12345));
        REQUIRE_NOTHROW(builder.addBoolean(u8"main.value_2", true));
        REQUIRE_NOTHROW(builder.addFloat(u8"main.value_3", 123.456));
        REQUIRE_NOTHROW(builder.addText(u8"main.value_4", String{u8"😆"}));
        REQUIRE_NOTHROW(builder.addDate(u8"main.value_5", Date{2025, 12, 26}));
        REQUIRE_NOTHROW(builder.addTime(u8"main.value_6", Time{22, 11, 33, 123456000, TimeOffset::utc()}));
        REQUIRE_NOTHROW(builder.addDateTime(u8"main.value_7", DateTime{Date{2025, 12, 26}, Time{22, 11, 33, 123456000, TimeOffset::utc()}}));
        REQUIRE_NOTHROW(builder.addBytes(u8"main.value_8", Bytes::fromHex("0102aabbcc")));
        REQUIRE_NOTHROW(builder.addTimeDelta(u8"main.value_9", TimeDelta{TimeUnit::Hours, 5}));
        REQUIRE_NOTHROW(builder.addRegEx(u8"main.value_10", RegEx{u8"abc"}));
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
            };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testAllTypes3() {
        REQUIRE_NOTHROW(builder.addSectionMap(NamePath::fromText(u8"main")));
        REQUIRE_NOTHROW(builder.addInteger(NamePath::fromText(u8"main.value_1"), 12345));
        REQUIRE_NOTHROW(builder.addBoolean(NamePath::fromText(u8"main.value_2"), true));
        REQUIRE_NOTHROW(builder.addFloat(NamePath::fromText(u8"main.value_3"), 123.456));
        REQUIRE_NOTHROW(builder.addText(NamePath::fromText(u8"main.value_4"), String{u8"😆"}));
        REQUIRE_NOTHROW(builder.addDate(NamePath::fromText(u8"main.value_5"), Date{2025, 12, 26}));
        REQUIRE_NOTHROW(builder.addTime(NamePath::fromText(u8"main.value_6"), Time{22, 11, 33, 123456000, TimeOffset::utc()}));
        REQUIRE_NOTHROW(builder.addDateTime(NamePath::fromText(u8"main.value_7"), DateTime{Date{2025, 12, 26}, Time{22, 11, 33, 123456000, TimeOffset::utc()}}));
        REQUIRE_NOTHROW(builder.addBytes(NamePath::fromText(u8"main.value_8"), Bytes::fromHex("0102aabbcc")));
        REQUIRE_NOTHROW(builder.addTimeDelta(NamePath::fromText(u8"main.value_9"), TimeDelta{TimeUnit::Hours, 5}));
        REQUIRE_NOTHROW(builder.addRegEx(NamePath::fromText(u8"main.value_10"), RegEx{u8"abc"}));
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
                };
        WITH_CONTEXT(verifyValueMap(expectedValueMap));
    }

    void testNestedSections() {
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main"));
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main.server"));
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main.server.filter"));
        REQUIRE_NOTHROW(builder.addValue(u8"value_1", 1));
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main.client"));
        REQUIRE_NOTHROW(builder.addValue(u8"value_2", 2));
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main.server.handler"));
        REQUIRE_NOTHROW(builder.addValue(u8"value_3", 3));
        REQUIRE_NOTHROW(builder.addSectionMap(u8"web"));
        REQUIRE_NOTHROW(builder.addValue(u8"value_4", 4));
        REQUIRE_NOTHROW(builder.addSectionMap(u8"web.pages"));
        REQUIRE_NOTHROW(builder.addValue(u8"value_5", 5));
        REQUIRE_NOTHROW(builder.addValue(u8"main.server.value_6", 6));
        REQUIRE_NOTHROW(builder.addValue(u8"main.client.value_7", 7));
        REQUIRE_NOTHROW(builder.addValue(u8"main.server.handler.value_8", 8));
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
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main"));
        REQUIRE_NOTHROW(builder.addSectionList(u8"main.server"));
        REQUIRE_NOTHROW(builder.addValue(u8"value_1", 1));
        REQUIRE_NOTHROW(builder.addSectionList(NamePath::fromText(u8"main.server")));
        REQUIRE_NOTHROW(builder.addValue(u8"value_2", 2));
        REQUIRE_NOTHROW(builder.addSectionList(u8"main.server"));
        REQUIRE_NOTHROW(builder.addValue(u8"value_3", 3));
        REQUIRE_NOTHROW(builder.addValue(u8"main.server.value_4", 4));
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main.server.details"));
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
        REQUIRE_NOTHROW(builder.addSectionMap(u8"one.two.three.four"));
        // one, two and three are intermediate sections at this point
        REQUIRE_NOTHROW(builder.addSectionMap(u8"one"));
        REQUIRE_NOTHROW(builder.addSectionMap(u8"one.two.three"));
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
        REQUIRE_THROWS_AS(Error, builder.addValue(u8"main", 1));
        REQUIRE_THROWS_AS(Error, builder.addValue(u8"main.server", 1));
        // Invalid name paths.
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(NamePath{}));
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(u8"main[5]"));
        REQUIRE_THROWS_AS(Error, builder.addSectionMap(u8"main.\"\"[5]"));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(NamePath{}));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(u8"main[5]"));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(u8"main.\"\"[5]"));
        REQUIRE_THROWS_AS(Error, builder.addSectionList(u8"main.\"text\""));
        REQUIRE_THROWS_AS(Error, builder.addValue(NamePath{}, 1));
        REQUIRE_THROWS_AS(Error, builder.addValue(u8"main[1]", 1));
        REQUIRE_THROWS_AS(Error, builder.addValue(u8"main.\"\"[2]", 1));
        // Adding a value to a non-existing section.
        REQUIRE_NOTHROW(builder.addSectionMap(u8"main.server"));
        REQUIRE_THROWS_AS(Error, builder.addValue(u8"main.one.two.three", 1));
        // after all these errors, no additional elements should be created.
        auto expectedValueMap = ExpectedValueMap{
            {u8"main", u8"IntermediateSection()"},
            {u8"main.server", u8"SectionWithNames()"},
        };
        verifyValueMap(expectedValueMap);
    }
};

