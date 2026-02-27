// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/impl/char/Char.hpp>
#include <erbsland/conf/impl/lexer/TokenType.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <format>


using namespace erbsland::conf;
using impl::TokenType;
using impl::Char;


TESTED_TARGETS(TokenType)
class TokenTypeTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultAndConstructor() {
        TokenType typeDefault;
        REQUIRE(typeDefault == TokenType::Error);

        TokenType lineBreak{TokenType::LineBreak};
        REQUIRE(lineBreak == TokenType::LineBreak);
        REQUIRE(lineBreak != TokenType::Error);
    }

    void testMultiLineOpen() {
        REQUIRE(TokenType::fromMultiLineOpen(Char::DoubleQuote) == TokenType::MultiLineTextOpen);
        REQUIRE(TokenType::fromMultiLineOpen(Char::Backtick) == TokenType::MultiLineCodeOpen);
        REQUIRE(TokenType::fromMultiLineOpen(Char::Slash) == TokenType::MultiLineRegexOpen);
        REQUIRE(TokenType::fromMultiLineOpen(Char::LessThan) == TokenType::MultiLineBytesOpen);
        REQUIRE(TokenType::fromMultiLineOpen(U'?') == TokenType::EndOfData);
    }

    void testMultiLineClose() {
        REQUIRE(TokenType::fromMultiLineClose(Char::DoubleQuote) == TokenType::MultiLineTextClose);
        REQUIRE(TokenType::fromMultiLineClose(Char::Backtick) == TokenType::MultiLineCodeClose);
        REQUIRE(TokenType::fromMultiLineClose(Char::Slash) == TokenType::MultiLineRegexClose);
        REQUIRE(TokenType::fromMultiLineClose(Char::GreaterThan) == TokenType::MultiLineBytesClose);
        REQUIRE(TokenType::fromMultiLineClose(U'?') == TokenType::EndOfData);
    }

    void testFormatter() {
        REQUIRE(std::format("{}", TokenType{TokenType::Boolean}) == "Boolean");
        REQUIRE(std::format("{}", TokenType{TokenType::MultiLineCodeOpen}) == "MultiLineCodeOpen");
        REQUIRE(std::format("{}", TokenType{TokenType::Error}) == "Error");
    }
};

