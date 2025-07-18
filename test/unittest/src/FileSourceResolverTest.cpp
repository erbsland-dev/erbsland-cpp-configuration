// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/FileSourceResolver.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;


TESTED_TARGETS(FileSourceResolver)
class FileSourceResolverTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void tearDown() override {
        cleanUpTestFileDirectory();
    }

    using FileList = std::vector<std::string>;
    using ExpectedSourceList = std::vector<String>;

    SourceListPtr sources;
    SourceIdentifierPtr documentSourceIdentifier;
    std::vector<std::pair<String, String>> actualSourceList;

    auto createTestFile(const std::string &relativePath) -> std::filesystem::path {
        const auto basePath = useTestFileDirectory();
        auto filePath = basePath / relativePath;
        std::filesystem::create_directories(filePath.parent_path());
        std::ofstream stream(filePath);
        stream << "# Erbsland Configuration Language - Test File\n";
        stream << "[main]\n";
        stream << "value = 123\n";
        stream.close();
        return filePath;
    }

    auto createSourceIdentifier(const std::filesystem::path &path) {
        auto canonicalPath = canonical(path);
        documentSourceIdentifier = SourceIdentifier::createForFile(canonicalPath.u8string());
    }

    void setupFileList(const FileList &fileList) {
        documentSourceIdentifier = {};
        for (const auto &path : fileList) {
            auto filePath = createTestFile(path);
            if (documentSourceIdentifier == nullptr) {
                createSourceIdentifier(filePath);
            }
        }
        REQUIRE(documentSourceIdentifier != nullptr);
    }

    void expectSuccess(
        const String &includeText,
        const ExpectedSourceList &expected) {

        auto resolver = FileSourceResolver::create();
        REQUIRE(resolver != nullptr);
        const SourceResolverContext context{
            .includeText = includeText,
            .sourceIdentifier = documentSourceIdentifier
        };
        const auto basePath = useTestFileDirectory();
        actualSourceList = {};
        runWithContext(SOURCE_LOCATION(), [&]() {
            REQUIRE_NOTHROW(sources = resolver->resolve(context));
            for (size_t i = 0; i < sources->size(); ++i) {
                auto sourcePathStr = sources->at(i)->path();
                auto sourcePath = std::filesystem::path(sourcePathStr.toCharString());
                auto relSourcePath = relative(sourcePath, basePath);
                actualSourceList.emplace_back(relSourcePath.string(), sourcePathStr);
            }
            REQUIRE_EQUAL(sources->size(), expected.size());
            for (size_t i = 0; i < sources->size(); ++i) {
                auto sourcePathStr = sources->at(i)->path();
                REQUIRE_FALSE(sourcePathStr.empty());
                auto sourcePath = std::filesystem::path(sourcePathStr.toCharString());
                REQUIRE(sourcePath.is_absolute());
                auto relSourcePath = relative(sourcePath, basePath);
                REQUIRE_FALSE(relSourcePath.empty());
                REQUIRE_EQUAL(relSourcePath.string(), expected[i].toCharString());
            }
        }, [&]() -> std::string {
            std::string result;
            result += std::format("Include: {}\n", context.includeText.toCharString());
            result += "Expected Sources:\n";
            int index = 0;
            for (const auto &source : expected) {
                result += std::format("  {}: {}\n", index++, source.toCharString());
            }
            result += "Actual Sources:\n";
            if (actualSourceList.empty()) {
                result += "  <EMPTY>\n";
            } else {
                index = 0;
                for (const auto &[relPath, absPath] : actualSourceList) {
                    auto path = relPath.empty() ? absPath : relPath;
                    result += std::format("  {}: {}\n", index++, path);
                }
            }
            return result;
        });
    }

    void expectSuccessVariants(const String &includeText, const ExpectedSourceList &expected) {
        std::vector<String> variants;
        variants.emplace_back(includeText);
        variants.emplace_back(String{u8"file:"} + includeText);
        variants.emplace_back(String{useTestFileDirectory().u8string()} + u8"/config/" + includeText);
        variants.emplace_back(String{u8"file:"} + useTestFileDirectory().u8string() + u8"/config/" + includeText);
        for (const auto &variant : variants) {
            WITH_CONTEXT(expectSuccess(variant, expected));
        }
    }

    void expectSuccessAbs(
        const String &includeText,
        const ExpectedSourceList &expected) {

        String absIncludeText;
        if (includeText.starts_with(u8"file:")) {
            absIncludeText.append(u8"file:");
        }
        absIncludeText.append(useTestFileDirectory().u8string());
        absIncludeText.append(u8"/");
        if (includeText.starts_with(u8"file:")) {
            absIncludeText.append(includeText.substr(5));
        } else {
            absIncludeText.append(includeText);
        }
        expectSuccess(absIncludeText, expected);
    }

    void expectFailure(const String &includeText, ErrorCategory expectedErrorCategory = ErrorCategory::Syntax) {
        auto resolver = FileSourceResolver::create();
        REQUIRE(resolver != nullptr);
        const SourceResolverContext context{
            .includeText = includeText,
            .sourceIdentifier = documentSourceIdentifier
        };
        bool caughtError = false;
        runWithContext(SOURCE_LOCATION(), [&]() {
            try {
                sources = resolver->resolve(context);
                REQUIRE(false);
            } catch (const Error &error) {
                caughtError = true;
                REQUIRE_EQUAL(error.category(), expectedErrorCategory);
            }
        }, [&]() -> std::string {
            std::string result;
            if (!caughtError) {
                result += "Expected error not thrown.\n";
            }
            result += std::format("Include: {}\n", context.includeText.toCharString());
            result += "Sources:\n";
            if (sources == nullptr) {
                result += "  <NULL>\n";
            } else {
                int index = 0;
                for (const auto &source : *sources) {
                    result += std::format("  {}: {}\n", index++, source->path().toCharString());
                }
            }
            return result;
        });
    }

    void testIncorrectInput() {
        auto resolver = FileSourceResolver::create();
        REQUIRE(resolver != nullptr);
        REQUIRE_THROWS(resolver->resolve({}));
        REQUIRE_THROWS(resolver->resolve({.includeText = "test.elcl", .sourceIdentifier = {}}));
        REQUIRE_THROWS(resolver->resolve({.includeText = "test.elcl", .sourceIdentifier = SourceIdentifier::createForText()}));
        REQUIRE_THROWS(resolver->resolve({.includeText = "test.elcl", .sourceIdentifier = SourceIdentifier::createForFile("relative.elcl")}));
        REQUIRE_THROWS(resolver->resolve({.includeText = "test.elcl", .sourceIdentifier = SourceIdentifier::createForFile("/")}));
        REQUIRE_THROWS(resolver->resolve({.includeText = "test.elcl", .sourceIdentifier = SourceIdentifier::createForFile("/invalid/path/a/b/c/d/e/relative.elcl")}));
        REQUIRE_THROWS(resolver->resolve({.includeText = "test.elcl", .sourceIdentifier = SourceIdentifier::createForFile("/document.elcl")}));
        const auto fileList = FileList{
            "config/document.elcl",
        };
        auto doubleFilePath = useTestFileDirectory() / "config/document.elcl/document.elcl";
        REQUIRE_THROWS(resolver->resolve({.includeText = "test.elcl", .sourceIdentifier = SourceIdentifier::createForFile(doubleFilePath.u8string())}));
        setupFileList(fileList);
    }

    void testOneAbsolutePath() {
        // before starting with automated tests, do a manual test to make sure the test methods work as expected.
        auto documentPath = createTestFile("config/document.elcl");
        REQUIRE(!documentPath.empty());
        REQUIRE(documentPath.is_absolute());
        REQUIRE(is_regular_file(documentPath));
        createSourceIdentifier(documentPath);
        REQUIRE(documentSourceIdentifier != nullptr);
        REQUIRE(documentSourceIdentifier->name() == u8"file");
        const auto includedFile = createTestFile("config/IncludedFile.elcl");
        REQUIRE(is_regular_file(includedFile));
        const auto resolver = FileSourceResolver::create();
        REQUIRE(resolver != nullptr);
        const auto basePath = String{useTestFileDirectory().u8string()};
        REQUIRE(!basePath.empty());
        const SourceResolverContext context{
            .includeText = basePath + u8"/config/IncludedFile.elcl",
            .sourceIdentifier = documentSourceIdentifier,
        };
        SourceListPtr sourceList;
        REQUIRE_NOTHROW(sourceList = resolver->resolve(context));
        REQUIRE(sourceList != nullptr);
        REQUIRE_EQUAL(sourceList->size(), 1);
        auto source = sourceList->at(0);
        REQUIRE(source != nullptr);
        auto pathFromSourceList = source->path();
        auto actualPathOfInclude = String{canonical(includedFile).u8string()};
        REQUIRE_EQUAL(actualPathOfInclude, pathFromSourceList);
    }

    void testMaximumWildcards() {
        // manually test what happens when using the pattern "**/*"
        auto documentPath = createTestFile("config/document.elcl");
        REQUIRE(!documentPath.empty());
        REQUIRE(documentPath.is_absolute());
        REQUIRE(is_regular_file(documentPath));
        createSourceIdentifier(documentPath);
        REQUIRE(documentSourceIdentifier != nullptr);
        REQUIRE(documentSourceIdentifier->name() == u8"file");
        createTestFile("config/file1.elcl");
        createTestFile("config/file2.elcl");
        createTestFile("config/file3.elcl");
        const auto resolver = FileSourceResolver::create();
        REQUIRE(resolver != nullptr);
        const SourceResolverContext context{
            .includeText = u8"**/*",
            .sourceIdentifier = documentSourceIdentifier,
        };
        SourceListPtr sourceList;
        REQUIRE_NOTHROW(sourceList = resolver->resolve(context));
        REQUIRE(sourceList != nullptr);
        REQUIRE_EQUAL(sourceList->size(), 4);
    }

    void testPlainPaths() {
        const auto fileList = FileList{
            "config/MainDocument.elcl",
            "config/SameDir.elcl",
            "config/SubDir/SubDirDocument.elcl",
            "ParentDocument.elcl",
            "other/OtherDocument.elcl"
        };
        setupFileList(fileList);
        // plain and simple
        WITH_CONTEXT(expectSuccessVariants(u8"SameDir.elcl",{u8"config/SameDir.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"SubDir/SubDirDocument.elcl",{u8"config/SubDir/SubDirDocument.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"../ParentDocument.elcl", {u8"ParentDocument.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"../other/OtherDocument.elcl", {u8"other/OtherDocument.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"..//other//////OtherDocument.elcl", {u8"other/OtherDocument.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"../////////other///OtherDocument.elcl", {u8"other/OtherDocument.elcl"}));
        // normalization required
        WITH_CONTEXT(expectSuccessVariants(u8"./SameDir.elcl",{u8"config/SameDir.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8".//SameDir.elcl",{u8"config/SameDir.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8".\\SameDir.elcl",{u8"config/SameDir.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"./././SameDir.elcl",{u8"config/SameDir.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8".//////./////.///SameDir.elcl",{u8"config/SameDir.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"SubDir/../SameDir.elcl",{u8"config/SameDir.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"SubDir\\..\\.\\SameDir.elcl",{u8"config/SameDir.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"./SubDir/../../config/SubDir/SubDirDocument.elcl",{u8"config/SubDir/SubDirDocument.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8"../other/../ParentDocument.elcl", {u8"ParentDocument.elcl"}));
        WITH_CONTEXT(expectSuccessVariants(u8".\\..\\other\\OtherDocument.elcl", {u8"other/OtherDocument.elcl"}));
    }

    void testFilenameWildcards() {
        const auto fileList = FileList{
            "config/MainDocument.elcl",
            "config/sub/a/doc001.elcl",
            "config/sub/a/doc002.elcl",
            "config/sub/b/doc003.elcl",
            "config/sub/b/doc004.elcl",
            "config/sub/doc005.elcl",
            "config/sub/doc006.elcl",
            "config/sub/conf007.elcl",
            "config/sub/conf008.txt",
            "config/doc009.elcl",
            "config/doc010.elcl",
            "config/doc011.elcl",
            "config/doc012.elcl",
            "doc013.elcl",
            "doc014.elcl",
            "config.txt",
        };
        setupFileList(fileList);
        WITH_CONTEXT(expectSuccess(u8"*",{
            u8"config/MainDocument.elcl",
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"*.elcl",{
            u8"config/MainDocument.elcl",
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"doc*",{
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"doc*.elcl",{
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"../*",{
            u8"config.txt",
            u8"doc013.elcl",
            u8"doc014.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"../*4.elcl",{
            u8"doc014.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"../doc*",{
            u8"doc013.elcl",
            u8"doc014.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"../doc*3.elcl",{
            u8"doc013.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"sub/*",{
            u8"config/sub/conf007.elcl",
            u8"config/sub/conf008.txt",
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"sub/*4.elcl",{}));
        WITH_CONTEXT(expectSuccess(u8"sub/*.elcl",{
            u8"config/sub/conf007.elcl",
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"sub/d*",{
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
        }));
        WITH_CONTEXT(expectSuccess(u8"sub/doc00*l",{
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
        }));
        // same with absolute paths
        WITH_CONTEXT(expectSuccessAbs(u8"config/*",{
            u8"config/MainDocument.elcl",
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/*.elcl",{
            u8"config/MainDocument.elcl",
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/doc*",{
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/doc*.elcl",{
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/../*",{
            u8"config.txt",
            u8"doc013.elcl",
            u8"doc014.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/../*4.elcl",{
            u8"doc014.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/../doc*",{
            u8"doc013.elcl",
            u8"doc014.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/../doc*3.elcl",{
            u8"doc013.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/sub/*",{
            u8"config/sub/conf007.elcl",
            u8"config/sub/conf008.txt",
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/sub/*4.elcl",{}));
        WITH_CONTEXT(expectSuccessAbs(u8"config/sub/*.elcl",{
            u8"config/sub/conf007.elcl",
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/sub/d*",{
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
        }));
        WITH_CONTEXT(expectSuccessAbs(u8"config/sub/doc00*l",{
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
        }));
    }

    void testDirectoryWildcards() {
        const auto fileList = FileList{
            "config/doc009.elcl",
            "config/doc010.elcl",
            "config/doc011.elcl",
            "config/doc012.elcl",
            "config/main.elcl",
            "config/sub/a/doc001.elcl",
            "config/sub/a/doc002.elcl",
            "config/sub/b/doc003.elcl",
            "config/sub/b/doc004.elcl",
            "config/sub/conf007.elcl",
            "config/sub/conf008.txt",
            "config/sub/doc005.elcl",
            "config/sub/doc006.elcl",
            "config.txt",
            "doc013.elcl",
            "doc014.elcl",
        };
        setupFileList(fileList);
        WITH_CONTEXT(expectSuccessVariants(u8"**/*",{
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
            u8"config/main.elcl",
            u8"config/sub/conf007.elcl",
            u8"config/sub/conf008.txt",
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
            u8"config/sub/a/doc001.elcl",
            u8"config/sub/a/doc002.elcl",
            u8"config/sub/b/doc003.elcl",
            u8"config/sub/b/doc004.elcl",
        }));
        WITH_CONTEXT(expectSuccessVariants(u8"../**/*",{
            u8"config.txt",
            u8"doc013.elcl",
            u8"doc014.elcl",
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
            u8"config/main.elcl",
            u8"config/sub/conf007.elcl",
            u8"config/sub/conf008.txt",
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
            u8"config/sub/a/doc001.elcl",
            u8"config/sub/a/doc002.elcl",
            u8"config/sub/b/doc003.elcl",
            u8"config/sub/b/doc004.elcl",
        }));
        WITH_CONTEXT(expectSuccessVariants(u8"sub/**/*",{
            u8"config/sub/conf007.elcl",
            u8"config/sub/conf008.txt",
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
            u8"config/sub/a/doc001.elcl",
            u8"config/sub/a/doc002.elcl",
            u8"config/sub/b/doc003.elcl",
            u8"config/sub/b/doc004.elcl",
        }));
        WITH_CONTEXT(expectSuccessVariants(u8"**/doc006.elcl",{
            u8"config/sub/doc006.elcl",
        }));
        WITH_CONTEXT(expectSuccessVariants(u8"**/doc999.elcl",{
        }));
        WITH_CONTEXT(expectSuccessVariants(u8"../**/doc004.elcl",{
            u8"config/sub/b/doc004.elcl",
        }));
        WITH_CONTEXT(expectSuccessVariants(u8"sub/**/conf008.txt",{
            u8"config/sub/conf008.txt",
        }));
        WITH_CONTEXT(expectSuccessVariants(u8"**/*.txt",{
            u8"config/sub/conf008.txt",
        }));
        WITH_CONTEXT(expectSuccessVariants(u8"../**/doc*",{
            u8"doc013.elcl",
            u8"doc014.elcl",
            u8"config/doc009.elcl",
            u8"config/doc010.elcl",
            u8"config/doc011.elcl",
            u8"config/doc012.elcl",
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
            u8"config/sub/a/doc001.elcl",
            u8"config/sub/a/doc002.elcl",
            u8"config/sub/b/doc003.elcl",
            u8"config/sub/b/doc004.elcl",
        }));
        WITH_CONTEXT(expectSuccessVariants(u8"sub/**/*.elcl",{
            u8"config/sub/conf007.elcl",
            u8"config/sub/doc005.elcl",
            u8"config/sub/doc006.elcl",
            u8"config/sub/a/doc001.elcl",
            u8"config/sub/a/doc002.elcl",
            u8"config/sub/b/doc003.elcl",
            u8"config/sub/b/doc004.elcl",
        }));
    }

    void testErrors() {
        const auto fileList = FileList{
            "config/MainDocument.elcl",
            "config/SameDir.elcl",
            "config/SubDir/SubDirDocument.elcl",
            "config/SubDir/A/a.elcl",
            "config/SubDir/B/b.elcl",
            "ParentDocument.elcl",
            "other/OtherDocument.elcl"
        };
        setupFileList(fileList);
        // Test general errors.
        WITH_CONTEXT(expectFailure(u8""));
        WITH_CONTEXT(expectFailure(u8"file:"));
        WITH_CONTEXT(expectFailure(u8"."));
        WITH_CONTEXT(expectFailure(u8".."));
        WITH_CONTEXT(expectFailure(u8"../../../../../../../../../config/SameDir.elcl"));
        WITH_CONTEXT(expectFailure(u8"config/SameDir.elcl"));
        WITH_CONTEXT(expectFailure(u8"SameDir.elcl/"));
        WITH_CONTEXT(expectFailure(u8"../SameDir.elcl/"));
        WITH_CONTEXT(expectFailure(u8"SubDir/"));
        WITH_CONTEXT(expectFailure(u8"../SubDir/"));
        // Test invalid wildcards
        WITH_CONTEXT(expectFailure(u8"Sub*Dir/SubDirDocument.elcl"));
        WITH_CONTEXT(expectFailure(u8"Sub**Dir/SubDirDocument.elcl"));
        WITH_CONTEXT(expectFailure(u8"Sub***Dir/SubDirDocument.elcl"));
        WITH_CONTEXT(expectFailure(u8"**/**/SubDirDocument.elcl"));
        WITH_CONTEXT(expectFailure(u8"**/**/SubDirDocument.elcl"));
        WITH_CONTEXT(expectFailure(u8"SubDir/**/**/SubDirDocument.elcl"));
        WITH_CONTEXT(expectFailure(u8"Sub**/SubDirDocument.elcl"));
        WITH_CONTEXT(expectFailure(u8"**Dir/SubDirDocument.elcl"));
        WITH_CONTEXT(expectFailure(u8"SubDir/S*D*Document.elcl"));
        WITH_CONTEXT(expectFailure(u8"SubDir/S**Document.elcl"));
        WITH_CONTEXT(expectFailure(u8"SubDir/S***Document.elcl"));
        // Test invalid UNC paths
        WITH_CONTEXT(expectFailure(u8"//"));
        WITH_CONTEXT(expectFailure(u8"///"));
        WITH_CONTEXT(expectFailure(u8"///config.elcl"));
        WITH_CONTEXT(expectFailure(u8"//a"));
        WITH_CONTEXT(expectFailure(u8"//abc"));
        WITH_CONTEXT(expectFailure(u8"//abc/"));
        WITH_CONTEXT(expectFailure(u8"//local*host/config.elcl"));
        WITH_CONTEXT(expectFailure(u8"//local?host/config.elcl"));
        WITH_CONTEXT(expectFailure(u8"//local|host/config.elcl"));
        WITH_CONTEXT(expectFailure(u8"//local\"host/config.elcl"));
        WITH_CONTEXT(expectFailure(u8"//local<host/config.elcl"));
        WITH_CONTEXT(expectFailure(u8"//local😀host/config.elcl"));
        WITH_CONTEXT(expectFailure(u8"\\\\"));
        WITH_CONTEXT(expectFailure(u8"\\\\\\"));
        WITH_CONTEXT(expectFailure(u8"\\\\\\config.elcl"));
        WITH_CONTEXT(expectFailure(u8"\\\\a"));
        WITH_CONTEXT(expectFailure(u8"\\\\abc"));
        WITH_CONTEXT(expectFailure(u8"\\\\abc\\"));
        WITH_CONTEXT(expectFailure(u8"\\\\local*host\\config.elcl"));
        WITH_CONTEXT(expectFailure(u8"\\\\local?host\\config.elcl"));
        WITH_CONTEXT(expectFailure(u8"\\\\local|host\\config.elcl"));
        WITH_CONTEXT(expectFailure(u8"\\\\local\"host\\config.elcl"));
        WITH_CONTEXT(expectFailure(u8"\\\\local<host\\config.elcl"));
        WITH_CONTEXT(expectFailure(u8"\\\\local😀host\\config.elcl"));
    }
};

