// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/Error.hpp>
#include <erbsland/conf/Source.hpp>

#include <erbsland/conf/impl/source/FileSource.hpp>
#include <erbsland/conf/impl/Limits.hpp>
#include <erbsland/conf/impl/source/StreamSource.hpp>
#include <erbsland/conf/impl/source/StreamTestInterface.hpp>

#include <cassert>


using namespace el::conf;
using namespace std::filesystem;


static constexpr std::size_t cLineBufferSize = 5000; // add extra bytes, to test for overflow.


TESTED_TARGETS(Source)
class FileSourceTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    SourcePtr source{}; ///< The current source.
    std::array<std::byte, cLineBufferSize> lineBuffer{}; ///< The line buffer to read back the lines.
    std::size_t lineLength{0}; ///< The reported line length.
    path temporaryTestFileDirectory{}; ///< The temporary directory to perform all tests.

    /// Clear the line buffer with a special pattern.
    void clearLineBuffer() {
        // fill the line buffer with a data pattern.
        assert(lineBuffer.size() == cLineBufferSize);
        for (std::size_t i = 0; i < lineBuffer.size(); ++i) {
            lineBuffer[i] = std::byte(0xfc + i % 4);
        }
    }

    /// Get the changed byte count in the line buffer.
    auto getChangedByteCount() -> std::size_t {
        assert(lineBuffer.size() == cLineBufferSize);
        for (std::size_t i = lineBuffer.size(); i > 0; --i) {
            if (lineBuffer[i-1] != std::byte(0xfc + (i - 1) % 4)) {
                return i;
            }
        }
        return 0;
    }

    void setUp() override {
        clearLineBuffer();
    }

    void tearDown() override {
        cleanUpTestFileDirectory();
    }

public: // --------------- tests start here ----------------
    void testConstruction() {
        auto filePath = createTestFile("[main]");
        source = Source::fromFile(filePath);
        REQUIRE(source != nullptr);
        REQUIRE(source->name() == u8"file");
        REQUIRE(source->path() == String{filePath.string()});
        REQUIRE(source->identifier()->toText() == String(u8"file:") + filePath.string());
        REQUIRE(source->isOpen() == false);
        REQUIRE(source->atEnd() == false);
    }

    void testConstructionWithInvalidPath() {
        auto filePath = path("/this/path/does/not/exist");
        REQUIRE_NOTHROW(source = Source::fromFile(filePath));
        try {
            source->open();
            REQUIRE(false);
        } catch (Error const &error) {
            REQUIRE(error.category() == ErrorCategory::IO);
        }
    }

    void testCloseWithNoRead() {
        auto filePath = createTestFile("[main]");
        source = Source::fromFile(filePath);
        REQUIRE(source != nullptr);
        REQUIRE_NOTHROW(source->open());
        REQUIRE(source->name() == u8"file");
        REQUIRE(source->path() == String{filePath.string()});
        REQUIRE(source->identifier()->toText() == String(u8"file:") + filePath.string());
        REQUIRE(source->isOpen() == true);
        REQUIRE(source->atEnd() == false);
        source->close();
        REQUIRE(source->name() == u8"file");
        REQUIRE(source->path() == String{filePath.string()});
        REQUIRE(source->identifier()->toText() == String(u8"file:") + filePath.string());
        REQUIRE(source->isOpen() == false);
        REQUIRE(source->atEnd() == false); // we didn't read from the source.
    }

    void requireValidShortFileOfSize(std::size_t size) {
        clearLineBuffer();
        std::vector<std::byte> content(size, std::byte(0x41 + size % 26));
        auto filePath = createTestFile(Bytes{content});
        source = Source::fromFile(filePath);
        REQUIRE(source != nullptr);
        REQUIRE_NOTHROW(source->open());
        REQUIRE(source->name() == u8"file");
        REQUIRE(source->path() == String{filePath.string()});
        REQUIRE(source->identifier()->toText() == String(u8"file:") + filePath.string());
        REQUIRE(source->isOpen() == true);
        REQUIRE(source->atEnd() == false);
        try {
            lineLength = source->readLine(std::span<std::byte>{lineBuffer});
        } catch (const Error &error) {
            consoleWriteLine(
                std::format(
                    "At size {}, unexpected exception when first calling `readLine`: {}",
                    size,
                    error.message().toCharString()));
            throw;
        }
        REQUIRE(lineLength == size);
        REQUIRE(getChangedByteCount() == size);
        REQUIRE(std::equal(content.begin(), content.end(), lineBuffer.begin()));
        REQUIRE(source->atEnd() == true);
        try {
            lineLength = source->readLine(lineBuffer);
        } catch (const Error &error) {
            consoleWriteLine(
                std::format(
                    "At size {}, unexpected exception when calling `readLine` after end: {}",
                    size,
                    error.message().toCharString()));
            throw;
        }
        REQUIRE(lineLength == 0);
        REQUIRE(source->isOpen() == false);
    }

    void testValidShortFilesWithoutNewlines() {
        // valid files.
        constexpr std::size_t validSizes[] = {
            0, 1, 2, 3,
            100, 128, 200, 400, 1000, 1024, 1999, 2048,
            limits::maxLineLength - 3,
            limits::maxLineLength - 2,
            limits::maxLineLength - 1,
            limits::maxLineLength};
        for (std::size_t size : validSizes) {
            try {
                WITH_CONTEXT(requireValidShortFileOfSize(size));
            } catch (const erbsland::AssertFailed&) {
                consoleWriteLine(std::format("Failed for valid size {}", size));
                throw;
            }
        }
    }

    void requireInvalidShortFileOfSize(std::size_t size) {
        clearLineBuffer();
        std::vector<std::byte> content(size, std::byte(0x41 + size % 26));
        auto filePath = createTestFile(Bytes{content});
        source = Source::fromFile(filePath);
        REQUIRE(source != nullptr);
        REQUIRE_NOTHROW(source->open());
        REQUIRE(source->name() == u8"file");
        REQUIRE(source->path() == String{filePath.string()});
        REQUIRE(source->identifier()->toText() == String(u8"file:") + filePath.string());
        REQUIRE(source->isOpen() == true);
        REQUIRE(source->atEnd() == false);
        try {
            lineLength = source->readLine(std::span<std::byte>{lineBuffer});
            REQUIRE(false);
        } catch (const Error &error) {
            REQUIRE(error.category() == ErrorCategory::LimitExceeded);
        }
        REQUIRE(source->isOpen() == false);
        REQUIRE(getChangedByteCount() <= limits::maxLineLength); // make sure no more bytes are changed as the maximum.
    }

    void testInvalidShortFilesWithoutNewlines() {
        // invalid files.
        constexpr std::size_t invalidSizes[] = {
            limits::maxLineLength + 1,
            limits::maxLineLength + 2,
            limits::maxLineLength + 3,
            limits::maxLineLength + 100,
            limits::maxLineLength * 2 - 1,
            limits::maxLineLength * 2,
            limits::maxLineLength * 2 + 1
        };
        for (std::size_t size : invalidSizes) {
            try {
                WITH_CONTEXT(requireInvalidShortFileOfSize(size));
            } catch (const erbsland::AssertFailed&) {
                consoleWriteLine(std::format("Failed for invalid size {}", size));
                throw;
            }
        }
    }

    void testReadFromManuallyClosedSource() {
        auto filePath = createTestFile("[main]\nline1: 12");
        // test 1: immediately close the source.
        source = Source::fromFile(filePath);
        REQUIRE_NOTHROW(source->open());
        source->close();
        REQUIRE_THROWS_AS(Error, source->readLine(lineBuffer));
        // test 2: read the first line, then close the source.
        source = Source::fromFile(filePath);
        REQUIRE_NOTHROW(source->open());
        REQUIRE_NOTHROW(source->readLine(lineBuffer));
        source->close();
        REQUIRE_THROWS_AS(Error, source->readLine(lineBuffer));
    }

    void testReadIntoSmallLineBuffer() {
        auto filePath = createTestFile("[main]\nline1: 12");
        std::array<std::byte, 0> buffer0;
        source = Source::fromFile(filePath);
        REQUIRE_NOTHROW(source->open());
        REQUIRE_THROWS_AS(Error, lineLength = source->readLine(buffer0));
        std::array<std::byte, 1> buffer1;
        source = Source::fromFile(filePath);
        REQUIRE_NOTHROW(source->open());
        REQUIRE_THROWS_AS(Error, lineLength = source->readLine(buffer1));
        std::array<std::byte, 100> buffer100;
        source = Source::fromFile(filePath);
        REQUIRE_NOTHROW(source->open());
        REQUIRE_THROWS_AS(Error, lineLength = source->readLine(buffer100));
        std::array<std::byte, limits::maxLineLength - 1> bufferMinus1;
        source = Source::fromFile(filePath);
        REQUIRE_NOTHROW(source->open());
        REQUIRE_THROWS_AS(Error, lineLength = source->readLine(bufferMinus1));
    }

    void requireExactLineMatches(const FileLines& lines) {
        auto filePath = createTestFile(lines);
        source = Source::fromFile(filePath);
        REQUIRE_NOTHROW(source->open());
        std::size_t lineIndex;
        try {
            for (lineIndex = 0; lineIndex < lines.size(); ++lineIndex) {
                const auto &line = lines[lineIndex];
                if (line.size() > limits::maxLineLength) {
                    REQUIRE_THROWS_AS(Error, lineLength = source->readLine(lineBuffer));
                    return;
                }
                lineLength = source->readLine(lineBuffer);
                REQUIRE(lineLength == line.size());
                REQUIRE(std::equal(line.begin(), line.end(), lineBuffer.begin()));
            }
        } catch (const erbsland::AssertFailed&) {
            consoleWriteLine(std::format("Failed at line {}. Expected {} bytes, but read {} bytes.",
                lineIndex + 1, lines[lineIndex].size(), lineLength));
            writeBytesToConsole("Expected", lines[lineIndex]);
            writeBytesToConsole("Actual", std::vector(lineBuffer.begin(), lineBuffer.begin() + lineLength));
            throw;
        }
        try {
            REQUIRE(source->atEnd() == true);
        } catch (const erbsland::AssertFailed&) {
            CHECK_NOTHROW(lineLength = source->readLine(lineBuffer));
            consoleWriteLine(std::format("Next read reports a line length of {}", lineLength));
            throw;
        }
    }

    void requireValidAssortedLines(LineBreak lineBreak, LineBreak lastLineBreak) {
        auto maxLineLength = limits::maxLineLength - 1;
        if (lineBreak == LineBreak::CRLF) {
            maxLineLength -= 1;
        }
        const auto lineLengths = std::vector<std::size_t>{
            1, 2, 3, 100, 0, 200, 2, 5, 600, 600,
            500, 1000, 1023, 0, 0, maxLineLength, maxLineLength - 1, 1, 100, 500, 3049
        };
        auto lines = generateLines(lineLengths, lineBreak, lastLineBreak);
        WITH_CONTEXT(requireExactLineMatches(lines));
    }

    void testFileWithVariousLineLengths() {
        WITH_CONTEXT(requireValidAssortedLines(LineBreak::LF, LineBreak::None));
        WITH_CONTEXT(requireValidAssortedLines(LineBreak::CRLF, LineBreak::None));
        WITH_CONTEXT(requireValidAssortedLines(LineBreak::LF, LineBreak::LF));
        WITH_CONTEXT(requireValidAssortedLines(LineBreak::CRLF, LineBreak::LF));
        WITH_CONTEXT(requireValidAssortedLines(LineBreak::LF, LineBreak::CRLF));
        WITH_CONTEXT(requireValidAssortedLines(LineBreak::CRLF, LineBreak::CRLF));
    }

    void testSpecialLineLengths() {
        // try to align line lengths with the buffer size to cover more branches.
        constexpr auto quadBufferSize = impl::StreamSource::cBufferSize / 4u - 1u;
        auto lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize
        };
        auto lines = generateLines(lineLengths, LineBreak::LF, LineBreak::LF);
        WITH_CONTEXT(requireExactLineMatches(lines));
        // the same, but without a trailing linebreak.
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize + 1
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::None);
        WITH_CONTEXT(requireExactLineMatches(lines));
        // Move the linebreak just to the next buffer.
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize + 1,
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::LF);
        WITH_CONTEXT(requireExactLineMatches(lines));
        // Just one byte before the next buffer starts.
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize - 1,
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::LF);
        WITH_CONTEXT(requireExactLineMatches(lines));
        // A continued line, without newline.
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, limits::maxLineLength
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::None);
        WITH_CONTEXT(requireExactLineMatches(lines));
    }

    void testTooLongLineLengths() {
        // try to align line lengths with the buffer size to cover more branches.
        constexpr auto quadBufferSize = impl::StreamSource::cBufferSize / 4u - 1u;
        auto lineLengths = std::vector<std::size_t>{
            limits::maxLineLength + 1
        };
        auto lines = generateLines(lineLengths, LineBreak::LF, LineBreak::None);
        WITH_CONTEXT(requireExactLineMatches(lines));
        // one byte too much in a second line.
        lineLengths = std::vector<std::size_t>{
            0, limits::maxLineLength + 1
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::None);
        WITH_CONTEXT(requireExactLineMatches(lines));
        // Same tests with ending line-breaks.
        lineLengths = std::vector<std::size_t>{
            limits::maxLineLength, // error
            quadBufferSize, quadBufferSize, quadBufferSize
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::LF);
        WITH_CONTEXT(requireExactLineMatches(lines));
        lineLengths = std::vector<std::size_t>{
            0, limits::maxLineLength, // error
            quadBufferSize, quadBufferSize, quadBufferSize
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::LF);
        WITH_CONTEXT(requireExactLineMatches(lines));
        // Test the behaviour, for the second buffer read.
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            limits::maxLineLength, // error
            quadBufferSize, quadBufferSize, quadBufferSize
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::LF);
        WITH_CONTEXT(requireExactLineMatches(lines));
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            0, limits::maxLineLength, // error
            quadBufferSize, quadBufferSize, quadBufferSize
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::LF);
        WITH_CONTEXT(requireExactLineMatches(lines));
        // Test the behaviour across buffer boundaries
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize,
            limits::maxLineLength, // error
            quadBufferSize, quadBufferSize, quadBufferSize
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::LF);
        WITH_CONTEXT(requireExactLineMatches(lines));
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize - 2,
            0, limits::maxLineLength, // error
            quadBufferSize, quadBufferSize, quadBufferSize
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::LF);
        WITH_CONTEXT(requireExactLineMatches(lines));
        // Same tests, but at the end of the file with no trailing line-break.
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize,
            limits::maxLineLength + 1 // error
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::None);
        WITH_CONTEXT(requireExactLineMatches(lines));
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            limits::maxLineLength + 1 // error
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::None);
        WITH_CONTEXT(requireExactLineMatches(lines));
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            0, limits::maxLineLength + 1 // error
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::None);
        WITH_CONTEXT(requireExactLineMatches(lines));
        lineLengths = std::vector<std::size_t>{
            quadBufferSize, quadBufferSize, quadBufferSize, quadBufferSize,
            100, limits::maxLineLength + 1 // error
        };
        lines = generateLines(lineLengths, LineBreak::LF, LineBreak::None);
        WITH_CONTEXT(requireExactLineMatches(lines));
    }

    void testErrorOnOpeningFile() {
        struct TI : impl::StreamTestInterface {
            void afterOpen(Stream &stream) const override {
                throw std::ios_base::failure("Opening file failed");
            }
        };
        TI test;
        auto filePath = createTestFile("[main]");
        source = Source::fromFile(filePath);
        std::dynamic_pointer_cast<impl::FileSource>(source)->setStreamTestInterface(&test);
        REQUIRE_THROWS_AS(Error, source->open());
    }

    void testErrorOnClosingFile() {
        struct TI : impl::StreamTestInterface {
            void afterClose(Stream &stream) const override {
                throw std::ios_base::failure("Closing file failed");
            }
        };
        TI test;
        auto filePath = createTestFile("[main]");
        source = Source::fromFile(filePath);
        std::dynamic_pointer_cast<impl::FileSource>(source)->setStreamTestInterface(&test);
        REQUIRE_NOTHROW(source->open());
        lineLength = source->readLine(lineBuffer);
        REQUIRE(source->atEnd());
        source->close();
    }

    void testErrorAtFirstReading() {
        struct TI : impl::StreamTestInterface {
            void beforeRead(Stream &stream) const override {
                throw std::ios_base::failure("Reading file failed.");
            }
        };
        TI test;
        auto filePath = createTestFile("[main]");
        source = Source::fromFile(filePath);
        std::dynamic_pointer_cast<impl::FileSource>(source)->setStreamTestInterface(&test);
        REQUIRE_NOTHROW(source->open());
        REQUIRE_THROWS_AS(Error, lineLength = source->readLine(lineBuffer));
    }
};

