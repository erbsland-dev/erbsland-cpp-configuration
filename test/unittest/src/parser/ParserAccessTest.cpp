// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ParserTestHelper.hpp"


TESTED_TARGETS(Parser)
class ParserAccessTest final : public UNITTEST_SUBCLASS(ParserTestHelper) {
public:
    void tearDown() override {
        cleanUpTestFileDirectory();
        doc = {};
    }

    void expectParserError(
        const std::filesystem::path &path,
        const ErrorCategory errorCategory,
        String expectedWordInErrorMessage = {}) {
        try {
            Parser parser;
            doc = parser.parseOrThrow(Source::fromFile(path));
            REQUIRE(false);
        } catch (const Error &error) {
            REQUIRE_EQUAL(error.category(), errorCategory);
            if (!expectedWordInErrorMessage.empty()) {
                auto success = error.message().find(expectedWordInErrorMessage.raw()) != std::string::npos;
                if (!success) {
                    consoleWriteLine(error.toText().toCharString());
                }
                REQUIRE(success);
            }
        }
    }

    enum class TestFile {
        SameDirectory,
        Subdirectory,
        ParentDirectory,
        WrongSuffix,
    };

    struct TestFileData {
        std::filesystem::path path;
        String includeText;
    };

    std::map<TestFile, TestFileData> testFileData {
        {
            TestFile::SameDirectory,
            {
                "config/file.elcl",
                u8"file.elcl"
            }
        },
        {
            TestFile::Subdirectory,
            {
                "config/sub/file.elcl",
                u8"sub/file.elcl"
            }
        },
        {
            TestFile::ParentDirectory,
            {
                "file.elcl",
                u8"../file.elcl"
            }
        },
        {
            TestFile::WrongSuffix,
            {
                "config/file.txt",
                u8"file.txt"
            }
        },
    };

    struct TestData {
        TestFile testFile;
        std::vector<FileAccessCheck::Feature> enabledFeatures;
        std::vector<FileAccessCheck::Feature> disabledFeatures;
        bool expectedAccessGranted;
    };

    void verifyAccess(const TestData &data) {
        // Prepare the test environment.
        String document = u8"[main]\nvalue: 123\n@include: \"";
        const auto testFile = testFileData.at(data.testFile);
        document += testFile.includeText;
        document += u8"\"\n# end\n";
        auto mainFile = createTestFile("config/main.elcl", document);
        createTestFile(testFile.path, u8"[other]\nvalue: 456\n");
        // Create a custom access check.
        auto accessCheck = FileAccessCheck::create();
        for (auto feature : data.enabledFeatures) {
            accessCheck->enable(feature);
        }
        for (auto feature : data.disabledFeatures) {
            accessCheck->disable(feature);
        }
        // Set up the parser
        Parser parser;
        parser.setAccessCheck(accessCheck);
        auto source = Source::fromFile(mainFile);
        if (data.expectedAccessGranted) {
            REQUIRE_NOTHROW(doc = parser.parseOrThrow(source));
        } else {
            try {
                doc = parser.parseOrThrow(source);
                REQUIRE(false);
            } catch (const Error &error) {
                REQUIRE_EQUAL(error.category(), ErrorCategory::Access);
            }
        }
    }

    void testIncludeGranted() {
        const auto testData = std::vector<TestData>{
            // Establish the baseline, with all default settings.
            {TestFile::ParentDirectory, {}, {}, false}, // 0
            {TestFile::SameDirectory, {}, {}, true},
            {TestFile::Subdirectory, {}, {}, true},
            {TestFile::WrongSuffix, {}, {}, true},
            //
            {TestFile::ParentDirectory, {}, {FileAccessCheck::SameDirectory}, false}, // 4
            {TestFile::SameDirectory, {}, {FileAccessCheck::SameDirectory}, false},
            {TestFile::Subdirectory, {}, {FileAccessCheck::SameDirectory}, true},
            {TestFile::WrongSuffix, {}, {FileAccessCheck::SameDirectory}, false},
            //
            {TestFile::ParentDirectory, {}, {FileAccessCheck::Subdirectories}, false}, // 8
            {TestFile::SameDirectory, {}, {FileAccessCheck::Subdirectories}, true},
            {TestFile::Subdirectory, {}, {FileAccessCheck::Subdirectories}, false},
            {TestFile::WrongSuffix, {}, {FileAccessCheck::Subdirectories}, true},
            //
            {TestFile::ParentDirectory, {}, {FileAccessCheck::SameDirectory, FileAccessCheck::Subdirectories}, false}, // 12
            {TestFile::SameDirectory, {}, {FileAccessCheck::SameDirectory, FileAccessCheck::Subdirectories}, false},
            {TestFile::Subdirectory, {}, {FileAccessCheck::SameDirectory, FileAccessCheck::Subdirectories}, false},
            {TestFile::WrongSuffix, {}, {FileAccessCheck::SameDirectory, FileAccessCheck::Subdirectories}, false},
            //
            {TestFile::ParentDirectory, {FileAccessCheck::AnyDirectory}, {}, true}, // 16
            {TestFile::SameDirectory, {FileAccessCheck::AnyDirectory}, {}, true},
            {TestFile::Subdirectory, {FileAccessCheck::AnyDirectory}, {}, true},
            {TestFile::WrongSuffix, {FileAccessCheck::AnyDirectory}, {}, true},
            //
            {TestFile::ParentDirectory, {FileAccessCheck::AnyDirectory}, {FileAccessCheck::SameDirectory, FileAccessCheck::Subdirectories}, true}, // 20
            {TestFile::SameDirectory, {FileAccessCheck::AnyDirectory}, {FileAccessCheck::SameDirectory, FileAccessCheck::Subdirectories}, true},
            {TestFile::Subdirectory, {FileAccessCheck::AnyDirectory}, {FileAccessCheck::SameDirectory, FileAccessCheck::Subdirectories}, true},
            {TestFile::WrongSuffix, {FileAccessCheck::AnyDirectory}, {FileAccessCheck::SameDirectory, FileAccessCheck::Subdirectories}, true},
            //
            {TestFile::ParentDirectory, {FileAccessCheck::RequireSuffix}, {}, false}, // 24
            {TestFile::SameDirectory, {FileAccessCheck::RequireSuffix}, {}, true},
            {TestFile::Subdirectory, {FileAccessCheck::RequireSuffix}, {}, true},
            {TestFile::WrongSuffix, {FileAccessCheck::RequireSuffix}, {}, false},
        };
        int index = 0;
        for (const auto &data : testData) {
            runWithContext(SOURCE_LOCATION(), [&] {
                verifyAccess(data);
            }, [&]() -> std::string {
                return std::format("Failed for test case {}", index);
            });
            cleanUpTestFileDirectory();
            index += 1;
        }
    }

    void testRequireFileSources() {
        // Prepare the test environment.
        String document = u8"[main]\nvalue: 123\n";
        // Create a custom access check.
        auto accessCheck = FileAccessCheck::create();
        accessCheck->enable(FileAccessCheck::OnlyFileSources);
        // Set up the parser
        Parser parser;
        parser.setAccessCheck(accessCheck);
        auto source = Source::fromString(document);
        try {
            doc = parser.parseOrThrow(source);
            REQUIRE(false);
        } catch (const Error &error) {
            REQUIRE_EQUAL(error.category(), ErrorCategory::Access);
        }
    }
};

