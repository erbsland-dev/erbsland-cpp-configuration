// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>
#include <format>


namespace erbsland::conf {


/// The type of name.
///
/// @tested `NameTypeTest`
///
enum class NameType : uint8_t {
    /// A regular name: name
    Regular,
    /// A text name: "text"
    Text,
    /// An index name: [<index>]
    Index,
    /// A text index name: ""[<index>]
    TextIndex,
};


[[nodiscard]] constexpr auto toStringView(const NameType nameType) noexcept -> std::string_view {
    switch (nameType) {
    case NameType::Regular:
        return "Regular";
    case NameType::Text:
        return "Text";
    case NameType::Index:
        return "Index";
    case NameType::TextIndex:
        return "TextIndex";
    }
    return {};
}


}


template<>
struct std::formatter<erbsland::conf::NameType> : std::formatter<std::string_view> {
    auto format(const erbsland::conf::NameType nameType, format_context &ctx) const {
        return formatter<string_view>::format(toStringView(nameType), ctx);
    }
};


