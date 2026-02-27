// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StreamSource.hpp"


#include "StreamTestInterface.hpp"

#include "../constants/Limits.hpp"
#include "../utf8/U8Format.hpp"

#include "../../Error.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>


namespace erbsland::conf::impl {


void StreamSource::open() {
    if (isOpen()) {
        throw Error(ErrorCategory::Internal, u8"The source is already open.", Location{identifier()});
    }
    openStream();
    _sourceIsOpen = true;
}


auto StreamSource::isOpen() const noexcept -> bool {
    return _sourceIsOpen;
}


auto StreamSource::atEnd() const noexcept -> bool {
    return _sourceIsAtEnd;
}


auto StreamSource::readLine(std::span<std::byte> lineBuffer) -> std::size_t {
    // Make sure the buffer is set up on the first access.
    initializeRead();

    // End of source: simply return without modifying the buffer.
    if (_sourceIsAtEnd) {
        return 0;
    }

    // Reading from a closed source while not at the end is a logic error.
    if (!_sourceIsOpen) {
        throwSourceNotOpen();
    }

    // The caller must always provide a buffer big enough for one line.
    if (lineBuffer.size() < limits::maxLineLength) {
        throwLineBufferTooSmall();
    }

    // When the last read used the complete buffer, we need to refill.
    if (_readOffset >= _buffer.size()) {
        assert(_streamHasMoreData);
        refillBuffer();
    }

    auto lineLength = nextLineLength();

    // If the whole line is already in the buffer, copy it at once.
    if (lineLength > 0) {
        return readWholeLine(lineBuffer, lineLength);
    }

    // Otherwise the line spans multiple buffer loads.
    return readRemainingLine(lineBuffer, lineLength);
}


void StreamSource::close() noexcept {
    closeSource();
    closeStreamAndFlagNoMoreData();
}


void StreamSource::refillBuffer() {
    try {
        _readOffset = 0;
        // The first call of `peek` will cause the `eof` bit set, in case no data was read from the file yet.
        stream().peek();
        // We reached the end of the file (probably the size of the file changed).
        if (stream().eof()) {
            _bufferSize = 0;
            closeStreamAndFlagNoMoreData();
            return;
        }
        ERBSLAND_CONF_STREAM_TEST(beforeRead);
        stream().read(reinterpret_cast<char *>(_buffer.data()), static_cast<std::streamsize>(_buffer.size()));
        if (stream().fail()) { // Could not fill the whole buffer.
            const auto count = stream().gcount(); // Determine how much data we got.
            _bufferSize = count > 0 ? static_cast<std::size_t>(count) : 0U;
        } else {
            _bufferSize = _buffer.size();
            stream().peek(); // peek one byte ahead, to see if there is more data.
        }
        if (stream().eof()) {
            closeStreamAndFlagNoMoreData();
        } else if (_bufferSize != _buffer.size()) { // Catch unexpected behavior of the stream.
            // Test Coverage: This is a very unlikely case that indicates a very bad std library implementation.
            close(); // If we throw an exception, close both the stream and source.
            throw Error(
                ErrorCategory::IO,
                u8format(
                    "Unexpected behaviour of data stream. Fail bit not set, but didn't fill the buffer. Read {} bytes.",
                    _bufferSize),
                Location{identifier()});
        }
    } catch (const std::ios_base::failure &e) {
        close(); // If we throw an exception, close both the stream and source.
        throw Error(
            ErrorCategory::IO,
            String(u8"Failed read from file. Error:") + String{std::string{e.what()}}.toEscaped(EscapeMode::ErrorText),
            Location{identifier()});
    }
}


auto StreamSource::nextLineLength() noexcept -> std::size_t {
    const auto rangeBegin = _buffer.begin() + _readOffset;
    const auto rangeEnd = _buffer.begin() + _bufferSize;
    const auto newlineIt = std::find(rangeBegin, rangeEnd, std::byte{'\n'});
    if (newlineIt != rangeEnd) {
        return static_cast<std::size_t>(std::distance(rangeBegin, newlineIt)) + 1U; // Include the newline in the length.
    }
    return 0; // No line end found.
}


void StreamSource::closeSource() noexcept {
    _readOffset = 0;
    _bufferSize = 0;
    _sourceIsOpen = false;
}


void StreamSource::sourceIsAtEnd() noexcept {
    closeSource();
    _sourceIsAtEnd = true;
}


void StreamSource::throwLineLengthExceeded() {
    close(); // In case the stream contains errors, close both the stream and source.
    throw Error(
        ErrorCategory::LimitExceeded,
        u8format("The line exceeds the maximum size of {} bytes.", limits::maxLineLength),
        Location{identifier()});
}


void StreamSource::throwSourceNotOpen() {
    throw Error(
        ErrorCategory::IO,
        u8"You cannot read from a closed source.",
        Location{identifier()});
}


void StreamSource::throwLineBufferTooSmall() {
    throw Error(
        ErrorCategory::LimitExceeded,
        u8format("Line buffer too small. Need at least {} bytes.", limits::maxLineLength));
}


void StreamSource::closeStreamAndFlagNoMoreData() {
    closeStream();
    _streamHasMoreData = false;
}


void StreamSource::initializeRead() {
    if (_isInitialized) {
        return;
    }
    _isInitialized = true;
    if (!_sourceIsOpen) {
        // Reading without opening the source is not allowed.
        throwSourceNotOpen();
    }

    // A new read always starts before the end is reached and while there is more data.
    assert(!_sourceIsAtEnd);
    assert(_streamHasMoreData);

    refillBuffer();

    // If nothing was read, the stream contains no data at all.
    if (_bufferSize == 0) {
        sourceIsAtEnd();
    }
}


auto StreamSource::readWholeLine(std::span<std::byte> lineBuffer, std::size_t lineLength) -> std::size_t {
    if (lineLength > limits::maxLineLength) {
        throwLineLengthExceeded();
    }

    // Line fits completely into the current buffer.
    const auto lineBegin = _buffer.begin() + _readOffset;
    std::copy_n(lineBegin, lineLength, lineBuffer.begin());
    _readOffset += lineLength;

    if (_readOffset >= _bufferSize && !_streamHasMoreData) {
        sourceIsAtEnd();
    }

    return lineLength;
}


auto StreamSource::readRemainingLine(std::span<std::byte> lineBuffer, std::size_t lineLength) -> std::size_t {
    // This branch handles a line split over multiple buffers.
    if (_bufferSize - _readOffset > limits::maxLineLength) {
        throwLineLengthExceeded();
    }

    lineLength = _bufferSize - _readOffset;
    const auto lineBegin = _buffer.begin() + _readOffset;
    std::copy_n(lineBegin, lineLength, lineBuffer.begin());

    if (!_streamHasMoreData) {
        sourceIsAtEnd();
        return lineLength;
    }

    auto nextWrite = lineBuffer.begin() + static_cast<std::span<std::byte>::difference_type>(lineLength);
    refillBuffer();
    auto continued = nextLineLength();
    if (continued == 0) {
        if (_bufferSize == 0) {
            // Test Coverage: This is a rare case when the size of the test file changes while reading it.
            assert(!_streamHasMoreData);
            sourceIsAtEnd();
            return lineLength;
        }
        lineLength += _bufferSize;
        if (lineLength > limits::maxLineLength) {
            throwLineLengthExceeded();
        }
        assert(!_streamHasMoreData);
        std::copy_n(_buffer.begin(), _bufferSize, nextWrite);
        sourceIsAtEnd();
        return lineLength;
    }
    lineLength += continued;
    if (lineLength > limits::maxLineLength) {
        throwLineLengthExceeded();
    }
    std::copy_n(_buffer.begin(), continued, nextWrite);
    _readOffset = continued;
    if (_readOffset >= _bufferSize && !_streamHasMoreData) {
        sourceIsAtEnd();
    }
    return lineLength;
}


}

