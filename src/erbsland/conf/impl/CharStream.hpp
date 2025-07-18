// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Defaults.hpp"
#include "InternalView.hpp"
#include "Limits.hpp"

#include "crypto/ShaHash.hpp"
#include "decoder/DecodedChar.hpp"

#include "../Source.hpp"

#include <cassert>


namespace erbsland::conf::impl {


class CharStream;
using CharStreamPtr = std::shared_ptr<CharStream>;


/// The UTF-8 and line decoder.
///
/// - Reads the lines from the source.
/// - Keeps track of the location.
/// - Decodes and verifies the UTF-8 input.
/// - Handles end of a file.
///
/// @tested `DecoderTest`
///
class CharStream final {
public:
    static auto create(SourcePtr source) noexcept -> CharStreamPtr {
        assert(source != nullptr);
        return std::make_shared<CharStream>(std::move(source));
    }
    explicit CharStream(SourcePtr source) noexcept : _source{std::move(source)} {
        assert(_source != nullptr);
    }
    ~CharStream() = default;

public:
    /// Decode the next character in the stream.
    ///
    [[nodiscard]] auto next() -> DecodedChar;

    /// Capture the text up to the given position.
    ///
    /// @param endPosition The byte-index of the end position to capture.
    ///
    [[nodiscard]] auto captureTo(const std::size_t endPosition) -> String {
        if (endPosition > _lineCharacterStartIndex) {
            throwInternalError("Invalid capture position. End after actual read position.");
        }
        if (endPosition <= _captureStartIndex) {
            throwInternalError("Invalid capture position. End before start index.");
        }
        const auto startPosition = std::exchange(_captureStartIndex, endPosition);
        return String{std::span{_line.begin() + startPosition, _line.begin() + endPosition}, PrivateTag{}};
    }

    /// Capture everything up to the end of the line.
    ///
    [[nodiscard]] auto captureToEndOfLine() noexcept -> String {
        const auto startPosition = std::exchange(_captureStartIndex, _lineLength);
        return String{std::span{_line.begin() + startPosition, _line.begin() + _lineLength}, PrivateTag{}};
    }

    /// Access the source used by this decoder.
    ///
    [[nodiscard]] auto source() const noexcept -> const SourcePtr& { return _source; }

    /// Access the last start index of the current character.
    ///
    /// @note This call is used, in case of an error, to get the precise error location. For the buffered token
    /// decoder, storing the precise location of the error is important to allow capturing all characters before
    /// the error happens.
    ///
    [[nodiscard]] auto lastCharacterStartIndex() const noexcept -> std::size_t { return _lineCharacterStartIndex; }

    /// Get the hash digest for the document.
    ///
    /// Call this function, *after* you received the end-of-document character. The digest is only available
    /// when the decoder detected a `\@signature` value in the first line of the document.
    ///
    /// @return The digest, or empty if no digest was created.
    ///
    [[nodiscard]] auto digest() const noexcept -> Bytes { return _digest; }

    /// Manually enable hash calculation.
    ///
    void enableHash() noexcept { _hashEnabled = true; }

    /// Determine whether the current line starts with a signature marker.
    ///
    /// @return `true` if a "@signature" value is detected.
    ///
    [[nodiscard]] auto isSignatureLine() const noexcept -> bool;

private:
    /// Reads the next line from the source into the internal buffer.
    ///
    /// - Resets the line's current index and its character start index.
    /// - Updates the capture-related metadata to prepare for further
    ///   data decoding.
    /// - Resets the start of text capture fields for the current line.
    ///
    /// @throws Error passes all exceptions from the `Source::readLine()` call.
    ///
    void readNextLine();

    /// Decode the next UTF-8 sequence in the line buffer.
    ///
    /// @return The decoded character.
    ///
    auto decodeNext() -> DecodedChar;

    /// Create the end-of-data character.
    ///
    /// Updates the current position and finalizes the digest when called the first time.
    ///
    /// @return The special end-of-data marker.
    ///
    [[nodiscard]] auto createEndOfData() noexcept -> DecodedChar;

    /// Throw an encoding error at the current document position.
    ///
    /// @param message The diagnostic text.
    /// @throws Error Always thrown.
    ///
    void throwEncodingError(String message) const;

    /// Throw a character-related error at the current document position.
    ///
    /// @param message The diagnostic text.
    /// @throws Error Always thrown.
    ///
    void throwCharacterError(String message) const;

    /// Throw an internal error at the current document position.
    ///
    /// @param message The diagnostic text.
    /// @throws Error Always thrown.
    ///
    void throwInternalError(String message) const;

    /// Check if the decoder reached the end of the current line buffer.
    ///
    /// @return `true` when the line index matches the line length.
    ///
    [[nodiscard]] auto isAtEndOfLine() const noexcept -> bool { return _lineCurrentIndex == _lineLength; }

public: // testing
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const CharStream &object) -> InternalViewPtr;
#endif

private:
    SourcePtr _source; ///< The input source.
    bool _endOfData{false}; ///< True, if the end of the data was reached.
    std::array<std::byte, limits::maxLineLength + 1> _line{}; ///< The line buffer.
    std::size_t _lineLength{0}; ///< The line buffer length.
    std::size_t _lineCurrentIndex{0}; ///< The line buffer index.
    std::size_t _lineCharacterStartIndex{0}; ///< The index, where the last read character started.
    std::size_t _captureStartLine{0}; ///< The capture start line (for integrity checks).
    std::size_t _captureStartIndex{0}; ///< The capture start index.
    Position _position{0, 1}; ///< The current position.
    bool _hashEnabled{false}; ///< Set to `true` if a `\@signature` line is encountered.
    crypto::ShaHash _hash{defaults::documentHashAlgorithm}; ///< The hash function, used for signed documents.
    Bytes _digest; ///< The hash digest.
};


}


