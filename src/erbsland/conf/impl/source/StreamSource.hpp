// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "StreamTestInterface.hpp"

#include "../../Source.hpp"


namespace erbsland::conf::impl {


/// A stream source base class.
///
/// @tested Tested via `FileSource` and `StringSource` classes.
///
class StreamSource : public Source {
public:
    using Stream = std::basic_istream<char>;
    static constexpr std::size_t cBufferSize = 8192;

public:
    StreamSource() = default;
    ~StreamSource() override = default;

public:
    void open() override;
    [[nodiscard]] auto isOpen() const noexcept -> bool override;
    [[nodiscard]] auto atEnd() const noexcept -> bool override;
    [[nodiscard]] auto readLine(std::span<std::byte> lineBuffer) -> std::size_t override;
    void close() noexcept override;

protected:
    /// Open the stream.
    ///
    /// Implement this method to open the stream for reading if this is not already done in the constructor.
    ///
    /// @throws Error (IO) If an error occurs while opening the stream.
    ///
    virtual void openStream() = 0;

    /// Access the stream.
    ///
    /// The stream is only accessed if the source is open.
    /// Therefore, when you implement this method, you can safely assume that `open()`,
    /// and, therefore `openStream()` was called before the stream is accessed.
    ///
    virtual auto stream() noexcept -> Stream& = 0;

    /// Close the stream.
    ///
    /// This method is called if the stream reports `eof`, or when `close()` is called.
    ///
    virtual void closeStream() noexcept = 0;

private:
    /// Refill the buffer.
    ///
    void refillBuffer();

    /// Test how long the next line is.
    ///
    /// @return The next line length, including the newline or zero if no newline was found.
    ///
    auto nextLineLength() noexcept -> std::size_t;

    /// Close this source.
    ///
    void closeSource() noexcept;

    /// Mark the source as at the end.
    ///
    void sourceIsAtEnd() noexcept;

    /// Throw if the line length was exceeded.
    ///
    void throwLineLengthExceeded();

    /// Throw when tried to read if the source is closed.
    ///
    void throwSourceNotOpen();

    /// Throw when the passed line buffer was too small.
    ///
    void throwLineBufferTooSmall();

    /// Close the stream and flag there is no more data.
    ///
    void closeStreamAndFlagNoMoreData();

    /// Initialize the read process on first access.
    ///
    void initializeRead();

    /// Copy a complete line from the buffer to the output.
    ///
    [[nodiscard]] auto readWholeLine(std::span<std::byte> lineBuffer, std::size_t lineLength) -> std::size_t;

    /// Read a line that spans more than one buffer.
    /// 
    [[nodiscard]] auto readRemainingLine(std::span<std::byte> lineBuffer, std::size_t lineLength) -> std::size_t;

protected:
    bool _isInitialized{false}; ///< Initialization flag
    std::array<std::byte, cBufferSize> _buffer{}; ///< A round buffer to read data
    bool _sourceIsOpen{false}; ///< If this source is open.
    bool _sourceIsAtEnd{false}; ///< If this source is at the end.
    bool _streamHasMoreData{true}; ///< If the stream has more data to refill the buffer.
    std::size_t _readOffset{0}; ///< The offset for the next read operation.
    std::size_t _bufferSize{0}; ///< How much data the buffer contains.

public: // interfaces only used for testing.
    ERBSLAND_CONF_ONLY_FOR_UNITTEST(void setStreamTestInterface(StreamTestInterface *ti) { _testInterface = ti; };)
    ERBSLAND_CONF_ONLY_FOR_UNITTEST(StreamTestInterface *_testInterface{nullptr};)
};


}

