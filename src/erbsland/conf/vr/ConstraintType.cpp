// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ConstraintType.hpp"


#include "../impl/utf8/U8StringView.hpp"

#include <cassert>


namespace erbsland::conf::vr {


ConstraintType::ValueToTextMap ConstraintType::_valueToTextMap = {
    ValueToTextEntry(Undefined, String{u8"Undefined"}),
    ValueToTextEntry(Chars, String{u8"Chars"}),
    ValueToTextEntry(Contains, String{u8"Contains"}),
    ValueToTextEntry(Ends, String{u8"Ends"}),
    ValueToTextEntry(Equals, String{u8"Equals"}),
    ValueToTextEntry(In, String{u8"In"}),
    ValueToTextEntry(Key, String{u8"Key"}),
    ValueToTextEntry(Matches, String{u8"Matches"}),
    ValueToTextEntry(Maximum, String{u8"Maximum"}),
    ValueToTextEntry(MaximumVersion, String{u8"MaximumVersion"}),
    ValueToTextEntry(Minimum, String{u8"Minimum"}),
    ValueToTextEntry(MinimumVersion, String{u8"MinimumVersion"}),
    ValueToTextEntry(Multiple, String{u8"Multiple"}),
    ValueToTextEntry(Starts, String{u8"Starts"}),
    ValueToTextEntry(Version, String{u8"Version"}),
};


ConstraintType::TextToValueMap ConstraintType::_textToValueMap = {
    {String{u8"chars"}, Chars},
    {String{u8"contains"}, Contains},
    {String{u8"equals"}, Equals},
    {String{u8"ends"}, Ends},
    {String{u8"in"}, In},
    {String{u8"key"}, Key},
    {String{u8"matches"}, Matches},
    {String{u8"maximum"}, Maximum},
    {String{u8"maximum_version"}, MaximumVersion},
    {String{u8"minimum"}, Minimum},
    {String{u8"minimum_version"}, MinimumVersion},
    {String{u8"multiple"}, Multiple},
    {String{u8"starts"}, Starts},
    {String{u8"version"}, Version},
};


auto ConstraintType::toText() const noexcept -> const String & {
    auto it = std::ranges::find_if(
        _valueToTextMap, [this](const ValueToTextEntry &entry) -> bool {
        return entry.value == _value;
    });
    assert(it != _valueToTextMap.end());
    return it->text;
}


auto ConstraintType::fromText(const String &text) noexcept -> ConstraintType {
    if (text.empty() || text.length() > 20) {
        return Undefined;
    }
    auto normalizedText = impl::U8StringView(text).transformed(
        [](const impl::Char c) -> impl::Char { return c.toRegularName(); });
    const auto it = _textToValueMap.find(normalizedText);
    if (it == _textToValueMap.end()) {
        return Undefined;
    }
    return it->second;
}


}
