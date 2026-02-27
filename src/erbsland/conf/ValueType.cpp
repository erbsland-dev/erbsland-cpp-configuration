// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ValueType.hpp"


#include <algorithm>
#include <cassert>
#include <ranges>


namespace erbsland::conf {


ValueType::ValueToTextMap ValueType::_valueToTextMap = {
    std::make_tuple(Undefined,              String{u8"Undefined"}),
    std::make_tuple(Integer,                String{u8"Integer"}),
    std::make_tuple(Boolean,                String{u8"Boolean"}),
    std::make_tuple(Float,                  String{u8"Float"}),
    std::make_tuple(Text,                   String{u8"Text"}),
    std::make_tuple(Date,                   String{u8"Date"}),
    std::make_tuple(Time,                   String{u8"Time"}),
    std::make_tuple(DateTime,               String{u8"DateTime"}),
    std::make_tuple(Bytes,                  String{u8"Bytes"}),
    std::make_tuple(TimeDelta,              String{u8"TimeDelta"}),
    std::make_tuple(RegEx,                  String{u8"RegEx"}),
    std::make_tuple(ValueList,              String{u8"ValueList"}),
    std::make_tuple(SectionList,            String{u8"SectionList"}),
    std::make_tuple(IntermediateSection,    String{u8"IntermediateSection"}),
    std::make_tuple(SectionWithNames,       String{u8"SectionWithNames"}),
    std::make_tuple(SectionWithTexts,       String{u8"SectionWithTexts"}),
    std::make_tuple(Document,               String{u8"Document"}),
};


auto ValueType::toText() const noexcept -> const String& {
    auto it = std::ranges::find_if(_valueToTextMap, [this](const ValueToTextEntry &entry) -> bool {
        return std::get<0>(entry) == _value;
    });
    assert(it != _valueToTextMap.end());
    return std::get<1>(*it);
}


auto ValueType::toValueDescription(const bool withArticle) const -> String {
    String result;
    switch (_value) {
    case Integer: result = u8"integer value"; break;
    case Boolean: result = u8"boolean value"; break;
    case Float: result = u8"floating-point value"; break;
    case Text: result = u8"text value"; break;
    case Date: result = u8"date value"; break;
    case Time: result = u8"time value"; break;
    case DateTime: result = u8"date-time value"; break;
    case Bytes: result = u8"bytes value"; break;
    case TimeDelta: result = u8"time-delta value"; break;
    case RegEx: result = u8"regular expression"; break;
    case ValueList: result = u8"value list"; break;
    case SectionList: result = u8"section list"; break;
    case IntermediateSection: result = u8"intermediate section"; break;
    case SectionWithNames: result = u8"section"; break;
    case SectionWithTexts: result = u8"section with texts"; break;
    case Document: result = u8"document"; break;
    default: break;
    }
    if (withArticle && !result.empty()) {
        if (result.starts_with(u8"i")) {
            result = String{u8"an "} + result;
        } else {
            result = String{u8"a "} + result;
        }
    }
    return result;
}


auto ValueType::all() noexcept -> const std::array<ValueType, 17>& {
    static const std::array<ValueType, 17> values = {
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
        ValueList,
        SectionList,
        IntermediateSection,
        SectionWithNames,
        SectionWithTexts,
        Document};
    return values;
}


}
