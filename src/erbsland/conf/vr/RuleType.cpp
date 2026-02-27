// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RuleType.hpp"


#include "../impl/utf8/U8StringView.hpp"

#include <cassert>


namespace erbsland::conf::vr {


auto RuleType::all() noexcept -> const std::array<RuleType, 19> & {
    static const std::array<RuleType, 19> values = {
        Undefined,
        Integer,
        Boolean,
        Float,
        Text,
        Date,
        Time,
        DateTime,
        Bytes,
        TimeDelta,
        RegEx,
        Value,
        ValueList,
        ValueMatrix,
        Section,
        SectionList,
        SectionWithTexts,
        NotValidated,
        Alternatives};
    return values;
}


auto RuleType::valueToTextMap() noexcept -> const ValueMap& {
    static const ValueMap valueMap = {
        Entry{Undefined, String{u8"Undefined"}, ValueType::Undefined, String{}},
        Entry{Integer, String{u8"Integer"}, ValueType::Integer, String{u8"an integer value"}},
        Entry{Boolean, String{u8"Boolean"}, ValueType::Boolean, String{u8"a Boolean value"}},
        Entry{Float, String{u8"Float"}, ValueType::Float, String{u8"a floating-point or integer value"}},
        Entry{Text, String{u8"Text"}, ValueType::Text, String{u8"a text value"}},
        Entry{Date, String{u8"Date"}, ValueType::Date, String{u8"a date value"}},
        Entry{Time, String{u8"Time"}, ValueType::Time, String{u8"a time value"}},
        Entry{DateTime, String{u8"DateTime"}, ValueType::DateTime, String{u8"a date-time value"}},
        Entry{Bytes, String{u8"Bytes"}, ValueType::Bytes, String{u8"a byte value"}},
        Entry{TimeDelta, String{u8"TimeDelta"}, ValueType::TimeDelta, String{u8"a time-delta value"}},
        Entry{RegEx, String{u8"RegEx"}, ValueType::RegEx, String{u8"a regular expression"}},
        Entry{Value, String{u8"Value"}, ValueType::Undefined, String{u8"any scalar value"}},
        Entry{ValueList, String{u8"ValueList"}, ValueType::ValueList, String{u8"a value list or scalar value"}},
        Entry{ValueMatrix, String{u8"ValueMatrix"}, ValueType::Undefined, String{u8"a nested value list or scalar value"}},
        Entry{Section, String{u8"Section"}, ValueType::SectionWithNames, String{u8"a section"}},
        Entry{SectionList, String{u8"SectionList"}, ValueType::SectionList, String{u8"a section list"}},
        Entry{SectionWithTexts, String{u8"SectionWithTexts"}, ValueType::SectionWithTexts, String{u8"a section with texts"}},
        Entry{NotValidated, String{u8"NotValidated"}, ValueType::Undefined, String{}},
        Entry{Alternatives, String{u8"Alternatives"}, ValueType::Undefined, String{}},
    };
    return valueMap;
}


auto RuleType::textToValueMap() noexcept -> const TextToValueMap& {
    static const TextToValueMap textToValueMap = {
        {String{u8"integer"}, Integer},
        {String{u8"boolean"}, Boolean},
        {String{u8"float"}, Float},
        {String{u8"text"}, Text},
        {String{u8"date"}, Date},
        {String{u8"time"}, Time},
        {String{u8"datetime"}, DateTime},
        {String{u8"date_time"}, DateTime},
        {String{u8"bytes"}, Bytes},
        {String{u8"timedelta"}, TimeDelta},
        {String{u8"time_delta"}, TimeDelta},
        {String{u8"regex"}, RegEx},
        {String{u8"value"}, Value},
        {String{u8"valuelist"}, ValueList},
        {String{u8"value_list"}, ValueList},
        {String{u8"valuematrix"}, ValueMatrix},
        {String{u8"value_matrix"}, ValueMatrix},
        {String{u8"section"}, Section},
        {String{u8"sectionwithnames"}, Section},
        {String{u8"section_with_names"}, Section},
        {String{u8"sectionlist"}, SectionList},
        {String{u8"section_list"}, SectionList},
        {String{u8"sectionwithtexts"}, SectionWithTexts},
        {String{u8"section_with_texts"}, SectionWithTexts},
        {String{u8"notvalidated"}, NotValidated},
        {String{u8"not_validated"}, NotValidated},
    };
    return textToValueMap;
}


auto RuleType::matchesValueType(const ValueType valueType) const noexcept -> bool {
    switch (_value) {
    case Undefined:
        return false; // coverage: undefined rules should not exist.
    case Value:
        return valueType.isScalar();
    case ValueList:
    case ValueMatrix:
        return valueType == ValueType::ValueList || valueType.isScalar();
    case NotValidated: // coverage: should be handled with a custom logic.
    case Alternatives: // coverage: should be handled with a custom logic.
        return true;
    case Section:
        return valueType == ValueType::SectionWithNames || valueType == ValueType::IntermediateSection;
    default:
        return valueType == toValueType();
    }
}


auto RuleType::toText() const noexcept -> const String& {
    return entry().text;
}


auto RuleType::fromText(const String &text) noexcept -> RuleType {
    if (text.empty() || text.length() > 20) {
        return Undefined;
    }
    auto normalizedText = impl::U8StringView(text).transformed(
        [](const impl::Char c) -> impl::Char { return c.toRegularName(); });
    const auto &map = textToValueMap();
    const auto it = map.find(normalizedText);
    if (it == map.end()) {
        return Undefined;
    }
    return it->second;
}


auto RuleType::toValueType() const noexcept -> ValueType {
    return entry().valueType;
}


auto RuleType::expectedValueTypeText() const noexcept -> const String & {
    return entry().expectedValueTypeText;
}


auto RuleType::entry() const noexcept -> const Entry& {
    const auto &map = valueToTextMap();
    const auto it = std::ranges::find_if(map, [this](const Entry &entry) -> bool {
        return entry.value == _value;
    });
    assert(it != map.end());
    return *it;
}


}
