// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DependencyMode.hpp"


#include "../utf8/U8StringView.hpp"


namespace erbsland::conf::impl {


auto DependencyMode::isValid(const bool hasSource, const bool hasTarget) const noexcept -> bool {
    if (!hasSource && !hasTarget) {
        return (_value & AllowNone) != 0;
    }
    if (hasSource && !hasTarget) {
        return (_value & AllowOnlySources) != 0;
    }
    if (!hasSource) {
        return (_value & AllowOnlyTargets) != 0;
    }
    return (_value & AllowBoth) != 0;
}


auto DependencyMode::toText() const noexcept -> const String& {
    for (const auto &entry : textToValueMap()) {
        if (entry.second == _value) {
            return entry.first;
        }
    }
    static const String undefinedText{u8"undefined"};
    return undefinedText;
}


auto DependencyMode::fromText(const String &text) noexcept -> DependencyMode {
    if (text.empty() || text.length() > 20) {
        return Undefined;
    }
    for (const auto &entry : textToValueMap()) {
        if (entry.first.nameCompare(text) == std::strong_ordering::equal) {
            return entry.second;
        }
    }
    return Undefined;
}


auto DependencyMode::textToValueMap() noexcept -> const TextToValueMap& {
    static const TextToValueMap map = {
        {String{u8"if"}, If},
        {String{u8"if_not"}, IfNot},
        {String{u8"or"}, OR},
        {String{u8"xnor"}, XNOR},
        {String{u8"xor"}, XOR},
        {String{u8"and"}, AND},
    };
    return map;
}


}


