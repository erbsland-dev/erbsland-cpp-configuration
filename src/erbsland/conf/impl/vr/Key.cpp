// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Key.hpp"


#include "../char/Char.hpp"
#include "../utf8/U8StringView.hpp"
#include "../utilities/HashHelper.hpp"


namespace erbsland::conf::impl {


auto Key::isEqual(
    const Key &other,
    const CaseSensitivity caseSensitivity) const noexcept -> bool {

    const auto maxSize = std::max(_elements.size(), other._elements.size());
    for (std::size_t i = 0; i < maxSize; ++i) {
        if (!isEqual(other, caseSensitivity, i)) {
            return false;
        }
    }
    return true;
}


auto Key::isEqual(
    const Key &other,
    const CaseSensitivity caseSensitivity,
    const std::size_t index) const noexcept -> bool {

    if (caseSensitivity == CaseSensitivity::CaseSensitive) {
        return element(index) == other.element(index);
    }
    return element(index).characterCompare(other.element(index), CaseSensitivity::CaseInsensitive) == std::strong_ordering::equal;
}


auto Key::elements() const noexcept -> const std::vector<String> & {
    return _elements;
}


auto Key::element(const std::size_t index) const noexcept -> const String & {
    static const String empty{};
    if (index >= _elements.size()) {
        return empty;
    }
    return _elements[index];
}


auto Key::toText() const noexcept -> String {
    return String{u8","}.join(_elements);
}


auto Key::size() const noexcept -> std::size_t {
    return _elements.size();
}


auto Key::hash(const CaseSensitivity caseSensitivity) const noexcept -> std::size_t {
    std::size_t hash = 0;
    if (caseSensitivity == CaseSensitivity::CaseSensitive) {
        for (const auto &element : _elements) {
            hashCombine(hash, std::hash<String>{}(element));
        }
    } else {
        for (const auto &element : _elements) {
            U8StringView{element}.forEachChar([&hash](const Char character) -> void {
                hashCombine(hash, std::hash<char32_t>{}(character.toLowerCase().raw()));
            });
        }
    }
    return hash;
}


auto Key::elementHash(const String &element, const CaseSensitivity caseSensitivity) noexcept -> std::size_t {
    if (caseSensitivity == CaseSensitivity::CaseSensitive) {
        return std::hash<String>{}(element);
    }
    std::size_t hash = 0;
    U8StringView{element}.forEachChar([&hash](const Char character) -> void {
        hashCombine(hash, std::hash<char32_t>{}(character.toLowerCase().raw()));
    });
    return hash;
}


}
