// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/ValueType.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <unordered_map>
#include <stdexcept>
#include <array>
#include <string>


using namespace el::conf;


TESTED_TARGETS(ValueType)
class ValueTypeTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultConstructor() {
        ValueType vt;

        // The default unit should be Undefined
        REQUIRE(vt == ValueType::Undefined);
        REQUIRE_FALSE(vt != ValueType::Undefined);
        REQUIRE(vt.isUndefined());
    }

    void testParameterizedConstructor() {
        ValueType vtUndefined(ValueType::Undefined);
        ValueType vtInteger(ValueType::Integer);
        ValueType vtBoolean(ValueType::Boolean);
        ValueType vtFloat(ValueType::Float);
        ValueType vtText(ValueType::Text);
        ValueType vtDate(ValueType::Date);
        ValueType vtTime(ValueType::Time);
        ValueType vtDateTime(ValueType::DateTime);
        ValueType vtBytes(ValueType::Bytes);
        ValueType vtTimeDelta(ValueType::TimeDelta);
        ValueType vtRegEx(ValueType::RegEx);
        ValueType vtValueList(ValueType::ValueList);
        ValueType vtSectionList(ValueType::SectionList);
        ValueType vtIntermediateSection(ValueType::IntermediateSection);
        ValueType vtSectionWithNames(ValueType::SectionWithNames);
        ValueType vtSectionWithTexts(ValueType::SectionWithTexts);

        // Verify each constructed ValueType
        REQUIRE(vtUndefined == ValueType::Undefined);
        REQUIRE(vtInteger == ValueType::Integer);
        REQUIRE(vtBoolean == ValueType::Boolean);
        REQUIRE(vtFloat == ValueType::Float);
        REQUIRE(vtText == ValueType::Text);
        REQUIRE(vtDate == ValueType::Date);
        REQUIRE(vtTime == ValueType::Time);
        REQUIRE(vtDateTime == ValueType::DateTime);
        REQUIRE(vtBytes == ValueType::Bytes);
        REQUIRE(vtTimeDelta == ValueType::TimeDelta);
        REQUIRE(vtRegEx == ValueType::RegEx);
        REQUIRE(vtValueList == ValueType::ValueList);
        REQUIRE(vtSectionList == ValueType::SectionList);
        REQUIRE(vtIntermediateSection == ValueType::IntermediateSection);
        REQUIRE(vtSectionWithNames == ValueType::SectionWithNames);
        REQUIRE(vtSectionWithTexts == ValueType::SectionWithTexts);
    }

    void testAssignmentFromEnum() {
        ValueType vt;
        vt = ValueType::Float;

        // The unit should now be Float
        REQUIRE(vt == ValueType::Float);
        REQUIRE_FALSE(vt != ValueType::Float);
    }

    void testAssignmentToEnum() {
        ValueType vt(ValueType::Boolean);
        ValueType::Enum enumVal = vt;

        // The enum value should match
        REQUIRE(enumVal == ValueType::Boolean);
    }

    void testOperators() {
        WITH_CONTEXT(requireAllOperators<ValueType, ValueType>(
            ValueType::Integer, ValueType::Boolean, ValueType::SectionWithTexts,
            ValueType::Integer, ValueType::Boolean, ValueType::SectionWithTexts
        ));
        WITH_CONTEXT(requireAllOperators<ValueType, ValueType::Enum>(
            ValueType::Integer, ValueType::Boolean, ValueType::SectionWithTexts,
            ValueType::Integer, ValueType::Boolean, ValueType::SectionWithTexts
        ));
        WITH_CONTEXT(requireAllOperators<ValueType::Enum, ValueType>(
            ValueType::Integer, ValueType::Boolean, ValueType::SectionWithTexts,
            ValueType::Integer, ValueType::Boolean, ValueType::SectionWithTexts
        ));
    }

    void testOrder() {
        WITH_CONTEXT(requireStrictOrder(std::array<ValueType, 16>({
            ValueType::Undefined,
            ValueType::Integer,
            ValueType::Boolean,
            ValueType::Float,
            ValueType::Text,
            ValueType::Date,
            ValueType::Time,
            ValueType::DateTime,
            ValueType::Bytes,
            ValueType::TimeDelta,
            ValueType::RegEx,
            ValueType::ValueList,
            ValueType::SectionList,
            ValueType::IntermediateSection,
            ValueType::SectionWithNames,
            ValueType::SectionWithTexts
        })));
        WITH_CONTEXT(requireStrictOrder(ValueType::all()));
    }

    void testConversionToEnum() {
        ValueType vtText(ValueType::Text);
        ValueType::Enum enumVal = static_cast<ValueType::Enum>(vtText);

        REQUIRE(enumVal == ValueType::Text);
    }

    void testIsUndefined() {
        ValueType vtUndefined;
        ValueType vtInteger(ValueType::Integer);

        REQUIRE(vtUndefined.isUndefined());
        REQUIRE_FALSE(vtInteger.isUndefined());
    }

    void testIsSection() {
        ValueType vtIntermediateSection(ValueType::IntermediateSection);
        ValueType vtSectionWithNames(ValueType::SectionWithNames);
        ValueType vtSectionWithTexts(ValueType::SectionWithTexts);
        ValueType vtValueList(ValueType::ValueList);
        ValueType vtInteger(ValueType::Integer);

        // These should be sections
        REQUIRE(vtIntermediateSection.isMap());
        REQUIRE(vtSectionWithNames.isMap());
        REQUIRE(vtSectionWithTexts.isMap());

        // These should not be sections
        REQUIRE_FALSE(vtValueList.isMap());
        REQUIRE_FALSE(vtInteger.isMap());
    }

    void testIsList() {
        ValueType vtValueList(ValueType::ValueList);
        ValueType vtSectionList(ValueType::SectionList);
        ValueType vtInteger(ValueType::Integer);
        ValueType vtIntermediateSection(ValueType::IntermediateSection);

        REQUIRE(vtValueList.isList());
        REQUIRE(vtSectionList.isList());

        REQUIRE_FALSE(vtInteger.isList());
        REQUIRE_FALSE(vtIntermediateSection.isList());
    }

    void testIsSingle() {
        ValueType vtInteger(ValueType::Integer);
        ValueType vtBoolean(ValueType::Boolean);
        ValueType vtFloat(ValueType::Float);
        ValueType vtText(ValueType::Text);
        ValueType vtDate(ValueType::Date);
        ValueType vtTime(ValueType::Time);
        ValueType vtDateTime(ValueType::DateTime);
        ValueType vtBytes(ValueType::Bytes);
        ValueType vtTimeDelta(ValueType::TimeDelta);
        ValueType vtRegEx(ValueType::RegEx);
        ValueType vtValueList(ValueType::ValueList);
        ValueType vtIntermediateSection(ValueType::IntermediateSection);

        // These should be single value types
        REQUIRE(vtInteger.isScalar());
        REQUIRE(vtBoolean.isScalar());
        REQUIRE(vtFloat.isScalar());
        REQUIRE(vtText.isScalar());
        REQUIRE(vtDate.isScalar());
        REQUIRE(vtTime.isScalar());
        REQUIRE(vtDateTime.isScalar());
        REQUIRE(vtBytes.isScalar());
        REQUIRE(vtTimeDelta.isScalar());
        REQUIRE(vtRegEx.isScalar());

        // These should not be single value types
        REQUIRE_FALSE(vtValueList.isScalar());
        REQUIRE_FALSE(vtIntermediateSection.isScalar());
    }

    void testToText() {
        ValueType vtUndefined(ValueType::Undefined);
        ValueType vtInteger(ValueType::Integer);
        ValueType vtBoolean(ValueType::Boolean);
        ValueType vtFloat(ValueType::Float);
        ValueType vtText(ValueType::Text);
        ValueType vtDate(ValueType::Date);
        ValueType vtTime(ValueType::Time);
        ValueType vtDateTime(ValueType::DateTime);
        ValueType vtBytes(ValueType::Bytes);
        ValueType vtTimeDelta(ValueType::TimeDelta);
        ValueType vtRegEx(ValueType::RegEx);
        ValueType vtValueList(ValueType::ValueList);
        ValueType vtSectionList(ValueType::SectionList);
        ValueType vtIntermediateSection(ValueType::IntermediateSection);
        ValueType vtSectionWithNames(ValueType::SectionWithNames);
        ValueType vtSectionWithTexts(ValueType::SectionWithTexts);

        // Verify toText() returns correct string representations
        REQUIRE(vtUndefined.toText() == u8"Undefined");
        REQUIRE(vtInteger.toText() == u8"Integer");
        REQUIRE(vtBoolean.toText() == u8"Boolean");
        REQUIRE(vtFloat.toText() == u8"Float");
        REQUIRE(vtText.toText() == u8"Text");
        REQUIRE(vtDate.toText() == u8"Date");
        REQUIRE(vtTime.toText() == u8"Time");
        REQUIRE(vtDateTime.toText() == u8"DateTime");
        REQUIRE(vtBytes.toText() == u8"Bytes");
        REQUIRE(vtTimeDelta.toText() == u8"TimeDelta");
        REQUIRE(vtRegEx.toText() == u8"RegEx");
        REQUIRE(vtValueList.toText() == u8"ValueList");
        REQUIRE(vtSectionList.toText() == u8"SectionList");
        REQUIRE(vtIntermediateSection.toText() == u8"IntermediateSection");
        REQUIRE(vtSectionWithNames.toText() == u8"SectionWithNames");
        REQUIRE(vtSectionWithTexts.toText() == u8"SectionWithTexts");
    }

    // Test the hash specialization by using ValueType as a key in unordered_map
    void testHashSpecialization() {
        std::unordered_map<ValueType, std::string> vtMap;

        // Insert all ValueType enums with their toText() as values
        for (const auto& enumVal : ValueType::all()) {
            ValueType vt(enumVal);
            vtMap[vt] = vt.toText().toCharString();
        }

        // Verify that all inserted keys are present with correct values
        for (const auto& enumVal : ValueType::all()) {
            ValueType vt(enumVal);
            auto it = vtMap.find(vt);
            REQUIRE(it != vtMap.end());
            REQUIRE(it->second == vt.toText().toCharString());
        }

        // Verify specific entries
        ValueType vtInteger(ValueType::Integer);
        REQUIRE(vtMap.find(vtInteger) != vtMap.end());
        REQUIRE(vtMap[vtInteger] == "Integer");

        ValueType vtSectionWithTexts(ValueType::SectionWithTexts);
        REQUIRE(vtMap.find(vtSectionWithTexts) != vtMap.end());
        REQUIRE(vtMap[vtSectionWithTexts] == "SectionWithTexts");
    }

    void testEnumerationCompleteness() {
        constexpr std::array expectedEnums = {
            ValueType::Undefined,
            ValueType::Integer,
            ValueType::Boolean,
            ValueType::Float,
            ValueType::Text,
            ValueType::Date,
            ValueType::Time,
            ValueType::DateTime,
            ValueType::Bytes,
            ValueType::TimeDelta,
            ValueType::RegEx,
            ValueType::ValueList,
            ValueType::SectionList,
            ValueType::IntermediateSection,
            ValueType::SectionWithNames,
            ValueType::SectionWithTexts
        };

        // Verify that each enum is handled correctly
        for (const auto& enumVal : expectedEnums) {
            ValueType const vt(enumVal);
            REQUIRE_FALSE(vt.toText().empty());
        }
    }

    void testDeriveFromNativeType() {
        ValueType vt;
        vt = ValueType::from<int8_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<int16_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<int32_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<int64_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<uint8_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<uint16_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<uint32_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<uint64_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<float>();
        REQUIRE_EQUAL(vt, ValueType::Float);
        vt = ValueType::from<double>();
        REQUIRE_EQUAL(vt, ValueType::Float);
        vt = ValueType::from<bool>();
        REQUIRE_EQUAL(vt, ValueType::Boolean);
        vt = ValueType::from<std::string>();
        REQUIRE_EQUAL(vt, ValueType::Text);
        vt = ValueType::from<std::u8string>();
        REQUIRE_EQUAL(vt, ValueType::Text);
        vt = ValueType::from<String>();
        REQUIRE_EQUAL(vt, ValueType::Text);
        vt = ValueType::from<Date>();
        REQUIRE_EQUAL(vt, ValueType::Date);
        vt = ValueType::from<Time>();
        REQUIRE_EQUAL(vt, ValueType::Time);
        vt = ValueType::from<DateTime>();
        REQUIRE_EQUAL(vt, ValueType::DateTime);
        vt = ValueType::from<Bytes>();
        REQUIRE_EQUAL(vt, ValueType::Bytes);
        vt = ValueType::from<TimeDelta>();
        REQUIRE_EQUAL(vt, ValueType::TimeDelta);

        vt = ValueType::from<const int8_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const int16_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const int32_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const int64_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const uint8_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const uint16_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const uint32_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const uint64_t>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const float>();
        REQUIRE_EQUAL(vt, ValueType::Float);
        vt = ValueType::from<const double>();
        REQUIRE_EQUAL(vt, ValueType::Float);
        vt = ValueType::from<const bool>();
        REQUIRE_EQUAL(vt, ValueType::Boolean);
        vt = ValueType::from<const std::string>();
        REQUIRE_EQUAL(vt, ValueType::Text);
        vt = ValueType::from<const std::u8string>();
        REQUIRE_EQUAL(vt, ValueType::Text);
        vt = ValueType::from<const String>();
        REQUIRE_EQUAL(vt, ValueType::Text);
        vt = ValueType::from<const Date>();
        REQUIRE_EQUAL(vt, ValueType::Date);
        vt = ValueType::from<const Time>();
        REQUIRE_EQUAL(vt, ValueType::Time);
        vt = ValueType::from<const DateTime>();
        REQUIRE_EQUAL(vt, ValueType::DateTime);
        vt = ValueType::from<const Bytes>();
        REQUIRE_EQUAL(vt, ValueType::Bytes);
        vt = ValueType::from<const TimeDelta>();
        REQUIRE_EQUAL(vt, ValueType::TimeDelta);

        vt = ValueType::from<const int8_t&>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const int16_t&>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const int32_t&>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const int64_t&>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const uint8_t&>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const uint16_t&>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const uint32_t&>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const uint64_t&>();
        REQUIRE_EQUAL(vt, ValueType::Integer);
        vt = ValueType::from<const float&>();
        REQUIRE_EQUAL(vt, ValueType::Float);
        vt = ValueType::from<const double&>();
        REQUIRE_EQUAL(vt, ValueType::Float);
        vt = ValueType::from<const bool&>();
        REQUIRE_EQUAL(vt, ValueType::Boolean);
        vt = ValueType::from<const std::string&>();
        REQUIRE_EQUAL(vt, ValueType::Text);
        vt = ValueType::from<const std::u8string&>();
        REQUIRE_EQUAL(vt, ValueType::Text);
        vt = ValueType::from<const String&>();
        REQUIRE_EQUAL(vt, ValueType::Text);
        vt = ValueType::from<const Date&>();
        REQUIRE_EQUAL(vt, ValueType::Date);
        vt = ValueType::from<const Time&>();
        REQUIRE_EQUAL(vt, ValueType::Time);
        vt = ValueType::from<const DateTime&>();
        REQUIRE_EQUAL(vt, ValueType::DateTime);
        vt = ValueType::from<const Bytes&>();
        REQUIRE_EQUAL(vt, ValueType::Bytes);
        vt = ValueType::from<const TimeDelta&>();
        REQUIRE_EQUAL(vt, ValueType::TimeDelta);
    }
};

