// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TestHelper.hpp"

#include <erbsland/conf/Parser.hpp>
#include <erbsland/conf/impl/utf8/U8StringView.hpp>

#include <algorithm>
#include <filesystem>
#include <map>


using namespace el::conf;
using impl::U8StringView;


class ParserTestHelper : public TestHelper {
public:
    using ExpectedValueMap = std::map<String, String>;

    class MockSource final : public Source {
    public:
        MockSource() = default;
        ~MockSource() override = default;
        [[nodiscard]] auto identifier() const noexcept -> SourceIdentifierPtr override {
            return SourceIdentifier::createForFile(u8"mock.elcl");
        }
        void open() override {
            _open = true;
            actions.push_back(u8"open");
        }
        [[nodiscard]] auto isOpen() const noexcept -> bool override {
            return _open;
        }
        [[nodiscard]] auto atEnd() const noexcept -> bool override {
            return currentLine >= lines.size();
        }
        [[nodiscard]] auto readLine(std::span<std::byte> lineBuffer) -> std::size_t override {
            if (atEnd()) {
                return 0;
            }
            Bytes line;
            if (std::holds_alternative<String>(lines[currentLine])) {
                line = U8StringView{std::get<String>(lines[currentLine])}.toBytes();
            } else {
                line = std::get<Bytes>(lines[currentLine]);
            }
            const auto lineLength = std::min(line.size(), lineBuffer.size());
            std::copy_n(line.begin(), lineLength, lineBuffer.begin());
            currentLine += 1;
            actions.push_back(u8"readLine");
            return lineLength;
        }
        void close() noexcept override {
            _open = false;
            actions.push_back(u8"close");
        }

        using Line = std::variant<String, Bytes>;

        std::size_t currentLine = 0;
        std::vector<Line> lines;
        std::vector<String> actions;

        bool _open = false;
    };
    using MockSourcePtr = std::shared_ptr<MockSource>;

public:
    void verifyValueMap(const ExpectedValueMap &expectedValueMap) {
        auto flatMap = doc->toFlatValueMap();
        // First, convert and verify all name paths.
        auto actualValues = std::map<String, String>{};
        for (auto it = flatMap.begin(); it != flatMap.end(); ++it) {
            if (it->second->type() == ValueType::Document) {
                continue; // ignore the document itself.
            }
            auto namePathText = it->first.toText();
            runWithContext(SOURCE_LOCATION(), [&]() {
                REQUIRE(expectedValueMap.contains(namePathText));
            }, [&]() -> std::string {
                return std::format("Unexpected additional value: {} = {}", namePathText, it->second->toTestText());
            });
            actualValues[namePathText] = it->second->toTestText();
        }
        // Now test if all expected values are part of the document.
        for (const auto &[expectedNamePath, expectedValueText] : expectedValueMap) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                REQUIRE(actualValues.contains(expectedNamePath));
            }, [&]() -> std::string {
                return std::format("Missing value: {} = {}", expectedNamePath, expectedValueText);
            });
            const auto actualValueText = actualValues[expectedNamePath];
            if (expectedValueText.find(u8"Float(") == 0) {
                // special handling for floating point values.
                const auto expectedFloat = std::stof(
                    expectedValueText.substr(6, expectedValueText.size() - 7).toCharString());
                REQUIRE(actualValueText.find(u8"Float(") == 0);
                const auto actualFloat = std::stof(
                    actualValueText.substr(6, actualValueText.size() - 7).toCharString());
                REQUIRE_LESS(std::abs(actualFloat - expectedFloat), std::numeric_limits<double>::epsilon());
            } else {
                REQUIRE_EQUAL(actualValueText, expectedValueText);
            }
        }
    }

    auto createTestFile(
        const std::filesystem::path &relativePath,
        const String &text) -> std::filesystem::path {

        if (!relativePath.is_relative()) {
            throw std::logic_error("The path must be relative.");
        }
        const auto filePath = useTestFileDirectory() / relativePath;
        std::filesystem::create_directories(filePath.parent_path());
        std::ofstream stream(filePath, std::ios::binary);
        stream << text.toCharString();
        stream.close();
        return filePath;
    }

public:
    DocumentPtr doc; ///< Document instance.
};


