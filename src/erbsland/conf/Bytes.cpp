// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Bytes.hpp"


#include "impl/utilities/HashHelper.hpp"

#include <cstring>


namespace erbsland::conf {


namespace {
auto isWhitespace(const char c) noexcept -> bool {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}
auto isHexDigit(const char c) noexcept -> bool {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
auto digitToByte(const char digit) noexcept -> std::byte {
    if (digit >= '0' && digit <= '9') {
        return static_cast<std::byte>(digit - '0');
    }
    if (digit >= 'a' && digit <= 'f') {
        return static_cast<std::byte>(digit - 'a' + 10);
    }
    if (digit >= 'A' && digit <= 'F') {
        return static_cast<std::byte>(digit - 'A' + 10);
    }
    return std::byte{0xff};
}
auto byteToDigit(const std::byte byte) noexcept -> char {
    if (byte <= std::byte{0x09}) {
        return static_cast<char>(byte) + '0';
    }
    return static_cast<char>(byte) - 10 + 'a';
}

template<std::input_iterator It>
auto convertFromHex(It begin, It end) noexcept -> Bytes {
    std::vector<std::byte> result;
    if (const auto dist = std::distance(begin, end); dist > 0) {
        result.reserve(static_cast<std::size_t>(dist) / 2U);
    }

    auto skipSpaces = [&]() {
        while (begin != end && isWhitespace(static_cast<char>(*begin))) {
            ++begin;
        }
    };

    skipSpaces();
    while (begin != end) {
        if (!isHexDigit(static_cast<char>(*begin))) {
            return {};
        }
        std::byte high = digitToByte(static_cast<char>(*begin));
        ++begin;
        skipSpaces();
        if (begin == end || !isHexDigit(static_cast<char>(*begin))) {
            return {};
        }
        std::byte low = digitToByte(static_cast<char>(*begin));
        ++begin;
        result.push_back((high << 4) | low);
        skipSpaces();
    }
    return Bytes{result};
}
}


auto Bytes::fromHex(const std::string_view hex) noexcept -> Bytes {
    return convertFromHex(hex.begin(), hex.end());
}


auto Bytes::fromHex(const char *hex) noexcept -> Bytes {
    if (hex == nullptr) {
        return {};
    }
    return convertFromHex(hex, hex + std::strlen(hex));
}


auto Bytes::fromHex(const String &hex) noexcept -> Bytes {
    return convertFromHex(hex.begin(), hex.end());
}


auto Bytes::fromHex(const std::u8string_view hex) noexcept -> Bytes {
    return convertFromHex(hex.begin(), hex.end());
}


auto Bytes::fromHex(const char8_t *hex) noexcept -> Bytes {
    if (hex == nullptr) {
        return {};
    }
    const char *ptr = reinterpret_cast<const char *>(hex);
    return convertFromHex(ptr, ptr + std::strlen(ptr));
}


auto Bytes::toHex() const noexcept -> String {
    String result;
    result.reserve(size() * 2);
    for (const auto byte : *this) {
        result.append(byteToDigit(byte >> 4));
        result.append(byteToDigit(byte & std::byte{0x0f}));
    }
    return result;
}


auto Bytes::toHexForErrors() const noexcept -> String {
    String result;
    result.reserve(35);
    auto maxLength = std::min(static_cast<std::size_t>(16), size());
    for (std::size_t i = 0; i < maxLength; ++i) {
        const auto byte = (*this)[i];
        result.append(byteToDigit(byte >> 4));
        result.append(byteToDigit(byte & std::byte{0x0f}));
    }
    if (maxLength < size()) {
        result.append(u8"...");
    }
    return result;
}


}


auto std::hash<erbsland::conf::Bytes>::operator()(const erbsland::conf::Bytes &bytes) const noexcept -> std::size_t {
    std::size_t result = 0;
    for (const auto byte : bytes.raw()) {
        erbsland::conf::impl::hashCombine(result, byte);
    }
    return result;
}

