// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/Parser.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <filesystem>
#include <format>
#include <cstdlib>


using namespace el::conf;


TESTED_TARGETS(Parser)
class ParserComplianceTest final : public el::UnitTest {
public:
    std::filesystem::path testSuitePath;
    std::filesystem::path testFilePath;
    SourcePtr source;
    DocumentPtr document;

    auto additionalErrorMessages() -> std::string override {
        try {
            std::string result;
            result += std::format("Failed test file path: {}\n",
                std::filesystem::relative(testFilePath, testSuitePath).string());
            if (document != nullptr) {
                result += "State of the parsed document:\n";
                auto flatMap = document->toFlatValueMap();
                for (const auto &[namePath, value] : flatMap) {
                    result += namePath.toText().toCharString() + ": " + value->toTestText().toCharString() + "\n";
                }
            } else {
                result += "No document was parsed.\n";
            }
            return result;
        } catch (...) {
            return {"unexpected exception"};
        }
    }

    void validateTestFile(const std::filesystem::path &path, bool expectPass) {
        source = Source::fromFile(path);
        try {
            Parser parser;
            document = {};
            document = parser.parseOrThrow(source);
            if (!expectPass) {
                consoleWriteLine("Parsing file should have failed.");
                REQUIRE(expectPass);
            }
        } catch (const Error &error) {
            if (expectPass) {
                consoleWriteLine(error.toText().toCharString());
                REQUIRE_FALSE(expectPass);
            }
        }
    }

    void testPassOrFail() {
        testSuitePath = std::filesystem::path(std::getenv("ERBSLAND_CONF_TEST_SUITE"));
        if (testSuitePath.empty() || !std::filesystem::is_directory(testSuitePath)) {
            consoleWriteLine(std::format("Test suite directory was not found: {}", testSuitePath.string()));
            REQUIRE(false);
        }
        testSuitePath /= "tests";
        testSuitePath /= "V1_0";
        REQUIRE(std::filesystem::is_directory(testSuitePath));
        std::filesystem::recursive_directory_iterator it(testSuitePath);
        std::vector<std::filesystem::directory_entry> entries;
        for (const auto &entry : it) {
            if (entry.is_regular_file() && entry.path().extension() == ".elcl") {
                entries.push_back(entry);
            }
        }
        std::ranges::sort(
            entries,
            [](const auto &a, const auto &b) { return a.path() < b.path(); });
        for (const auto &entry : entries) {
            testFilePath = entry.path();
            bool expectPass = testFilePath.filename().string().find("PASS") != std::string::npos;
            WITH_CONTEXT(validateTestFile(entry.path(), expectPass));
        }
    }
};

