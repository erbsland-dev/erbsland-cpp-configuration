// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../TestHelper.hpp"

#include <erbsland/conf/Integer.hpp>
#include <erbsland/conf/Error.hpp>
#include <erbsland/conf/Source.hpp>

#include <erbsland/conf/impl/source/FileSource.hpp>
#include <erbsland/conf/impl/Lexer.hpp>

#include <sstream>


using namespace el::conf;
using impl::Lexer;
using impl::LexerPtr;
using impl::LexerToken;
using impl::FileSource;
using impl::CharStreamPtr;
using impl::CharStream;
using impl::TokenType;
using impl::TokenGenerator;


class LexerTestHelper : public TestHelper {
public:
    std::variant<String, std::filesystem::path> testContentSource;
    SourcePtr source;
    CharStreamPtr decoder;
    LexerPtr lexer;
    LexerToken token;
    TokenGenerator tokenGenerator;
    TokenGenerator::iterator tokenIterator;
    TokenGenerator::iterator tokenIteratorEnd;

    auto additionalErrorMessages() -> std::string override {
        try {
            std::ostringstream oss;
            oss << "Tested content:\n" << testContents.toCharString() << "\n";
            if (lexer != nullptr) {
                oss << "Lexer state:\n";
                oss << internalView(*lexer)->toString(2).toCharString() << "\n";
            }
            oss << "Last lexer token:\n";
            oss << internalView(token)->toString(2).toCharString() << "\n";
            if (std::holds_alternative<std::filesystem::path>(testContentSource)) {
                oss << "Lexing last test file again:\n";
                source = FileSource::fromFile(std::get<std::filesystem::path>(testContentSource));
            } else {
                oss << "Lexing last test string again:\n";
                source = FileSource::fromString(std::get<String>(testContentSource));
            }
            REQUIRE_NOTHROW(source->open());
            decoder = CharStream::create(source);
            lexer = Lexer::create(decoder);
            try {
                auto index = 0;
                for (auto token : lexer->tokens()) {
                    oss << index << ":\n";
                    oss << internalView(token)->toString(2).toCharString() << "\n";
                    index += 1;
                }
            } catch (Error &e) {
                oss << e.category().toText().toCharString() << " exception: " << e.message().toCharString() << "\n";
            }
            return oss.str();
        } catch (...) {
            return "Unexpected exception.";
        }
    }

    void tearDown() override {
        // free all resources here to avoid side effects from deconstruction in the next test.
        tokenIterator = {};
        tokenIteratorEnd = {};
        tokenGenerator = {};
        lexer.reset();
        decoder.reset();
        source.reset();
        testContentSource = {};
        cleanUpTestFileDirectory();
    }

    void setupLexer(const String &content) {
        testContentSource = content;
        source = createTestMemorySource(content);
        REQUIRE(source != nullptr);
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        lexer = Lexer::create(decoder);
        REQUIRE(lexer != nullptr);
    }

    void setupLexer(const Bytes &content) {
        auto path = createTestFile(content);
        testContentSource = path;
        source = Source::fromFile(path);
        REQUIRE(source != nullptr);
        REQUIRE_NOTHROW(source->open());
        decoder = CharStream::create(source);
        REQUIRE(decoder != nullptr);
        lexer = Lexer::create(decoder);
        REQUIRE(lexer != nullptr);
    }


    template<typename T>
    void setupTokenIterator(const T &content) {
        setupLexer(content);
        tokenGenerator = lexer->tokens();
        tokenIterator = tokenGenerator.begin();
        tokenIteratorEnd = tokenGenerator.end();
    }

    void setupTokenIteratorFast(const String &content) {
        testContentSource = {};
        testContents = {};
        source = Source::fromString(content);
        source->open();
        decoder = CharStream::create(source);
        lexer = Lexer::create(decoder);
        tokenGenerator = lexer->tokens();
        tokenIterator = tokenGenerator.begin();
        tokenIteratorEnd = tokenGenerator.end();
    }

    void requireNextToken(
        TokenType expectedTokenType,
        const std::optional<String>& expectedRaw = std::nullopt,
        std::optional<Position> expectedBegin = std::nullopt,
        std::optional<Position> expectedEnd = std::nullopt) {

        REQUIRE(tokenIterator != tokenIteratorEnd);
        REQUIRE_NOTHROW(token = *tokenIterator);
        REQUIRE_EQUAL(token.type(), expectedTokenType);
        if (expectedRaw.has_value()) {
            REQUIRE_EQUAL(token.rawText(), expectedRaw.value());
        } else {
            REQUIRE_FALSE(token.rawText().empty())
        }
        if (expectedBegin.has_value()) {
            REQUIRE(token.begin() == expectedBegin);
        } else {
            REQUIRE_FALSE(token.begin().isUndefined());
        }
        if (expectedEnd.has_value()) {
            REQUIRE(token.end() == expectedEnd);
        } else {
            REQUIRE_FALSE(token.end().isUndefined());
        }
        REQUIRE_NOTHROW(++tokenIterator);
    }

    template<typename T>
    void requireNextValueToken(
        const TokenType expectedTokenType,
        const T &expectedValue,
        const std::optional<String>& expectedRaw) {

        REQUIRE(tokenIterator != tokenIteratorEnd);
        token = *tokenIterator;
        REQUIRE_EQUAL(token.type(), expectedTokenType);
        REQUIRE(std::holds_alternative<T>(token.content()));
        const auto actualValue = std::get<T>(token.content());
        REQUIRE_EQUAL(actualValue, expectedValue);
        if (expectedRaw.has_value()) {
            REQUIRE_EQUAL(token.rawText(), expectedRaw.value());
        } else {
            REQUIRE_FALSE(token.rawText().empty())
        }
        ++tokenIterator;
    }

    void requireNextStringToken(
        const TokenType expectedTokenType,
        const String &expectedString,
        const std::optional<String>& expectedRaw = std::nullopt) {

        requireNextValueToken<String>(expectedTokenType, expectedString, expectedRaw);
    }

    void requireNextIntegerToken(
        const TokenType expectedTokenType,
        const Integer expectedValue,
        const std::optional<String>& expectedRaw = std::nullopt) {

        requireNextValueToken<Integer>(expectedTokenType, expectedValue, expectedRaw);
    }

    void requireNextBytesToken(
    const TokenType expectedTokenType,
    const Bytes &expectedValue,
    const std::optional<String>& expectedRaw = std::nullopt) {

        requireNextValueToken<Bytes>(expectedTokenType, expectedValue, expectedRaw);
    }

    void requireError(ErrorCategory expectedErrorCategory) {
        try {
            token = *tokenIterator;
            REQUIRE(false);
        } catch (const Error &e) {
            REQUIRE(e.category() == expectedErrorCategory);
        }
    }

    void requireError(std::initializer_list<ErrorCategory> expectedErrorCategories) {
        try {
            token = *tokenIterator;
            REQUIRE(false);
        } catch (const Error &e) {
            const auto it = std::ranges::find(expectedErrorCategories, e.category());
            REQUIRE(it != expectedErrorCategories.end());
        }
    }

    void requireEndOfData() {
        REQUIRE(tokenIterator != tokenIteratorEnd);
        token = *tokenIterator;
        REQUIRE(token.type() == TokenType::EndOfData);
        REQUIRE(token.rawText().empty());
        REQUIRE(token.begin().isUndefined());
        REQUIRE(token.end().isUndefined());
        ++tokenIterator;
        REQUIRE(tokenIterator == tokenIteratorEnd);
    }
};


template<>
inline void LexerTestHelper::requireNextValueToken<Float>(
    const TokenType expectedTokenType,
    const Float &expectedValue,
    const std::optional<String>& expectedRaw) {

    REQUIRE(tokenIterator != tokenIteratorEnd);
    token = *tokenIterator;
    REQUIRE_EQUAL(token.type(), expectedTokenType);
    REQUIRE(std::holds_alternative<double>(token.content()));
    const auto actualValue = std::get<double>(token.content());
    if (std::isnan(expectedValue)) {
        REQUIRE(std::isnan(actualValue));
    } else if (std::isinf(expectedValue)) {
        REQUIRE(std::isinf(actualValue));
    } else {
        REQUIRE(std::abs(actualValue - expectedValue) < std::numeric_limits<Float>::epsilon());
    }
    if (expectedRaw.has_value()) {
        REQUIRE_EQUAL(token.rawText(), expectedRaw.value());
    } else {
        REQUIRE_FALSE(token.rawText().empty())
    }
    ++tokenIterator;
}
