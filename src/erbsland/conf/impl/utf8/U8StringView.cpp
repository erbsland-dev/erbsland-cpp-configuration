// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
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
    U8Decoder{data}.decodeAll([&](const Char character) -> void {
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
    const auto elideSequenceCharacterCount = elideSequence.characterLength();
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


auto U8StringView::startsWith(const String &other, const Comparator &comparator) const -> bool {
    auto itA = U8Iterator::begin(_string);
    const auto itAEnd = U8Iterator::end(_string);
    auto itB = U8Iterator::begin(other);
    const auto itBEnd = U8Iterator::end(other);
    while (itA != itAEnd && itB != itBEnd) {
        if (comparator(*itA, *itB) != std::strong_ordering::equal) {
            return false;
        }
        ++itA;
        ++itB;
    }
    return itB == itBEnd;
}


auto U8StringView::contains(const String &other, const Comparator &comparator) const -> bool {
    auto hayItStart = U8Iterator::begin(_string);
    const auto hayEnd = U8Iterator::end(_string);
    auto needleBegin = U8Iterator::begin(other);
    const auto needleEnd = U8Iterator::end(other);

    // Empty needle always contained
    if (needleBegin == needleEnd) {
        return true;
    }

    for (auto it = hayItStart; it != hayEnd; ++it) {
        auto hayIt = it;
        auto needleIt = needleBegin;
        while (hayIt != hayEnd && needleIt != needleEnd && comparator(*hayIt, *needleIt) == std::strong_ordering::equal) {
            ++hayIt;
            ++needleIt;
        }
        if (needleIt == needleEnd) {
            return true;
        }
        // otherwise continue with next starting position
    }
    return false;
}


auto U8StringView::firstByteIndex(const Char character, std::optional<std::size_t> fromByteIndex) const
    -> std::size_t {
    const auto startByteIndex = fromByteIndex.value_or(0);
    if (startByteIndex > _string.size()) {
        throw std::range_error{"The start position is outside the string."};
    }
    if (_string.empty() || startByteIndex == _string.size()) {
        return std::u8string_view::npos;
    }
    std::size_t position = 0;
    const auto buffer = std::span(_string.data(), _string.size());
    while (position < buffer.size()) {
        const auto charStart = position;
        const auto decoded = U8Decoder<const char8_t>::decodeChar(buffer, position);
        if (charStart < startByteIndex && position > startByteIndex) {
            throw std::range_error{"The start position is inside a UTF-8 sequence."};
        }
        if (charStart >= startByteIndex && decoded == character) {
            return charStart;
        }
    }
    return std::u8string_view::npos;
}


auto U8StringView::split(const Char character, std::optional<std::size_t> maxSplits) const -> StringList {
    StringList result;
    if (_string.empty()) {
        result.emplace_back();
        return result;
    }
    const auto buffer = std::span(_string.data(), _string.size());
    std::size_t position = 0;
    std::size_t segmentStart = 0;
    std::size_t splitCount = 0;
    while (position < buffer.size()) {
        const auto charStart = position;
        const auto decoded = U8Decoder<const char8_t>::decodeChar(buffer, position);
        const auto canSplit = !maxSplits || splitCount < *maxSplits;
        if (canSplit && decoded == character) {
            result.emplace_back(_string.substr(segmentStart, charStart - segmentStart));
            segmentStart = position;
            ++splitCount;
        }
    }
    result.emplace_back(_string.substr(segmentStart));
    return result;
}


auto U8StringView::join(const StringList &parts) const noexcept -> String {
    if (parts.empty()) {
        return {};
    }
    std::size_t totalSize = 0;
    for (const auto &part : parts) {
        totalSize += part.size();
    }
    if (parts.size() > 1) {
        totalSize += (parts.size() - 1) * _string.size();
    }
    String result;
    result.reserve(totalSize);
    for (std::size_t index = 0; index < parts.size(); ++index) {
        if (index > 0) {
            result.append(_string);
        }
        result.append(parts[index]);
    }
    return result;
}


auto U8StringView::endsWith(const String &other, const Comparator &comparator) const -> bool {
    auto itA = U8Iterator::begin(_string);
    const auto itAEnd = U8Iterator::end(_string);
    auto itB = U8Iterator::begin(other);
    const auto itBEnd = U8Iterator::end(other);
    while (itA != itAEnd) {
        if (itB == itBEnd || comparator(*itA, *itB) != std::strong_ordering::equal) {
            itB = U8Iterator::begin(other);
        } else {
            ++itB; // FIXME! Check with unittest!!!
        }
        ++itA;
    }
    return itA == itAEnd && itB == itBEnd;
}


auto U8StringView::transformed(const CharTransformer &transformer) const -> String {
    String result;
    result.reserve(_string.size());
    U8Decoder{_string}.decodeAll([&](const Char character) -> void {
        result.append(transformer(character));
    });
    return result;
}


auto U8StringView::transformed32(const CharTransformer32 &transformer) const -> String {
    String result;
    result.reserve(_string.size());
    U8Decoder{_string}.decodeAll([&](const Char character) -> void {
        result.append(Char{transformer(character.raw())});
    });
    return result;
}


void U8StringView::forEachChar(const CharFunction &fn) const {
    U8Decoder{_string}.decodeAll([&](const Char character) -> void {
        fn(character);
    });
}


void U8StringView::forEachChar32(const CharFunction32 &fn) const {
    U8Decoder{_string}.decodeAll([&](const Char character) -> void {
        fn(character.raw());
    });
}


auto U8StringView::escapedSize(const EscapeMode mode) const noexcept -> std::size_t {
    std::size_t expectedSize = 0;
    U8Decoder{_string}.decodeAll([&](const Char character) -> void {
        expectedSize += character.escapedUtf8Size(mode);
    });
    return expectedSize;

}


auto U8StringView::toEscaped(const EscapeMode mode) const noexcept -> String {
    String result;
    result.reserve(escapedSize(mode));
    U8Decoder{_string}.decodeAll([&](const Char character) -> void {
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

