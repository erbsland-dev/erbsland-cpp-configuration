// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "U8StringView.hpp"


#include "U8Decoder.hpp"
#include "U8Iterator.hpp"


namespace erbsland::conf::impl {


auto U8StringView::isValid() const noexcept -> bool {
    return U8Decoder{_string}.verify();
}


auto U8StringView::toBytes() const noexcept -> Bytes {
    return Bytes::convertFrom(_string.begin(), _string.end());
}


auto U8StringView::fromBytes(const Bytes &data) -> String {
    String result;
    result.reserve(data.size());
    U8Decoder{data}.decodeAll([&](const Char character) {
        character.appendTo(result);
    });
    return result;
}


auto U8StringView::length() const -> std::size_t {
    return U8Decoder{_string}.countAll();
}


auto U8StringView::startForChar(const std::size_t charIndex) const -> std::size_t {
    return U8Decoder{_string}.startByte(charIndex);
}


auto U8StringView::truncatedWithElide(
    const std::size_t maximumCharacters,
    const ElideLocation elideLocation,
    const String &elideSequence) const -> String {

    const auto characterCount = length();
    if (characterCount <= maximumCharacters) {
        return String{_string};
    }
    const auto elideSequenceCharacterCount = U8StringView{elideSequence}.length();
    if (maximumCharacters < elideSequenceCharacterCount + 2) {
        throw std::invalid_argument{"The maximum number of characters must be at least the length of the elide sequence plus two."};
    }
    String result;
    if (elideLocation == ElideLocation::Begin) {
        result.append(elideSequence);
        const auto endPartIndex = characterCount - maximumCharacters + elideSequenceCharacterCount;
        const auto endPartPos = startForChar(endPartIndex);
        result.append(_string.substr(endPartPos));
    } else if (elideLocation == ElideLocation::End) {
        const auto beginPartIndex = maximumCharacters - elideSequenceCharacterCount;
        const auto beginPartPos = startForChar(beginPartIndex);
        result.append(_string.substr(0, beginPartPos));
        result.append(elideSequence);
    } else {
        const auto maximumWithoutElideSequence = maximumCharacters - elideSequenceCharacterCount;
        const auto beginPartMaximum = maximumWithoutElideSequence / 2;
        const auto beginPartIndex = beginPartMaximum;
        const auto beginPartPos = startForChar(beginPartIndex);
        const auto endPartMaximum = maximumWithoutElideSequence - beginPartMaximum;
        const auto endPartIndex = characterCount - endPartMaximum;
        const auto endPartPos = startForChar(endPartIndex);
        result.append(_string.substr(0, beginPartPos));
        result.append(elideSequence);
        result.append(_string.substr(endPartPos));
    }
    return result;
}


auto U8StringView::compare(const String &other, const Comparator &comparator) const -> std::strong_ordering {
    auto itA = U8Iterator::begin(_string);
    const auto itAEnd = U8Iterator::end(_string);
    auto itB = U8Iterator::begin(other);
    const auto itBEnd = U8Iterator::end(other);
    while (itA != itAEnd && itB != itBEnd) {
        const auto result = comparator(*itA, *itB);
        if (result != std::strong_ordering::equal) {
            return result;
        }
        ++itA;
        ++itB;
    }
    if (itA == itAEnd && itB == itBEnd) {
        return std::strong_ordering::equal;
    }
    if (itA == itAEnd) {
        return std::strong_ordering::less;
    }
    return std::strong_ordering::greater;
}


auto U8StringView::transformed(const CharTransformer &transformer) const -> String {
    String result;
    result.reserve(_string.size());
    U8Decoder{_string}.decodeAll([&](const Char character) {
        result.append(transformer(character));
    });
    return result;
}


void U8StringView::forEachChar(const CharFunction &fn) const {
    U8Decoder{_string}.decodeAll([&](const Char character) {
        fn(character);
    });
}


auto U8StringView::escapedSize(const EscapeMode mode) const noexcept -> std::size_t {
    std::size_t expectedSize = 0;
    U8Decoder{_string}.decodeAll([&](const Char character) {
        expectedSize += character.escapedUtf8Size(mode);
    });
    return expectedSize;

}


auto U8StringView::toEscaped(const EscapeMode mode) const noexcept -> String {
    String result;
    result.reserve(escapedSize(mode));
    U8Decoder{_string}.decodeAll([&](const Char character) {
        character.appendEscaped(result, mode);
    });
    return result;
}


auto U8StringView::toSafeText(const std::size_t maximumSize, const ElideLocation elideLocation) const -> String {
    if (!isValid()) {
        return {u8"<contains UTF-8 encoding errors>"};
    }
    const auto safeText = toEscaped(EscapeMode::ErrorText);
    return U8StringView{safeText}.truncatedWithElide(maximumSize, elideLocation);
}


}

