// Copyright (c) 2024-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "CharStream.hpp"


#include "../utf8/U8Decoder.hpp"

#include <utility>


namespace erbsland::conf::impl {


auto CharStream::next() -> DecodedChar {
    if (_source == nullptr) {
        throw Error{ErrorCategory::Internal, u8"The source is null."};
    }
    if (_endOfData) { // once we reached the end, prevent polling the source further.
        return createEndOfData();
    }
    if (isAtEndOfLine()) { // If we reached the end, try to get more data.
        if (_source->atEnd()) {
            _lineCharacterStartIndex = _lineLength;
            return createEndOfData();
        }
        readNextLine();
        if (_lineLength == 0) {
            return createEndOfData();
        }
        _position.nextLine();
    } else {
        _position.nextColumn();
    }
    auto result = decodeNext();
    if (result == Char::ByteOrderMark) {
        if (_position.line() == 1 && _position.column() == 1) {
            _position = Position{1, 1}; // Reset the position.
            return next(); // Skip the BOM at the beginning of the document and return the next character.
        }
        throwEncodingError(u8"Read BOM in the middle of the document.");
    }
    // filter invalid control sequences in the configuration language.
    if (result != CharClass::ValidLang) {
        throwCharacterError(u8"Invalid control character.");
    }
    // Pass-through everything else.
    return result;
}


void CharStream::readNextLine() {
    // Fill the buffer with the next chunk of line data.
    _lineLength = _source->readLine(_line);
    // Important: As the char stream is not only used to verify, but also to create document signatures,
    // `_hashEnabled` can be set manually. In these cases, when re-signing a document that already has a
    // `\@signature` line - the first line must be skipped when building the hash.
    if (_position.line() == 0 && isSignatureLine()) {
        // 1. Enable hashing if this was the first line, and we found a `@signature` value.
        // (line counter starts at zero, as it is increased *after* reading the line.)
        // 2. Also, skipping this line for hash-calculation.
        _hashEnabled = true;
    } else if (_hashEnabled && _lineLength > 0) {
        _hash.update(std::span(_line.begin(), _lineLength));
    }
    _lineCurrentIndex = 0;
    _lineCharacterStartIndex = 0;
    _captureStartLine = static_cast<std::size_t>(_position.line()) + 1U; // will be increased after this call.
    _captureStartIndex = 0; // Reset the capture start.
}


auto CharStream::decodeNext() -> DecodedChar {
    _lineCharacterStartIndex = _lineCurrentIndex;
    try {
        const auto character = U8Decoder<std::byte>::decodeChar(_line, _lineCurrentIndex);
        return DecodedChar{character, _lineCharacterStartIndex, _position};
    } catch (const Error &error) {
        throw Error{ErrorCategory::Encoding, error.message(), Location{_source->identifier(), _position}};
    }
}


auto CharStream::createEndOfData() noexcept -> DecodedChar {
    if (!_endOfData) {
        // Fix the file position when returning the end of data mark for the first time.
        if (_position.isUndefined()) {
            _position = Position{1, 1};
        } else {
            _position.nextColumn();
        }
        _endOfData = true;
        if (_hashEnabled) {
            _digest = _hash.digest();
        }
    }
    return DecodedChar{Char::EndOfData, _lineCurrentIndex, _position};
}


void CharStream::throwEncodingError(String message) const  {
    throw Error(ErrorCategory::Encoding, std::move(message), Location{_source->identifier(), _position});
}


void CharStream::throwCharacterError(String message) const {
    throw Error(ErrorCategory::Character, std::move(message), Location{_source->identifier(), _position});
}


void CharStream::throwInternalError(String message) const {
    throw Error(ErrorCategory::Internal, std::move(message), Location{_source->identifier(), _position});
}


auto CharStream::isSignatureLine() const noexcept -> bool {
    // Use a lower and upper case version of the string "@signature" and "@SIGNATURE" to do low-level detection
    // of signed documents.
    // If a document has no signature, it makes no sense to calculate a hash for it.
    // The timing of handling this in the parser is not optimal: While the initial implementation would allow it,
    // it is possible that the lexer could read-ahead in a later version of the parser.
    static auto signatureLowerCase = Bytes::convertFrom(
        {0x40U, 0x73U, 0x69U, 0x67U, 0x6eU, 0x61U, 0x74U, 0x75U, 0x72U, 0x65U}); // "@signature"
    static auto signatureUpperCase = Bytes::convertFrom(
        {0x40U, 0x53U, 0x49U, 0x47U, 0x4eU, 0x41U, 0x54U, 0x55U, 0x52U, 0x45U}); // "@SIGNATURE"
    // Skip any BOM that may be present in the first line of the document.
    std::size_t startIndex = 0;
    constexpr std::size_t bomSize = 3;
    if (_lineLength >= bomSize && _line[0] == std::byte{0xEFU} && _line[1] == std::byte{0xBBU} && _line[2] == std::byte{0xBFU}) {
        startIndex = 3;
    }
    if (_lineLength < (signatureLowerCase.size() + startIndex)) {
        return false;
    }
    // Scan for a signature value name at the start of the line.
    for (std::size_t i = 0; i < signatureLowerCase.size(); ++i) {
        if (signatureLowerCase[i] != _line[startIndex + i] && signatureUpperCase[i] != _line[startIndex + i]) {
            return false;
        }
    }
    return true;
}


#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
auto internalView(const CharStream &object) -> InternalViewPtr {
    auto result = InternalView::create();
    result->setValue("source", *object._source);
    result->setValue("endOfData", object._endOfData);
    result->setValue("line", std::format("array(size={})", limits::maxLineLength + 1));
    result->setValue("lineLength", object._lineLength);
    result->setValue("lineCurrentIndex", object._lineCurrentIndex);
    result->setValue("lineCharacterStartIndex", object._lineCharacterStartIndex);
    result->setValue("captureStartLine", object._captureStartLine);
    result->setValue("captureStartIndex", object._captureStartIndex);
    result->setValue("position", object._position);
    return result;
}
#endif


}

