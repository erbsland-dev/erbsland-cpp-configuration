// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/conf/Bytes.hpp>
#include <erbsland/conf/Source.hpp>
#include <erbsland/conf/String.hpp>
#include <erbsland/conf/impl/utf8/U8Format.hpp>
#include <erbsland/unittest/all.hpp>

#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string_view>


class TestHelper : public el::UnitTest {
public:
    /// Lines of bytes
    using FileLines = std::vector<std::vector<std::byte>>;

    /// The type of line ending to generate.
    enum class LineBreak : uint8_t {
        None,
        LF,
        CRLF
    };

public:
    ~TestHelper() override = default;

public:
    /// Test all operators
    ///
    /// Pass six arguments that are:
    /// a1 == b1, a2 == b2, a3 == b3
    /// a1 < a2, a2 < a3, b1 < b2, b2 < b3
    ///
    /// The test always puts a on the left, and b on the right side of the operator.
    template<typename A, typename B>
    void requireAllOperators(
        const A &a1, const A &a2, const A &a3,
        const B &b1, const B &b2, const B &b3) {

        // Test ==
        REQUIRE(a1 == b1);
        REQUIRE_FALSE(a1 == b2);
        REQUIRE_FALSE(a1 == b3);
        REQUIRE_FALSE(a2 == b1);
        REQUIRE(a2 == b2);
        REQUIRE_FALSE(a2 == b3);
        REQUIRE_FALSE(a3 == b1);
        REQUIRE_FALSE(a3 == b2);
        REQUIRE(a3 == b3);

        // Test !=
        REQUIRE_FALSE(a1 != b1);
        REQUIRE(a1 != b2);
        REQUIRE(a1 != b3);
        REQUIRE(a2 != b1);
        REQUIRE_FALSE(a2 != b2);
        REQUIRE(a2 != b3);
        REQUIRE(a3 != b1);
        REQUIRE(a3 != b2);
        REQUIRE_FALSE(a3 != b3);

        // Test <
        REQUIRE_FALSE(a1 < b1);
        REQUIRE(a1 < b2);
        REQUIRE(a1 < b3);
        REQUIRE_FALSE(a2 < b1);
        REQUIRE_FALSE(a2 < b2);
        REQUIRE(a2 < b3);
        REQUIRE_FALSE(a3 < b1);
        REQUIRE_FALSE(a3 < b2);
        REQUIRE_FALSE(a3 < b3);

        // Test <=
        REQUIRE(a1 <= b1);
        REQUIRE(a1 <= b2);
        REQUIRE(a1 <= b3);
        REQUIRE_FALSE(a2 <= b1);
        REQUIRE(a2 <= b2);
        REQUIRE(a2 <= b3);
        REQUIRE_FALSE(a3 <= b1);
        REQUIRE_FALSE(a3 <= b2);
        REQUIRE(a3 <= b3);

        // Test >
        REQUIRE_FALSE(a1 > b1);
        REQUIRE_FALSE(a1 > b2);
        REQUIRE_FALSE(a1 > b3);
        REQUIRE(a2 > b1);
        REQUIRE_FALSE(a2 > b2);
        REQUIRE_FALSE(a2 > b3);
        REQUIRE(a3 > b1);
        REQUIRE(a3 > b2);
        REQUIRE_FALSE(a3 > b3);

        // Test >=
        REQUIRE(a1 >= b1);
        REQUIRE_FALSE(a1 >= b2);
        REQUIRE_FALSE(a1 >= b3);
        REQUIRE(a2 >= b1);
        REQUIRE(a2 >= b2);
        REQUIRE_FALSE(a2 >= b3);
        REQUIRE(a3 >= b1);
        REQUIRE(a3 >= b2);
        REQUIRE(a3 >= b3);

        // Test <=>
        REQUIRE((a1 <=> b1) == std::strong_ordering::equal);
        REQUIRE((a1 <=> b2) == std::strong_ordering::less);
        REQUIRE((a1 <=> b3) == std::strong_ordering::less);
        REQUIRE((a2 <=> b1) == std::strong_ordering::greater);
        REQUIRE((a2 <=> b2) == std::strong_ordering::equal);
        REQUIRE((a2 <=> b3) == std::strong_ordering::less);
        REQUIRE((a3 <=> b1) == std::strong_ordering::greater);
        REQUIRE((a3 <=> b2) == std::strong_ordering::greater);
        REQUIRE((a3 <=> b3) == std::strong_ordering::equal);
    }

    template<typename T, std::size_t tSize>
    void requireStrictOrder(const std::array<T, tSize> &valuesInOrder) {
        for (std::size_t i = 0; i < tSize; ++i) {
            for (std::size_t j = 0; j < tSize; ++j) {
                const auto &iValue = valuesInOrder[i];
                const auto &jValue = valuesInOrder[j];
                auto isEqual = i == j;
                auto isLess = i < j;
                auto isLessOrEqual = i <= j;
                auto isGreater = i > j;
                auto isGreaterOrEqual = i >= j;
                REQUIRE(isEqual == (iValue == jValue));
                REQUIRE(isLess == (iValue < jValue));
                REQUIRE(isLessOrEqual == (iValue <= jValue));
                REQUIRE(isGreater == (iValue > jValue));
                REQUIRE(isGreaterOrEqual == (iValue >= jValue));
                REQUIRE((iValue <=> jValue) == (i <=> j));
            }
        }
    }

    /// Generate a random hex string.
    auto generateRandomHex(std::size_t length) -> std::string {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        std::ostringstream oss;
        for (std::size_t i = 0; i < length; ++i) {
            oss << std::setw(2) << std::setfill('0') << std::hex << dis(gen);
        }
        return oss.str();
    }

    /// Generate lines for a test file.
    ///
    /// @param lineLengths An array with lengths, without line break.
    /// @param lineBreak The linebreak in the file.
    /// @param lastLineBreak The linebreak at the end of the file.
    ///
    auto generateLines(
        const std::vector<std::size_t> &lineLengths,
        const LineBreak lineBreak = LineBreak::LF,
        const LineBreak lastLineBreak = LineBreak::None) -> FileLines {

        std::vector<std::vector<std::byte>> lines;
        std::mt19937 gen(928391);
        std::uniform_int_distribution<> dis(0x20, 0x7e);
        for (std::size_t j = 0; j < lineLengths.size(); ++j) {
            const auto lineLength = lineLengths[j];
            std::vector<std::byte> line;
            line.reserve(lineLength);
            for (std::size_t i = 0; i < lineLength; ++i) {
                line.push_back(static_cast<std::byte>(dis(gen)));
            }
            switch ((j == lineLengths.size() - 1) ? lastLineBreak : lineBreak) {
            case LineBreak::CRLF:
                line.push_back(std::byte(0x0d));
                line.push_back(std::byte(0x0a));
                break;
            case LineBreak::LF:
                line.push_back(std::byte(0x0a));
                break;
            default:
                break;
            }
            lines.emplace_back(std::move(line));
        }
        return lines;
    }

    void writeBytesToConsole(std::string label, std::vector<std::byte> bytes) {
        std::stringstream ss;
        ss << label;
        ss << ": ";
        for (std::size_t i = 0; i < bytes.size(); ++i) {
            ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(bytes[i]);
        }
        consoleWriteLine(ss.str());
    }

public: // helper functions to work with generic test files.
    auto useTestFileDirectory() -> std::filesystem::path {
        if (_temporaryTestFileDirectory == nullptr) {
            auto temporaryTestFileDirectory = std::filesystem::temp_directory_path();
            temporaryTestFileDirectory /= "Erbsland_UnitTest_" + generateRandomHex(4);
            create_directories(temporaryTestFileDirectory);
            // Custom deleter that calls remove_all
            auto deleter = [](std::filesystem::path* path) {
                if (path) {
                    try {
                        std::filesystem::remove_all(*path);
                    } catch (std::exception &) { // NOLINT(*-empty-catch)
                        // ignore any exception.
                    }
                    delete path; // Explicitly delete the path pointer
                }
            };
            _temporaryTestFileDirectory = std::shared_ptr<std::filesystem::path>(
                new std::filesystem::path{temporaryTestFileDirectory}, deleter);
        }
        return *_temporaryTestFileDirectory;
    }

    auto cleanUpTestFileDirectory() {
        _temporaryTestFileDirectory.reset();
    }

    auto createTemporaryFilePath() -> std::filesystem::path {
        auto result = useTestFileDirectory();
        result /= generateRandomHex(8) + ".txt";
        return result;
    }

    void setTestContents(const erbsland::conf::String &text) {
        testContents = el::conf::String(std::format("UTF-8 Text, {} bytes (·=space ↦=tab ↲=newline ●=EOF):\n", text.size()));
        const std::u8string_view content{text.raw()};
        using erbsland::conf::String;
        for (auto character : content) {
            if (character == u8' ') {
                testContents += u8"·";
            } else if (character == u8'\t') {
                testContents += u8"↦";
            } else if (character == u8'\n') {
                testContents += u8"↲\n";
            } else {
                testContents += character;
            }
        }
        testContents += u8"●";
    }

    void setTestContents(const erbsland::conf::Bytes &content) {
        testContents = el::conf::String(std::format("Binary data, {} bytes:\n {}", content.size(), content));
    }

    void setTestContents(const FileLines &content) {
        testContents = el::conf::String(std::format("Artificial line data, {} lines:\n", content.size()));
        int counter = 0;
        for (const auto &line : content) {
            testContents += std::format("Line {}: {} bytes\n", counter++, line.size());
        }
    }

    auto createTestFile(const erbsland::conf::String &text) -> std::filesystem::path {
        auto filePath = createTemporaryFilePath();
        std::ofstream stream(filePath, std::ios::binary);
        stream << text.toCharString();
        stream.close();
        setTestContents(text);
        return filePath;
    }

    auto createTestFile(const erbsland::conf::Bytes &content) -> std::filesystem::path {
        auto filePath = createTemporaryFilePath();
        std::ofstream stream(filePath, std::ios::binary);
        stream.write(reinterpret_cast<const char *>(content.data()), static_cast<std::streamsize>(content.size()));
        stream.close();
        setTestContents(content);
        return filePath;
    }

    auto createTestFile(const FileLines &content) -> std::filesystem::path {
        auto filePath = createTemporaryFilePath();
        std::ofstream stream(filePath, std::ios::binary);
        for (const auto &line : content) {
            stream.write(reinterpret_cast<const char *>(line.data()), static_cast<std::streamsize>(line.size()));
        }
        stream.close();
        testContents = el::conf::String(std::format("Artificial line data, {} lines:\n", content.size()));
        int counter = 0;
        for (const auto &line : content) {
            testContents += std::format("Line {}: {} bytes\n", counter++, line.size());
            counter += 1;
        }
        return filePath;
    }

    auto createTestMemorySource(const erbsland::conf::String &text) -> erbsland::conf::SourcePtr {
        setTestContents(text);
        return erbsland::conf::Source::fromString(text);
    }

protected:
    erbsland::conf::String testContents;

private:
    std::shared_ptr<std::filesystem::path> _temporaryTestFileDirectory{};
};
