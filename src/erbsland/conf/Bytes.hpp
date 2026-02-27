// Copyright (c) 2024-2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "String.hpp"

#include "impl/Definitions.hpp"
#include "impl/utilities/ComparisonHelper.hpp"
#include "impl/utilities/ContainerWrappingHelper.hpp"

#include <cstddef>
#include <span>
#include <vector>


namespace erbsland::conf {


/// A sequence of bytes.
///
/// @tested `BytesTest`
///
class Bytes {
public:
    /// A vector of bytes.
    ///
    using ByteVector = std::vector<std::byte>;

    ERBSLAND_CONF_CONTAINER_DEFINITIONS(ByteVector);

public: // construction.
    /// Create an empty sequence of bytes.
    ///
    Bytes() = default;

    /// Create a sequence of bytes from a vector of bytes.
    ///
    explicit constexpr Bytes(ByteVector data) noexcept : _data{std::move(data)} {}

    /// Create a sequence of bytes from a span of bytes.
    ///
    explicit Bytes(const std::span<std::byte> data) noexcept : _data{data.begin(), data.end()} {};

public: // operators
    ERBSLAND_CONF_COMPARE_MEMBER(const Bytes &other, _data, other._data);

public: // Conversion
    /// Converts a list of 1-byte integers into a sequence of bytes.
    ///
    /// @param list The list of ints.
    /// @return The sequence of bytes.
    ///
    template<typename T>
    requires (std::is_integral_v<T> && sizeof(T) == 1)
    [[nodiscard]] static auto from(std::initializer_list<T> list) noexcept -> Bytes {
        ByteVector data;
        data.reserve(list.size());
        std::transform(list.begin(), list.end(), std::back_inserter(data),
            [](T v) { return static_cast<std::byte>(v); });
        return Bytes{data};
    }

    /// Converts a list of integers into a sequence of bytes, potentially losing the higher bytes.
    ///
    /// @param list The list of ints.
    /// @return The sequence of bytes.
    ///
    template<typename T>
    requires (std::is_integral_v<T>)
    [[nodiscard]] static auto convertFrom(std::initializer_list<T> list) noexcept -> Bytes {
        ByteVector data;
        data.reserve(list.size());
        std::transform(list.begin(), list.end(), std::back_inserter(data),
            [](T v) { return static_cast<std::byte>(v); });
        return Bytes{data};
    }

    /// Converts a list of 1-byte integers into a sequence of bytes.
    ///
    /// @param begin The begin iterator.
    /// @param end The end iterator.
    /// @return The sequence of bytes.
    ///
    template<std::input_iterator It>
    requires (std::is_integral_v<std::iter_value_t<It>> && sizeof(std::iter_value_t<It>) == 1)
    [[nodiscard]] static auto from(It begin, It end) -> Bytes {
        ByteVector data;
        if (const auto dist = std::distance(begin, end); dist > 0) {
            data.reserve(static_cast<std::size_t>(dist));
        }
        std::transform(begin, end, std::back_inserter(data), [](auto v) { return static_cast<std::byte>(v); });
        return Bytes{data};
    }

    /// Converts a list of integers into a sequence of bytes, potentially losing the higher bytes.
    ///
    /// @param begin The begin iterator.
    /// @param end The end iterator.
    /// @return The sequence of bytes.
    ///
    template<std::input_iterator It>
    requires (std::is_integral_v<std::iter_value_t<It>>)
    [[nodiscard]] static auto convertFrom(It begin, It end) -> Bytes {
        ByteVector data;
        if (const auto dist = std::distance(begin, end); dist > 0) {
            data.reserve(static_cast<std::size_t>(dist));
        }
        std::transform(begin, end, std::back_inserter(data), [](auto v) { return static_cast<std::byte>(v); });
        return Bytes{data};
    }

    /// @name Convert from Hexadecimal String
    ///
    /// Convert a string of hex characters into a sequence of bytes.
    ///
    /// Spaces are ignored and the text is parsed case-insensitive.
    /// In case of an error, an empty sequence is returned.
    ///
    /// @{

    /// @param hex The string with the hex characters.
    /// @return The sequence of bytes or an empty sequence on error.
    [[nodiscard]] static auto fromHex(std::string_view hex) noexcept -> Bytes;
    /// @param hex The string with the hex characters.
    /// @return The sequence of bytes or an empty sequence on error.
    [[nodiscard]] static auto fromHex(const char *hex) noexcept -> Bytes;
    /// @param hex The string with the hex characters.
    /// @return The sequence of bytes or an empty sequence on error.
    [[nodiscard]] static auto fromHex(const String &hex) noexcept -> Bytes;
    /// @param hex The string with the hex characters.
    /// @return The sequence of bytes or an empty sequence on error.
    [[nodiscard]] static auto fromHex(std::u8string_view hex) noexcept -> Bytes;
    /// @param hex The string with the hex characters.
    /// @return The sequence of bytes or an empty sequence on error.
    [[nodiscard]] static auto fromHex(const char8_t *hex) noexcept -> Bytes;
    /// @}

    /// Convert this sequence of bytes to a string of hex characters.
    /// @return The string with the hex characters.
    [[nodiscard]] auto toHex() const noexcept -> String;

    /// Convert this sequence of bytes to a string of hex characters.
    /// If the sequence is longer than 16 bytes, the sequence is shortened and "..." appended to the string.
    /// @return The string with the hex characters, suitable for error messages.
    [[nodiscard]] auto toHexForErrors() const noexcept -> String;

public: // wrapper around the underlying vector
    ERBSLAND_CONF_CONTAINER_ACCESS_METHODS(ByteVector, _data);
    ERBSLAND_CONF_CONTAINER_PUSH_BACK(_data);

public: // convenience methods
    /// Append another byte-sequence to this one.
    ///
    /// @param bytes A byte sequence.
    ///
    void append(const Bytes &bytes) noexcept {
        _data.insert(_data.end(), bytes.begin(), bytes.end());
    }

private:
    ByteVector _data;
};


}


template <>
struct std::hash<erbsland::conf::Bytes> {
    auto operator()(const erbsland::conf::Bytes& bytes) const noexcept -> std::size_t ;
};


template <>
struct std::formatter<erbsland::conf::Bytes> : std::formatter<std::string> {
    auto format(const erbsland::conf::Bytes &bytes, format_context& ctx) const {
        return std::formatter<std::string>::format(bytes.toHex().toCharString(), ctx);
    }
};

