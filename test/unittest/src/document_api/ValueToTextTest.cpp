// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ValueTestHelper.hpp"


TESTED_TARGETS(Document Value)
class ValueToTextTest final : public UNITTEST_SUBCLASS(ValueTestHelper) {
public:
    TESTED_TARGETS(toTextRepresentation)
    void testToTextRepresentation() {
        struct TestData {
            std::string valueText;
            String expectedText;
        };
        const auto testData = std::vector<TestData>{
            {"0xff", u8"255"}, // integer
            {"enabled", u8"true"}, // boolean
            {"off", u8"false"}, // boolean
            // float requires separate test, as the output format isn't stable.
            {"\"\\u{41}BC\"", u8"ABC"},
            {"2025-02-22", u8"2025-02-22"}, // date
            {"14:02:01.100", u8"14:02:01.1"}, // time
            {"2025-02-22 14:02:01.100", u8"2025-02-22 14:02:01.1"}, // date-time
            {"<  01 0203 >", u8"010203"}, // bytes
            {"20 seconds", u8"20s"}, // time delta
            {"/regex/", u8"regex"}, // time delta
        };
        for (const auto &data : testData) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                setupTemplate2(data.valueText);
                REQUIRE_EQUAL(value->toTextRepresentation(), data.expectedText);
            }, [&]() -> std::string {
                return std::format(
                    "Failed for value: \"{}\" expected: \"{}\"",
                    data.valueText,
                    data.expectedText
                );
            });
        }
        setupTemplate2("123.456");
        const auto text = value->toTextRepresentation();
        const auto doubleValue = std::stod(text.toCharString());
        REQUIRE_LESS(std::abs(doubleValue - 123.456), std::numeric_limits<double>::epsilon());
    }

    void testToTestText() {
        struct TestData {
            std::string valueText;
            String expectedText;
        };
        const auto testData = std::vector<TestData>{
                {"0xff", u8"Integer(255)"}, // integer
                {"enabled", u8"Boolean(true)"}, // boolean
                {"off", u8"Boolean(false)"}, // boolean
                // float requires separate test, as the output format isn't stable.
                {"\"\\u{41}BC\"", u8"Text(\"ABC\")"},
                {"2025-02-22", u8"Date(2025-02-22)"}, // date
                {"14:02:01.100", u8"Time(14:02:01.1)"}, // time
                {"2025-02-22 14:02:01.100", u8"DateTime(2025-02-22 14:02:01.1)"}, // date-time
                {"<  01 0203 >", u8"Bytes(010203)"}, // bytes
                {"20 seconds", u8"TimeDelta(20,second)"}, // time delta
                {"/regex/", u8"RegEx(\"regex\")"}, // time delta
            };
        for (const auto &data : testData) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                setupTemplate2(data.valueText);
                REQUIRE_EQUAL(value->toTestText(), data.expectedText);
            }, [&]() -> std::string {
                return std::format(
                    "Failed for value: \"{}\" expected: \"{}\"",
                    data.valueText,
                    data.expectedText
                );
            });
        }
        setupTemplate2("123.456");
        const auto text = value->toTestText();
        REQUIRE(text.starts_with(u8"Float("));
        REQUIRE(text.ends_with(u8")"));
        const auto doubleValue = std::stod(text.substr(6, text.size() - 7).toCharString());
        REQUIRE_LESS(std::abs(doubleValue - 123.456), std::numeric_limits<double>::epsilon());
    }
};

