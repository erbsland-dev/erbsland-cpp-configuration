// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "EscapeMode.hpp"
#include "TestFormat.hpp"

#include "impl/Char.hpp"
#include "impl/ComparisonHelper.hpp"
#include "impl/ContainerWrappingHelper.hpp"
#include "impl/PrivateTag.hpp"

#include <algorithm>
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
/// @tested `StringTest`, `StringUtf8Test`
///
class String final {
    using WrappedString = std::u8string;

public:
    /// A span of bytes referencing the underlying data of the string.
    ///
    using ConstByteSpan = std::span<const std::byte>;

public: // wrap type definitions
    ERBSLAND_CONF_CONTAINER_DEFINITIONS(WrappedString);

    /// Constant representing an invalid or not-found position.
    ///
    static constexpr auto npos = WrappedString::npos;

public: // construction
    // All `char8_t` constructors
    /// Construct from a UTF-8 string literal.
    ///
    /// @tparam N The length of the literal including the null terminator.
    /// @param literal The UTF-8 literal to copy.
    ///
    template<std::size_t N>
    constexpr String(const char8_t (&literal)[N]) noexcept : _string{literal, N - 1} {}

    /// Construct from a UTF-8 character pointer and size.
    ///
    /// @param str Pointer to UTF-8 characters.
    /// @param size Number of characters to read.
    ///
    constexpr String(const char8_t *str, const std::size_t size) noexcept
        : _string{str != nullptr ? std::u8string_view(str, size) : std::u8string_view{}} {
    }

    /// Construct from a UTF-8 string view.
    ///
    /// @param str The UTF-8 string view to copy.
    ///
    constexpr String(const std::u8string_view str) noexcept : _string{str} {}

    /// Copy construct from the underlying UTF-8 string.
    ///
    /// @param str The string to wrap.
    ///
    explicit constexpr String(const WrappedString &str) noexcept : _string{str} {}

    /// Move construct from the underlying UTF-8 string.
    ///
    /// @param str The string to move from.
    ///
    constexpr String(WrappedString &&str) noexcept : _string{std::move(str)} {}

    /// Construct a string with a repeated character.
    ///
    /// @param count Number of characters.
    /// @param c The character to repeat.
    ///
    constexpr String(std::size_t count, value_type c) noexcept : _string(count, c) {}

    /// Construct a string from a character range.
    ///
    /// @tparam InputIt Input iterator type.
    /// @param begin Iterator to the first character.
    /// @param end Iterator to one-past-last character.
    ///
    template<typename InputIt>
    constexpr String(InputIt begin, InputIt end) noexcept : _string{begin, end} {}

    // All `char` constructors
    /// Construct from a null-terminated string.
    ///
    /// @tparam N The length of the literal including the null terminator.
    /// @param literal The literal to copy.
    ///
    template<std::size_t N>
    constexpr String(const char (&literal)[N]) noexcept : _string{reinterpret_cast<const char8_t*>(literal), N - 1} {}

    /// Construct from a character pointer and size.
    ///
    /// @param str Pointer to narrow characters.
    /// @param size Number of characters to read.
    ///
    String(const char *str, const std::size_t size) noexcept
        : _string{str != nullptr ? std::u8string_view(reinterpret_cast<const char8_t*>(str), size) : std::u8string_view{}} {
    }

    /// Construct from a standard string view.
    ///
    /// @param str The standard string view to the string to copy.
    ///
    String(const std::string_view str) noexcept : _string{reinterpret_cast<const char8_t*>(str.data()), str.size()} {}

    /// Construct from a standard narrow string.
    ///
    /// @param str The std::string to convert.
    ///
    explicit String(const std::string &str) noexcept : _string{reinterpret_cast<const char8_t*>(str.data()), str.size()} {}

    /// Construct a string with a repeated narrow character.
    ///
    /// @param count Number of characters.
    /// @param c The character to repeat.
    ///
    constexpr String(std::size_t count, char c) noexcept : _string(count, c) {}

    /// @private
    explicit String(const ConstByteSpan byteSpan, impl::PrivateTag) noexcept : _string{
        reinterpret_cast<const char8_t*>(byteSpan.data()), byteSpan.size()} {}

    /// Default constructor.
    String() = default;
    /// Default destructor.
    ~String() = default;
    /// Default copy constructor.
    String(const String&) noexcept = default;
    /// Default move constructor.
    String(String&&) noexcept = default;
    /// Default copy assignment.
    auto operator=(const String&) -> String& = default;
    /// Default move assignment.
    auto operator=(String&&) -> String& = default;

public: // wrap some common operators
    ERBSLAND_CONF_COMPARE_MEMBER(const String &other, _string, other._string);

    /// Concatenate two strings.
    ///
    /// Concatenate two String objects.
    ///
    /// @param other The String to append to a copy of this one.
    /// @return A new String with the combined contents.
    ///
    auto operator+(const String &other) const noexcept -> String {
        return {_string + other._string};
    }

    /// Append another string to this string.
    ///
    /// @param other The string to append.
    /// @return Reference to this string.
    ///
    auto operator+=(const String &other) noexcept -> String& {
        _string.append(other._string);
        return *this;
    }

    /// Append a character to this String.
    ///
    /// @param c The character to append.
    /// @return Reference to this String.
    ///
    auto operator+=(value_type c) noexcept -> String& {
        _string.append(1, c);
        return *this;
    }

    // char8_t
    /// Compare this String to a UTF-8 literal for equality.
    ///
    /// @tparam N The size of the literal including null terminator.
    /// @param literal The UTF-8 literal to compare against.
    /// @return `true` if the literal matches exactly.
    ///
    template<std::size_t N>
    auto operator==(const char8_t (&literal)[N]) const noexcept -> bool {
        return _string == std::u8string_view(literal, N - 1);
    }

    /// Compare this String to a UTF-8 literal for inequality.
    ///
    /// @tparam N The size of the literal including null terminator.
    /// @param literal The UTF-8 literal to compare against.
    /// @return `true` if the literal does not match.
    ///
    template<std::size_t N>
    auto operator!=(const char8_t (&literal)[N]) const noexcept -> bool {
        return _string != std::u8string_view(literal, N - 1);
    }

    /// Concatenate a UTF-8 literal to this String.
    ///
    /// @tparam N The size of the literal including null terminator.
    /// @param literal The UTF-8 literal to append.
    /// @return A new String with the literal appended.
    ///
    template<std::size_t N>
    auto operator+(const char8_t (&literal)[N]) const noexcept -> String {
        auto copy = *this;
        copy.append(std::u8string_view(literal, N - 1));
        return copy;
    }

    /// Concatenate a std::u8string to this String.
    ///
    /// @param other The u8string to append.
    /// @return A new String with the contents appended.
    ///
    auto operator+(const std::u8string &other) const noexcept -> String {
        return {_string + other};
    }

    /// Append a UTF-8 literal to this String.
    ///
    /// @tparam N The size of the literal including null terminator.
    /// @param literal The UTF-8 literal to append.
    /// @return Reference to this String.
    ///
    template<std::size_t N>
    auto operator+=(const char8_t (&literal)[N]) noexcept -> String& {
        _string.append(std::u8string_view(literal, N - 1));
        return *this;
    }

    /// Append a std::u8string to this String.
    ///
    /// @param other The u8string to append.
    /// @return Reference to this String.
    ///
    auto operator+=(const std::u8string &other) noexcept -> String& {
        _string.append(other);
        return *this;
    }

    // char
    /// Compare this String to a narrow literal for equality.
    ///
    /// @tparam N The size of the literal including null terminator.
    /// @param literal The narrow literal to compare.
    /// @return `true` if the literal matches exactly.
    ///
    template<std::size_t N>
    auto operator==(const char (&literal)[N]) const noexcept -> bool {
        return _string == std::u8string_view(reinterpret_cast<const char8_t*>(literal), N - 1);
    }

    /// Compare this String to a narrow literal for inequality.
    ///
    /// @tparam N The size of the literal including null terminator.
    /// @param literal The narrow literal to compare.
    /// @return `true` if the literal does not match.
    ///
    template<std::size_t N>
    auto operator!=(const char (&literal)[N]) const noexcept -> bool {
        return _string != std::u8string_view(reinterpret_cast<const char8_t*>(literal), N - 1);
    }

    /// Concatenate a narrow literal to this String.
    ///
    /// @tparam N The size of the literal including null terminator.
    /// @param literal The narrow literal to append.
    /// @return A new String with the literal appended.
    ///
    template<std::size_t N>
    auto operator+(const char (&literal)[N]) const noexcept -> String {
        return {_string + std::u8string(reinterpret_cast<const char8_t*>(literal), N - 1)};
    }

    /// Concatenate a std::string to this String.
    ///
    /// @param other The std::string to append.
    /// @return A new String with the contents appended.
    ///
    auto operator+(const std::string &other) const noexcept -> String {
        return {_string + std::u8string(reinterpret_cast<const char8_t*>(other.data()), other.size())};
    }

    /// Append a narrow literal to this String.
    ///
    /// @tparam N The size of the literal including null terminator.
    /// @param literal The narrow literal to append.
    /// @return Reference to this String.
    ///
    template<std::size_t N>
    auto operator+=(const char (&literal)[N]) noexcept -> String& {
        _string.append(std::u8string(reinterpret_cast<const char8_t*>(literal), N - 1));
        return *this;
    }

    /// Append a std::string to this String.
    ///
    /// @param other The std::string to append.
    /// @return Reference to this String.
    ///
    auto operator+=(const std::string &other) noexcept -> String& {
        _string.append(std::u8string(reinterpret_cast<const char8_t*>(other.data()), other.size()));
        return *this;
    }

    // Char
    /// Append a single Unicode character to this String.
    ///
    /// @param unicodeChar The character to append.
    /// @return Reference to this String.
    ///
    auto operator+=(const impl::Char unicodeChar) noexcept -> String& {
        unicodeChar.appendTo(_string);
        return *this;
    }

public: // wrap common functions
    ERBSLAND_CONF_CONTAINER_ACCESS_METHODS(WrappedString, _string);
    ERBSLAND_CONF_CONTAINER_PUSH_BACK(_string);

    /// Get the number of characters in this String.
    ///
    /// @return The length of the string.
    ///
    [[nodiscard]] constexpr auto length() const noexcept -> size_type { return _string.length(); }

    /// Get the maximum number of characters this String can hold.
    ///
    /// @return The maximum possible size.
    ///
    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type { return _string.max_size(); }

    /// Erase all characters from this String.
    ///
    void clear() noexcept { _string.clear(); }

    /// Reserve storage to at least the specified capacity.
    ///
    /// @param size The minimum capacity to reserve.
    ///
    void reserve(size_type size) noexcept { _string.reserve(size); }

    /// Reduce memory usage to fit the current size.
    ///
    void shrink_to_fit() noexcept { _string.shrink_to_fit(); }

    /// Get the current capacity of the String.
    ///
    /// @return The allocated storage size.
    ///
    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return _string.capacity(); }

    /// Append another String to this one.
    ///
    /// @param other The String to append.
    ///
    void append(const String &other) noexcept { _string.append(other._string); }

    /// Append a character to this String.
    ///
    /// @param character The character to append.
    ///
    void append(const value_type character) noexcept { _string.append(1, character); }

    /// Append a UTF-8 literal to this String.
    ///
    /// @tparam N The literal length including null terminator.
    /// @param literal The UTF-8 literal to append.
    ///
    template<std::size_t N>
    void append(const char8_t (&literal)[N]) noexcept { _string.append(literal, N - 1); }

    /// Append a std::u8string to this String.
    ///
    /// @param str The u8string to append.
    ///
    void append(const std::u8string &str) noexcept { _string.append(str); }

    /// Append a UTF-8 string view to this String.
    ///
    /// @param str The u8string_view to append.
    ///
    void append(const std::u8string_view str) noexcept { _string.append(str.begin(), str.end()); }

    /// Append a std::string to this String.
    ///
    /// @param str The std::string to append.
    ///
    void append(const std::string &str) noexcept { append(String{str}); }

    /// Append a std::string_view to this String.
    ///
    /// @param str The string_view to append.
    ///
    void append(const std::string_view str) noexcept { append(String{str.begin(), str.end()}); }

    /// Append a Unicode character to this String.
    ///
    /// @param unicodeChar The unicode character to append.
    ///
    void append(const impl::Char unicodeChar) noexcept { unicodeChar.appendTo(_string); }

    /// Extract a substring from this String.
    ///
    /// @param pos The starting index.
    /// @param count The number of characters.
    /// @return The extracted substring.
    ///
    [[nodiscard]] auto substr(size_type pos = 0, size_type count = npos) const -> String { return {_string.substr(pos, count)}; }

    /// Erase a substring from the string.
    ///
    /// @param index The starting index to begin erasure.
    /// @param count The number of characters to erase.
    /// @return Reference to this string after erasure.
    ///
    auto erase(size_type index = 0, size_type count = npos) noexcept -> String& { _string.erase(index, count); return *this; }

    /// Erase the character at the specified position.
    ///
    /// @param position Iterator to the character to remove.
    /// @return Iterator following the removed character.
    ///
    auto erase(iterator position) noexcept -> iterator { return _string.erase(position); }

    /// Erase the character at the specified position.
    ///
    /// @param position Iterator to the character to remove.
    /// @return Iterator following the removed character.
    ///
    auto erase(const_iterator position) noexcept -> iterator { return _string.erase(position); }

    /// Erase a range of characters from the string.
    ///
    /// @param first Iterator to the first character to remove.
    /// @param last Iterator past the last character to remove.
    /// @return Iterator following the last removed character.
    ///
    auto erase(iterator first, iterator last) noexcept -> iterator { return _string.erase(first, last); }

    /// Erase a range of characters from the string.
    ///
    /// @param first Iterator to the first character to remove.
    /// @param last Iterator past the last character to remove.
    /// @return Iterator following the last removed character.
    ///
    auto erase(const_iterator first, const_iterator last) noexcept -> iterator { return _string.erase(first, last); }

    /// Find the first occurrence of a substring in the string.
    ///
    /// @tparam FindStr Type of the search string.
    /// @param s The substring to search for.
    /// @param pos The starting position of the search.
    /// @param count The number of characters of the substring.
    /// @return The index of the first occurrence, or npos if not found.
    ///
    template<typename FindStr>
    auto find(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find(s, pos, count); }
    /// @overload
    template<typename FindStr>
    auto find(FindStr s, size_type pos = 0) const -> size_type { return _string.find(s, pos); }

    /// Find the last occurrence of a substring in the string.
    ///
    /// @tparam FindStr Type of the search string.
    /// @param s The substring to search for.
    /// @param pos The starting position of the search.
    /// @param count The number of characters of the substring.
    /// @return The index of the last occurrence, or npos if not found.
    ///
    template<typename FindStr>
    auto rfind(FindStr s, size_type pos, size_type count) const -> size_type { return _string.rfind(s, pos, count); }
    /// @overload
    template<typename FindStr>
    auto rfind(FindStr s, size_type pos = npos) const -> size_type { return _string.rfind(s, pos); }

    /// Find the first occurrence of any character from a set.
    ///
    /// @tparam FindStr Type of the search set string.
    /// @param s The set of characters to search for.
    /// @param pos The starting position of the search.
    /// @param count The number of characters in the set.
    /// @return The index of the first matching character, or npos if not found.
    ///
    template<typename FindStr>
    auto find_first_of(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find_first_of(s, pos, count); }
    /// @overload
    template<typename FindStr>
    auto find_first_of(FindStr s, size_type pos = 0) const -> size_type { return _string.find_first_of(s, pos); }

    /// Find the first character not in a set.
    ///
    /// @tparam FindStr Type of the search set string.
    /// @param s The set of characters to exclude.
    /// @param pos The starting position of the search.
    /// @param count The number of characters in the set.
    /// @return The index of the first non-matching character, or npos if none.
    ///
    template<typename FindStr>
    auto find_first_not_of(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find_first_not_of(s, pos, count); }
    /// @overload
    template<typename FindStr>
    auto find_first_not_of(FindStr s, size_type pos = 0) const -> size_type { return _string.find_first_not_of(s, pos); }

    /// Find the last occurrence of any character from a set.
    ///
    /// @tparam FindStr Type of the search set string.
    /// @param s The set of characters to search for.
    /// @param pos The starting position of the search.
    /// @param count The number of characters in the set.
    /// @return The index of the last matching character, or npos if not found.
    template<typename FindStr>
    auto find_last_of(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find_last_of(s, pos, count); }
    /// @overload
    template<typename FindStr>
    auto find_last_of(FindStr s, size_type pos = npos) const -> size_type { return _string.find_last_of(s, pos); }

    /// Find the last character not in a set.
    ///
    /// @tparam FindStr Type of the search set string.
    /// @param s The set of characters to exclude.
    /// @param pos The starting position of the search.
    /// @param count The number of characters in the set.
    /// @return The index of the last non-matching character, or npos if none.
    ///
    template<typename FindStr>
    auto find_last_not_of(FindStr s, size_type pos, size_type count) const -> size_type { return _string.find_last_not_of(s, pos, count); }
    /// @overload
    template<typename FindStr>
    auto find_last_not_of(FindStr s, size_type pos = npos) const -> size_type { return _string.find_last_not_of(s, pos); }

    /// Check if the string starts with a given prefix.
    ///
    /// @tparam FindStr Type of the prefix string.
    /// @param s The prefix to check.
    /// @return `true` if the string starts with the prefix, `false` otherwise.
    template<typename FindStr>
    auto starts_with(FindStr s) const noexcept -> bool { return _string.starts_with(s); }

    /// Check if the string ends with a given suffix.
    ///
    /// @tparam FindStr Type of the suffix string.
    /// @param s The suffix to check.
    /// @return `true` if the string ends with the suffix, `false` otherwise.
    ///
    template<typename FindStr>
    auto ends_with(FindStr s) const noexcept -> bool { return _string.ends_with(s); }

    /// Check if the string contains a given substring.
    ///
    /// @tparam FindStr Type of the search string.
    /// @param s The substring to search for.
    /// @return `true` if the substring is found, `false` otherwise.
    ///
    template<typename FindStr>
    auto contains(FindStr s) const noexcept -> bool { return _string.find(s) != npos; }

public: // Extensions
    /// @name String Conversion
    /// @{

    /// Convert and verify UTF-8 data from a `char` based `std:string`.
    ///
    /// @param str The string to convert and verify.
    /// @throws Error (Encoding) if the string contains UTF-8 encoding errors or invalid Unicode code-points.
    ///
    [[nodiscard]] static auto fromCharString(const std::string &str) -> String;

    /// Convert and verify UTF-8 data from a `char` based `std:string`.
    ///
    /// @param strView The string to convert and verify.
    /// @throws Error (Encoding) if the string contains UTF-8 encoding errors or invalid Unicode code-points.
    ///
    [[nodiscard]] static auto fromCharString(std::string_view strView) -> String;

    /// Convert the wrapped string into a `char` based `std::string`.
    ///
    /// This helper performs the required conversion from `char8_t` to `char`
    /// and is primarily meant for interoperability with APIs that expect a
    /// regular `std::string`.
    ///
    /// @return A `char` based string.
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
    /// @}

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
