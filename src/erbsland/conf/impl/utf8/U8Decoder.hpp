// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Char.hpp"
#include "../../Bytes.hpp"
#include "../../Error.hpp"
#include "../../String.hpp"


namespace erbsland::conf::impl {


/// A safe and reliable UTF-8 decoder.
///
/// @tested `U8DecoderTest`
///
template<typename T> requires (std::is_same_v<std::remove_const_t<T>, std::byte> || (std::is_integral_v<T> && sizeof(T) == 1))
class U8Decoder final {
public:
    explicit U8Decoder(const std::span<T> buffer)
        : _buffer{buffer} {
    }
    explicit U8Decoder(const Bytes &bytes)
        : _buffer{std::span(bytes.data(), bytes.size())} {
    }
    explicit U8Decoder(const std::u8string &text)
        : _buffer{std::span(text.data(), text.size())} {
    }
    explicit U8Decoder(const std::u8string_view text)
        : _buffer{std::span(text.data(), text.size())} {
    }
    explicit U8Decoder(const String &text)
        : _buffer{std::span(text.raw().data(), text.raw().size())} {
    }

    // prevent assign and copy
    U8Decoder(const U8Decoder &) = delete;
    U8Decoder(U8Decoder &&) = delete;
    auto operator=(const U8Decoder &) -> U8Decoder & = delete;
    auto operator=(U8Decoder &&) -> U8Decoder & = delete;

public: // Accessor
    /// Access the buffer.
    ///
    [[nodiscard]] auto buffer() const noexcept -> const std::span<T>& {
        return _buffer;
    }

public:
    /// Decode all characters using a custom function.
    ///
    /// @throws Error (Encoding) If the buffer contains an encoding error.
    ///
    template<typename Function> requires std::invocable<Function, Char>
    void decodeAll(Function &&decodeFn) const {
        std::size_t position = 0;
        while (position < _buffer.size()) {
            decodeFn(decodeChar(_buffer, position));
        }
    }

    /// Count all characters in the buffer.
    ///
    /// @throws Error (Encoding) If the buffer contains an encoding error.
    ///
    [[nodiscard]] auto countAll() const -> std::size_t {
        if (_buffer.empty()) {
            return 0;
        }
        std::size_t count = 0;
        std::size_t position = 0;
        while (position < _buffer.size()) {
            skipChar(_buffer, position);
            count += 1;
        }
        return count;
    }

    /// Verify the encoding in the buffer.
    ///
    [[nodiscard]] auto verify() const noexcept -> bool {
        try {
            decodeAll([](const Char&) {});
            return true;
        } catch (const Error&) {
            return false;
        }
    }

    /// Get the byte position of a character.
    ///
    /// @return The start byte of the character.
    /// @throws std::range_error If the position is not in the buffer.
    /// @throws Error (Encoding) If the buffer contains an encoding error.
    ///
    [[nodiscard]] auto startByte(std::size_t characterIndex) -> std::size_t {
        if (_buffer.empty()) {
            throw std::range_error("Cannot get position of character in empty buffer.");
        }
        if (characterIndex == 0) {
            return 0;
        }
        std::size_t bytePosition = 0;
        for (std::size_t i = 0; i < characterIndex; ++i) {
            skipChar(_buffer, bytePosition);
            if (bytePosition >= _buffer.size()) {
                throw std::range_error("The position is outside the buffer.");
            }
        }
        return bytePosition;
    }

public:
    /// Throw an encoding error at the current document position.
    ///
    /// @param message The diagnostic text.
    /// @throws Error Always thrown.
    ///
    static void throwEncodingError(String message) {
        throw Error(ErrorCategory::Encoding, std::move(message));
    }

    /// Decode a single UTF-8 character in the buffer and advance the position.
    ///
    /// @param buffer The byte buffer to decode.
    /// @param position The read position that is advanced after a successful read.
    /// @throws Error (ErrorCategory::Encoding) if there is an encoding error in the data. The position is
    ///     only advanced when a character is successfully read.
    ///
    [[nodiscard]] static auto decodeChar(std::span<T> buffer, std::size_t &position) -> Char {
        if (position >= buffer.size()) {
            return Char::EndOfData;
        }
        auto index = position;
        auto c = static_cast<std::byte>(buffer[index]);
        index += 1;
        if (c < std::byte{0x80U}) { // 7-bit ASCII?
            const auto result = Char{static_cast<char32_t>(c)};
            position = index;
            return result;
        }
        std::size_t cSize = 0;
        char32_t unicodeValue{};
        if ((c & std::byte{0b11100000U}) == std::byte{0b11000000U} && c >= std::byte{0b11000010U}) {
            cSize = 2; // 2-byte sequence
            unicodeValue = static_cast<char32_t>(c & std::byte{0b00011111U});
        } else if ((c & std::byte{0b11110000U}) == std::byte{0b11100000U}) {
            cSize = 3; // 3-byte sequence
            unicodeValue = static_cast<char32_t>(c & std::byte{0b00001111U});
        } else if ((c & std::byte{0b11111000U}) == std::byte{0b11110000U} && c < std::byte{0b11110101U}) {
            cSize = 4; // 4-byte sequence
            unicodeValue = static_cast<char32_t>(c & std::byte{0b00000111U});
        } else {
            throwEncodingError(u8"Invalid or out-of-range start byte sequence.");
        }
        for (std::size_t i = 1; i < cSize; ++i) {
            if (index >= buffer.size()) {
                throwEncodingError(u8"Unexpected end of the data.");
            }
            c = static_cast<std::byte>(buffer[index]);
            index += 1;
            if ((c & std::byte{0b11000000U}) != std::byte{0b10000000U}) {
                throwEncodingError(u8"Unexpected continuation byte.");
            }
            unicodeValue <<= 6;
            unicodeValue |= static_cast<char32_t>(c & std::byte{0b00111111U});
        }
        if ((cSize == 3 && unicodeValue < 0x800) || cSize == 4 && unicodeValue < 0x10000) {
            throwEncodingError(u8"Overlong encoding.");
        }
        const auto result = Char{unicodeValue};
        if (!result.isValidUnicode()) {
            throwEncodingError(u8"Invalid Unicode character.");
        }
        position = index;
        return result;
    }

    /// Skip a UTF-8 character in the buffer and advance the position.
    ///
    /// This method is faster than `decodeChar` but does not detect all encoding errors.
    ///
    /// @param buffer The byte buffer to decode.
    /// @param position The read position that is advanced after a successful read.
    /// @throws Error (ErrorCategory::Encoding) if there is an encoding error in the data. The position is
    ///     only advanced when a character is successfully read.
    ///
    static void skipChar(std::span<T> buffer, std::size_t &position) {
        if (position >= buffer.size()) {
            return;
        }
        auto index = position;
        auto c = static_cast<std::byte>(buffer[index]);
        index += 1;
        if (c < std::byte{0x80U}) { // 7-bit ASCII?
            position = index;
            return;
        }
        std::size_t cSize = 0;
        if ((c & std::byte{0b11100000U}) == std::byte{0b11000000U} && c >= std::byte{0b11000010U}) {
            cSize = 2; // 2-byte sequence
        } else if ((c & std::byte{0b11110000U}) == std::byte{0b11100000U}) {
            cSize = 3; // 3-byte sequence
        } else if ((c & std::byte{0b11111000U}) == std::byte{0b11110000U} && c < std::byte{0b11110101U}) {
            cSize = 4; // 4-byte sequence
        }
        if (cSize < 2) {
            throwEncodingError(u8"Invalid or out-of-range start byte sequence.");
        }
        for (std::size_t i = 1; i < cSize; ++i) {
            if (index >= buffer.size()) {
                throwEncodingError(u8"Unexpected end of the data.");
            }
            c = static_cast<std::byte>(buffer[index]);
            index += 1;
            if ((c & std::byte{0b11000000U}) != std::byte{0b10000000U}) {
                throwEncodingError(u8"Unexpected continuation byte.");
            }
        }
        position = index;
    }

private:
    std::span<T> _buffer;
};


U8Decoder(const Bytes&) -> U8Decoder<const std::byte>;
U8Decoder(const String&) -> U8Decoder<const char8_t>;
U8Decoder(const std::u8string&) -> U8Decoder<const char8_t>;
U8Decoder(std::u8string_view) -> U8Decoder<const char8_t>;


}

