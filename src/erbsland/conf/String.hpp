// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "EscapeMode.hpp"

#include "impl/Char.hpp"
#include "impl/ComparisonHelper.hpp"
#include "impl/ContainerWrappingHelper.hpp"
#include "impl/PrivateTag.hpp"

#include <format>
#include <functional>
#include <ranges>
#include <span>
#include <string>


namespace erbsland::conf {


class Bytes;


/// Thin wrapper around `std::u8string`.
///
/// The class mirrors the API of `std::u8string` closely and adds a few convenience functions.
/// It is primarily intended to ease the integration of the parser into applications that use
/// `std::string` for text processing.
///
/// @note Because of unsolved problems in C++20 regarding the handling of `char8_t`, this thin wrapper around
/// `std::u8string` aims to simplify the integration of this library into projects and also shall ease the
/// transformation to `std::u8string` in future versions of C++.
///
/// @tested `StringTest`, `StringUtf8Test`
///
class String final {
    using WrappedString = std::u8string;

public:
    using ConstByteSpan = std::span<const std::byte>;

public: // wrap type definitions
    ERBSLAND_CONF_CONTAINER_DEFINITIONS(WrappedString);
    static constexpr auto npos = WrappedString::npos;

public: // construction
    // All `char8_t` constructors
    template<std::size_t N>
    constexpr String(const char8_t (&literal)[N]) noexcept : _string{literal, N - 1} {}
    explicit constexpr String(const char8_t *str, std::size_t size) noexcept {
        if (str != nullptr) {
            _string.assign(str, size);
        }
    }
    explicit constexpr String(const std::u8string_view str) noexcept : _string{str} {}
    String(const char *str) noexcept {
        if (str != nullptr) {
            _string = reinterpret_cast<const char8_t*>(str);
        }
    }
    String(const char *str, std::size_t size) noexcept {
        if (str != nullptr) {
            _string.assign(reinterpret_cast<const char8_t*>(str), size);
        }
    }
    explicit constexpr String(const WrappedString &str) noexcept : _string{str} {}
    constexpr String(WrappedString &&str) noexcept : _string{std::move(str)} {}
    constexpr String(std::size_t count, value_type c) noexcept : _string(count, c) {}
    template<typename InputIt>
    constexpr String(InputIt begin, InputIt end) noexcept : _string{begin, end} {}

    // All `char` constructors
    template<std::size_t N>
    constexpr String(const char (&literal)[N]) noexcept : _string{reinterpret_cast<const char8_t*>(literal), N - 1} {}
    explicit String(const std::string &str) noexcept : _string{reinterpret_cast<const char8_t*>(str.data()), str.size()} {}
    constexpr String(std::size_t count, char c) noexcept : _string(count, c) {}

    /// @private
    explicit String(const ConstByteSpan byteSpan, impl::PrivateTag) noexcept : _string{
        reinterpret_cast<const char8_t*>(byteSpan.data()), byteSpan.size()} {}

    // defaults
    String() = default;
    ~String() = default;
    String(const String&) noexcept = default;
    String(String&&) noexcept = default;
    auto operator=(const String&) -> String& = default;
    auto operator=(String&&) -> String& = default;

public: // wrap some common operators
    ERBSLAND_CONF_COMPARE_MEMBER(const String &other, _string, other._string);
    auto operator+(const String &other) const noexcept -> String {
        return {_string + other._string};
    }
    auto operator+=(const String &other) noexcept -> String& {
        _string.append(other._string);
        return *this;
    }
    auto operator+=(value_type c) noexcept -> String& {
        _string.append(1, c);
        return *this;
    }

    // char8_t
    template<std::size_t N>
    auto operator==(const char8_t (&literal)[N]) const noexcept -> bool {
        return _string == std::u8string_view(literal, N - 1);
    }
    template<std::size_t N>
    auto operator!=(const char8_t (&literal)[N]) const noexcept -> bool {
        return _string != std::u8string_view(literal, N - 1);
    }
    template<std::size_t N>
    auto operator+(const char8_t (&literal)[N]) const noexcept -> String {
        auto copy = *this;
        copy.append(std::u8string_view(literal, N - 1));
        return copy;
    }
    auto operator+(const std::u8string &other) const noexcept -> String {
        return {_string + other};
    }
    template<std::size_t N>
    auto operator+=(const char8_t (&literal)[N]) noexcept -> String& {
        _string.append(std::u8string_view(literal, N - 1));
        return *this;
    }
    auto operator+=(const std::u8string &other) noexcept -> String& {
        _string.append(other);
        return *this;
    }

    // char
    template<std::size_t N>
    auto operator==(const char (&literal)[N]) const noexcept -> bool {
        return _string == std::u8string_view(reinterpret_cast<const char8_t*>(literal), N - 1);
    }
    template<std::size_t N>
    auto operator!=(const char (&literal)[N]) const noexcept -> bool {
        return _string != std::u8string_view(reinterpret_cast<const char8_t*>(literal), N - 1);
    }
    template<std::size_t N>
    auto operator+(const char (&literal)[N]) const noexcept -> String {
        return {_string + std::u8string(reinterpret_cast<const char8_t*>(literal), N - 1)};
    }
    auto operator+(const std::string &other) const noexcept -> String {
        return {_string + std::u8string(reinterpret_cast<const char8_t*>(other.data()), other.size())};
    }
    template<std::size_t N>
    auto operator+=(const char (&literal)[N]) noexcept -> String& {
        _string.append(std::u8string(reinterpret_cast<const char8_t*>(literal), N - 1));
        return *this;
    }
    auto operator+=(const std::string &other) noexcept -> String& {
        _string.append(std::u8string(reinterpret_cast<const char8_t*>(other.data()), other.size()));
        return *this;
    }

    // Char
    auto operator+=(const impl::Char unicodeChar) noexcept -> String& {
        unicodeChar.appendTo(_string);
        return *this;
    }

public: // wrap common functions
    ERBSLAND_CONF_CONTAINER_ACCESS_METHODS(WrappedString, _string);
    ERBSLAND_CONF_CONTAINER_PUSH_BACK(_string);
    [[nodiscard]] constexpr auto length() const noexcept -> size_type { return _string.length(); } \
    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type { return _string.max_size(); }
    void clear() noexcept { _string.clear(); }
    void reserve(size_type size) noexcept { _string.reserve(size); }
    void shrink_to_fit() noexcept { _string.shrink_to_fit(); }
    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return _string.capacity(); }
    void append(const String &other) noexcept { _string.append(other._string); }
    void append(const value_type character) noexcept { _string.append(1, character); }
    template<std::size_t N>
    void append(const char8_t (&literal)[N]) noexcept { _string.append(literal, N - 1); }
    void append(const std::u8string &str) noexcept { _string.append(str); }
    void append(const std::u8string_view str) noexcept { _string.append(str.begin(), str.end()); }
    void append(const std::string &str) noexcept { append(String{str}); }
    void append(const std::string_view str) noexcept { append(String{str.begin(), str.end()}); }
    void append(const impl::Char unicodeChar) noexcept { unicodeChar.appendTo(_string); }
    [[nodiscard]] auto substr(size_type pos = 0, size_type count = npos) const -> String { return {_string.substr(pos, count)}; }
    auto erase(size_type index = 0, size_type count = npos) noexcept -> String& { _string.erase(index, count); return *this; }
    auto erase(iterator position) noexcept -> iterator { return _string.erase(position); }
    auto erase(const_iterator position) noexcept -> iterator { return _string.erase(position); }
    auto erase(iterator first, iterator last) noexcept -> iterator { return _string.erase(first, last); }
    auto erase(const_iterator first, const_iterator last) noexcept -> iterator { return _string.erase(first, last); }
    template<typename FindStr>
    auto find(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find(s, pos, count); }
    template<typename FindStr>
    auto find(FindStr s, size_type pos = 0) const -> size_type { return _string.find(s, pos); }
    template<typename FindStr>
    auto rfind(FindStr s, size_type pos, size_type count) const -> size_type { return _string.rfind(s, pos, count); }
    template<typename FindStr>
    auto rfind(FindStr s, size_type pos = npos) const -> size_type { return _string.rfind(s, pos); }
    template<typename FindStr>
    auto find_first_of(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find_first_of(s, pos, count); }
    template<typename FindStr>
    auto find_first_of(FindStr s, size_type pos = 0) const -> size_type { return _string.find_first_of(s, pos); }
    template<typename FindStr>
    auto find_first_not_of(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find_first_not_of(s, pos, count); }
    template<typename FindStr>
    auto find_first_not_of(FindStr s, size_type pos = 0) const -> size_type { return _string.find_first_not_of(s, pos); }
    template<typename FindStr>
    auto find_last_of(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find_last_of(s, pos, count); }
    template<typename FindStr>
    auto find_last_of(FindStr s, size_type pos = npos) const -> size_type { return _string.find_last_of(s, pos); }
    template<typename FindStr>
    auto find_last_not_of(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find_last_not_of(s, pos, count); }
    template<typename FindStr>
    auto find_last_not_of(FindStr s, size_type pos = npos) const -> size_type { return _string.find_last_not_of(s, pos); }
    template<typename FindStr>
    auto starts_with(FindStr s) const noexcept -> bool { return _string.starts_with(s); }
    template<typename FindStr>
    auto ends_with(FindStr s) const noexcept -> bool { return _string.ends_with(s); }
    template<typename FindStr>
    auto contains(FindStr s) const noexcept -> bool { return _string.find(s) != npos; }

public: // Extensions
    /// Convert the wrapped string into a `char` based `std::string`.
    ///
    /// This helper performs the required conversion from `char8_t` to `char`
    /// and is primarily meant for interoperability with APIs that expect a
    /// regular `std::string`.
    ///
    [[nodiscard]] auto toCharString() const noexcept -> std::string {
        std::string result;
        result.reserve(_string.size());
#if __cpp_lib_containers_ranges >= 201703L
        std::ranges::transform(_string, std::back_inserter(result), [](auto c) { return static_cast<char>(c); });
#else
        std::ranges::copy(_string.begin(), _string.end(), std::back_inserter(result));
#endif
        return result;
    }

    /// Get the byte size of the escaped string.
    ///
    /// Use this function to calculate the size requirements of an escaped string, without the actual conversion.
    ///
    /// @return The byte size of the escaped text (without trailing zero end byte).
    ///
    [[nodiscard]] auto escapedSize(EscapeMode mode) const noexcept -> std::size_t;

    /// Create an escaped version of this string.
    ///
    /// @param mode The escape mode to use for escaping.
    ///
    [[nodiscard]] auto toEscaped(EscapeMode mode) const noexcept -> String;

private:
    WrappedString _string;
};


}


template<>
struct std::hash<erbsland::conf::String> {
    auto operator()(const erbsland::conf::String &s) const noexcept -> size_t {
        const std::u8string &str = s.raw();
        const auto data = reinterpret_cast<const char*>(str.data());
        return std::hash<std::string_view>{}(std::string_view(data, str.size()));
    }
};

template <>
struct std::formatter<erbsland::conf::String> : std::formatter<std::string> {
    auto format(erbsland::conf::String str, format_context& ctx) const {
        return std::formatter<std::string>::format(str.toCharString(), ctx);
    }
};

