// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ParserTestHelper.hpp"


TESTED_TARGETS(Parser)
class ParserConvenienceTest final : public UNITTEST_SUBCLASS(ParserTestHelper) {
public:
    Parser parser;
    std::unique_ptr<Error> error;

    void tearDown() override {
        cleanUpTestFileDirectory();
        error = {};
        doc = {};
    }

    void verifyTextSourceIdentifier(const SourceIdentifierPtr &identifier) {
        REQUIRE(identifier != nullptr);
        REQUIRE_EQUAL(identifier->name(), String{u8"text"});
        REQUIRE(identifier->path().empty());
    }

    void verifyFileSourceIdentifier(
        const SourceIdentifierPtr &identifier,
        const std::filesystem::path &path) {

        const auto expectedPath = String{std::filesystem::absolute(path).lexically_normal().string()};
        REQUIRE(identifier != nullptr);
        REQUIRE_EQUAL(identifier->name(), String{u8"file"});
        REQUIRE_EQUAL(identifier->path(), expectedPath);
    }

    void requireTextSourceAfterError() {
        REQUIRE(doc == nullptr);
        error = std::make_unique<Error>(parser.lastError());
        verifyTextSourceIdentifier(error->location().sourceIdentifier());
    }

    template<typename Fn>
    void requireFileSourceAfterError(const Fn &fn) {
        try {
            fn();
            REQUIRE(false);
        } catch (const Error &error) {
            verifyTextSourceIdentifier(error.location().sourceIdentifier());
        }
    }

    void testParseTextUsesTextSourceAndParse() {
        const String textStr = "[";
        const std::string text = "[";
        const std::u8string u8text = u8"[";

        REQUIRE_NOTHROW(doc = parser.parseText(textStr));
        WITH_CONTEXT(requireTextSourceAfterError());

        REQUIRE_NOTHROW(doc = parser.parseText(text));
        WITH_CONTEXT(requireTextSourceAfterError());

        REQUIRE_NOTHROW(doc = parser.parseText(std::string_view{text}));
        WITH_CONTEXT(requireTextSourceAfterError());

        REQUIRE_NOTHROW(doc = parser.parseText(u8text));
        WITH_CONTEXT(requireTextSourceAfterError());

        REQUIRE_NOTHROW(doc = parser.parseText(std::u8string_view{u8text}));
        WITH_CONTEXT(requireTextSourceAfterError());

        REQUIRE_NOTHROW(doc = parser.parseText("["));
        WITH_CONTEXT(requireTextSourceAfterError());

        REQUIRE_NOTHROW(doc = parser.parseText(u8"["));
        WITH_CONTEXT(requireTextSourceAfterError());
    }

    void testParseTextOrThrowUsesTextSource() {
        const String textStr = "[";
        const std::string text = "[";
        const std::u8string u8text = u8"[";

        WITH_CONTEXT(requireFileSourceAfterError([&]() -> void {
            doc = parser.parseTextOrThrow(textStr);
        }));
        WITH_CONTEXT(requireFileSourceAfterError([&]() -> void {
            doc = parser.parseTextOrThrow(text);
        }));
        WITH_CONTEXT(requireFileSourceAfterError([&]() -> void {
            doc = parser.parseTextOrThrow(std::string_view{text});
        }));
        WITH_CONTEXT(requireFileSourceAfterError([&]() -> void {
            doc = parser.parseTextOrThrow(u8text);
        }));
        WITH_CONTEXT(requireFileSourceAfterError([&]() -> void {
            doc = parser.parseTextOrThrow(std::u8string_view{u8text});
        }));
        WITH_CONTEXT(requireFileSourceAfterError([&]() -> void {
            doc = parser.parseTextOrThrow("[");
        }));
        WITH_CONTEXT(requireFileSourceAfterError([&]() -> void {
            doc = parser.parseTextOrThrow(u8"[");
        }));
    }

    void testParseFileUsesFileSourceAndParse() {
        const auto filePath = createTestFile("config/invalid.elcl", u8"[");
        REQUIRE_NOTHROW(doc = parser.parseFile(filePath));
        REQUIRE(doc == nullptr);
        error = std::make_unique<Error>(parser.lastError());
        verifyFileSourceIdentifier(error->location().sourceIdentifier(), filePath);
    }

    void testParseFileUsesFileSourceAndParseWithString() {
        const auto filePath = createTestFile("config/invalid.elcl", u8"[");
        const auto pathText = String{filePath.string()};
        REQUIRE_NOTHROW(doc = parser.parseFile(pathText));
        REQUIRE(doc == nullptr);
        error = std::make_unique<Error>(parser.lastError());
        verifyFileSourceIdentifier(error->location().sourceIdentifier(), filePath);
    }

    void testParseFileOrThrowUsesFileSource() {
        const auto filePath = createTestFile("config/invalid.elcl", u8"[");
        try {
            doc = parser.parseFileOrThrow(filePath);
            REQUIRE(false);
        } catch (const Error &error) {
            verifyFileSourceIdentifier(error.location().sourceIdentifier(), filePath);
        }
    }

    void testParseFileOrThrowUsesFileSourceWithString() {
        const auto filePath = createTestFile("config/invalid.elcl", u8"[");
        const auto pathText = String{filePath.string()};
        try {
            doc = parser.parseFileOrThrow(pathText);
            REQUIRE(false);
        } catch (const Error &error) {
            verifyFileSourceIdentifier(error.location().sourceIdentifier(), filePath);
        }
    }
};
