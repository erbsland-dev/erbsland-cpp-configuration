// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/vr/RuleType.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>


using namespace el::conf;
using namespace el::conf::vr;


TESTED_TARGETS(RuleType)
class RuleTypeTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultConstructor() {
        RuleType ruleType;

        REQUIRE(ruleType == RuleType::Undefined);
        REQUIRE(ruleType.isUndefined());
        REQUIRE(ruleType.raw() == RuleType::Undefined);
    }

    void testParameterizedConstructor() {
        RuleType ruleTypeInteger{RuleType::Integer};
        RuleType ruleTypeSection{RuleType::Section};
        RuleType ruleTypeAlternatives{RuleType::Alternatives};

        REQUIRE(ruleTypeInteger == RuleType::Integer);
        REQUIRE(ruleTypeSection == RuleType::Section);
        REQUIRE(ruleTypeAlternatives == RuleType::Alternatives);
    }

    void testAcceptsDefaults() {
        const std::array<RuleType, 5> disallowed = {
            RuleType::Section,
            RuleType::SectionList,
            RuleType::SectionWithTexts,
            RuleType::NotValidated,
            RuleType::Alternatives
        };
        for (const auto &ruleType : disallowed) {
            REQUIRE_FALSE(ruleType.acceptsDefaults());
        }
        REQUIRE(RuleType{RuleType::Integer}.acceptsDefaults());
        REQUIRE(RuleType{RuleType::Value}.acceptsDefaults());
    }

    void testMatchesValueType() {
        REQUIRE_FALSE(RuleType{RuleType::Undefined}.matchesValueType(ValueType::Integer));

        REQUIRE(RuleType{RuleType::Value}.matchesValueType(ValueType::Integer));
        REQUIRE_FALSE(RuleType{RuleType::Value}.matchesValueType(ValueType::ValueList));

        REQUIRE(RuleType{RuleType::ValueList}.matchesValueType(ValueType::ValueList));
        REQUIRE(RuleType{RuleType::ValueList}.matchesValueType(ValueType::Float));
        REQUIRE_FALSE(RuleType{RuleType::ValueList}.matchesValueType(ValueType::SectionList));

        REQUIRE(RuleType{RuleType::ValueMatrix}.matchesValueType(ValueType::ValueList));
        REQUIRE(RuleType{RuleType::ValueMatrix}.matchesValueType(ValueType::Boolean));
        REQUIRE_FALSE(RuleType{RuleType::ValueMatrix}.matchesValueType(ValueType::SectionWithTexts));

        REQUIRE(RuleType{RuleType::Section}.matchesValueType(ValueType::SectionWithNames));
        REQUIRE(RuleType{RuleType::Section}.matchesValueType(ValueType::IntermediateSection));
        REQUIRE_FALSE(RuleType{RuleType::Section}.matchesValueType(ValueType::SectionList));

        REQUIRE(RuleType{RuleType::NotValidated}.matchesValueType(ValueType::SectionWithTexts));
        REQUIRE(RuleType{RuleType::Alternatives}.matchesValueType(ValueType::SectionList));

        REQUIRE(RuleType{RuleType::Integer}.matchesValueType(ValueType::Integer));
        REQUIRE_FALSE(RuleType{RuleType::Integer}.matchesValueType(ValueType::Boolean));
    }

    void testToTextAndValueTypeMapping() {
        // pin down mappings to prevent accidental changes
        struct Mapping {
            RuleType ruleType;
            String text;
            ValueType valueType;
            String expectedValueTypeText;
        };
        const std::array<Mapping, 19> mappings = {
            Mapping{RuleType::Undefined, String{u8"Undefined"}, ValueType::Undefined, String{}},
            Mapping{RuleType::Integer, String{u8"Integer"}, ValueType::Integer, String{u8"an integer value"}},
            Mapping{RuleType::Boolean, String{u8"Boolean"}, ValueType::Boolean, String{u8"a Boolean value"}},
            Mapping{RuleType::Float, String{u8"Float"}, ValueType::Float, String{u8"a floating-point or integer value"}},
            Mapping{RuleType::Text, String{u8"Text"}, ValueType::Text, String{u8"a text value"}},
            Mapping{RuleType::Date, String{u8"Date"}, ValueType::Date, String{u8"a date value"}},
            Mapping{RuleType::Time, String{u8"Time"}, ValueType::Time, String{u8"a time value"}},
            Mapping{RuleType::DateTime, String{u8"DateTime"}, ValueType::DateTime, String{u8"a date-time value"}},
            Mapping{RuleType::Bytes, String{u8"Bytes"}, ValueType::Bytes, String{u8"a byte value"}},
            Mapping{RuleType::TimeDelta, String{u8"TimeDelta"}, ValueType::TimeDelta, String{u8"a time-delta value"}},
            Mapping{RuleType::RegEx, String{u8"RegEx"}, ValueType::RegEx, String{u8"a regular expression"}},
            Mapping{RuleType::Value, String{u8"Value"}, ValueType::Undefined, String{u8"any scalar value"}},
            Mapping{RuleType::ValueList, String{u8"ValueList"}, ValueType::ValueList, String{u8"a value list or scalar value"}},
            Mapping{RuleType::ValueMatrix, String{u8"ValueMatrix"}, ValueType::Undefined, String{u8"a nested value list or scalar value"}},
            Mapping{RuleType::Section, String{u8"Section"}, ValueType::SectionWithNames, String{u8"a section"}},
            Mapping{RuleType::SectionList, String{u8"SectionList"}, ValueType::SectionList, String{u8"a section list"}},
            Mapping{RuleType::SectionWithTexts, String{u8"SectionWithTexts"}, ValueType::SectionWithTexts, String{u8"a section with texts"}},
            Mapping{RuleType::NotValidated, String{u8"NotValidated"}, ValueType::Undefined, String{}},
            Mapping{RuleType::Alternatives, String{u8"Alternatives"}, ValueType::Undefined, String{}}
        };

        for (const auto &mapping : mappings) {
            RuleType ruleType(mapping.ruleType);
            REQUIRE_EQUAL(ruleType.toText(), mapping.text);
            REQUIRE(ruleType.toValueType() == mapping.valueType);
            REQUIRE_EQUAL(ruleType.expectedValueTypeText(), mapping.expectedValueTypeText);
        }
    }

    void testFromText() {
        struct Mapping {
            String text;
            RuleType::Enum ruleType;
        };
        const std::array<Mapping, 9> mappings = {
            Mapping{String{u8"integer"}, RuleType::Integer},
            Mapping{String{u8"DateTime"}, RuleType::DateTime},
            Mapping{String{u8"date_time"}, RuleType::DateTime},
            Mapping{String{u8"value_list"}, RuleType::ValueList},
            Mapping{String{u8"section_with_names"}, RuleType::Section},
            Mapping{String{u8"SECTION_WITH_TEXTS"}, RuleType::SectionWithTexts},
            Mapping{String{u8"notvalidated"}, RuleType::NotValidated},
            Mapping{String{u8"time_delta"}, RuleType::TimeDelta},
            Mapping{String{u8"regex"}, RuleType::RegEx}
        };

        for (const auto &mapping : mappings) {
            REQUIRE(RuleType::fromText(mapping.text) == mapping.ruleType);
        }

        REQUIRE(RuleType::fromText(String{}) == RuleType::Undefined);
        REQUIRE(RuleType::fromText(String{u8"unknown"}) == RuleType::Undefined);
        REQUIRE(RuleType::fromText(String{u8"123456789012345678901"}) == RuleType::Undefined);
    }

    void testAllEnumeration() {
        const std::array<RuleType, 19> expected = {
            RuleType::Undefined,
            RuleType::Integer,
            RuleType::Boolean,
            RuleType::Float,
            RuleType::Text,
            RuleType::Date,
            RuleType::Time,
            RuleType::DateTime,
            RuleType::Bytes,
            RuleType::TimeDelta,
            RuleType::RegEx,
            RuleType::Value,
            RuleType::ValueList,
            RuleType::ValueMatrix,
            RuleType::Section,
            RuleType::SectionList,
            RuleType::SectionWithTexts,
            RuleType::NotValidated,
            RuleType::Alternatives
        };
        const auto &values = RuleType::all();
        REQUIRE_EQUAL(values.size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            REQUIRE(values[i] == expected[i]);
        }
    }
};