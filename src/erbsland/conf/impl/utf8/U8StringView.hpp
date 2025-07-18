// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Char.hpp"
#include "../../String.hpp"


namespace erbsland::conf::impl {


/// The elide-location
///
enum class ElideLocation : uint8_t {
    Begin,
    Center,
    End
};


/// A view for `String` supporting UTF-8 methods.
///
class U8StringView final {
public:
    using Comparator = std::function<std::strong_ordering(Char, Char)>;
    using CharTransformer = std::function<Char(Char)>;
    using CharFunction = std::function<void(Char)>;

public:
    explicit U8StringView(const String &str) noexcept : _string{str.raw()} {}
    explicit U8StringView(const std::u8string &str) noexcept : _string{str} {}
    explicit U8StringView(const std::u8string_view str) noexcept : _string{str} {}
    ~U8StringView() = default;

    // disable copy and assign.
    U8StringView(const U8StringView &) = delete;
    auto operator=(const U8StringView &) -> U8StringView& = delete;

public:
    /// Test if this string contains valid UTF-8 data.
    ///
    /// Tests for out-of-range characters and encoding errors.
    ///
    [[nodiscard]] auto isValid() const noexcept -> bool;

    /// Encode this string into UTF-8 data.
    ///
    /// @warning Assumes that this string contains valid UTF-8 data.
    /// If the source of the string isn't trusted, call `isValidUtf8()` first.
    ///
    /// @returns A byte sequence with the encoded UTF-8 data.
    ///
    [[nodiscard]] auto toBytes() const noexcept -> Bytes;

    /// Securely decode a string from UTF-8 data.
    ///
    /// @param data The bytes with the UTF-8 encoded data.
    /// @throws Error (Encoding) for any encoding errors in the given data.
    ///
    [[nodiscard]] static auto fromBytes(const Bytes &data) -> String;

    /// Get the number of UTF-8 characters in this string.
    ///
    /// @throws Error (Encoding) for any encoding errors in the given data.
    ///
    [[nodiscard]] auto length() const -> std::size_t;

    /// Get the byte start position of a character in this string.
    ///
    /// @return The byte position.
    /// @throws Error (Encoding) for any encoding errors in the given data.
    /// @throws std::range_error If the position is outside this string.
    ///
    [[nodiscard]] auto startForChar(std::size_t charIndex) const -> std::size_t;

    /// Truncate a string to the given maximum number of *characters*.
    ///
    /// @param maximumCharacters The maximum number of characters. This must be at a minimum two
    ///     characters larger than the elide-sequence.
    /// @param elideLocation Where the elide-sequence is inserted when truncated.
    /// @param elideSequence The text to insert when the string is truncated, default is `...`.
    /// @return A string with the given maximum number of *characters*.
    /// @throws Error (Encoding) for any encoding errors in the given data.
    ///
    [[nodiscard]] auto truncatedWithElide(
        std::size_t maximumCharacters,
        ElideLocation elideLocation = ElideLocation::End,
        const String &elideSequence = String{u8"…"}) const -> String;

    /// Compare two strings using Unicode code points.
    ///
    /// @param other The other string for the comparison.
    /// @param comparator The comparator to use.
    /// @return The comparison result.
    /// @throws Error (Encoding) for any encoding errors in the given data.
    ///
    [[nodiscard]] auto compare(
        const String &other,
        const Comparator &comparator = Char::compare) const -> std::strong_ordering;

    /// Transform a string using Unicode code points.
    ///
    /// @param transformer A function that transforms one character into another.
    ///
    [[nodiscard]] auto transformed(const CharTransformer &transformer) const -> String;

    /// Call a function for each decoded character.
    ///
    /// @param fn The function to call for each decoded character.
    ///
    void forEachChar(const CharFunction &fn) const;

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

    /// Convert the path into text that is safe for output or logs.
    ///
    /// @param maximumSize The maximum number of *characters* for the text.
    /// @param elideLocation Where the elide should be placed if the text needs to be shortened.
    ///
    [[nodiscard]] auto toSafeText(std::size_t maximumSize = 200, ElideLocation elideLocation = ElideLocation::Center) const -> String;

public:
    std::u8string_view _string; ///< A reference to the string.
};


}

