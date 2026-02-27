// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/lexer/LexerToken.hpp>
#include <erbsland/conf/Bytes.hpp>
#include <erbsland/conf/Date.hpp>
#include <erbsland/conf/DateTime.hpp>
#include <erbsland/conf/Position.hpp>
#include <erbsland/conf/Time.hpp>
#include <erbsland/conf/TimeDelta.hpp>
#include <erbsland/conf/TimeOffset.hpp>
#include <erbsland/conf/TimeUnit.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;
using impl::NoContent;
using impl::LexerToken;
using impl::TokenType;


TESTED_TARGETS(LexerToken)
class LexerTokenTest final : public el::UnitTest {
public:
    template<typename ValueType>
    void verifyToken(TokenType type, const String &rawText, const ValueType &expectedValue) {
        const Position begin{1, 1};
        const Position end{1, 1 + static_cast<int>(rawText.size())};
        LexerToken token{type, begin, end, String{rawText}, expectedValue};
        REQUIRE(token.type() == type);
        REQUIRE(token.begin() == begin);
        REQUIRE(token.end() == end);
        REQUIRE(token.rawText() == rawText);
        REQUIRE(std::holds_alternative<ValueType>(token.content()));
        REQUIRE(std::get<ValueType>(token.content()) == expectedValue);
    }

    void testNoContentToken() {
        const Position begin{3, 1};
        const Position end{3, 2};
        LexerToken token{TokenType::LineBreak, begin, end, String{u8"\n"}, NoContent{}};
        REQUIRE(token.type() == TokenType::LineBreak);
        REQUIRE(token.begin() == begin);
        REQUIRE(token.end() == end);
        REQUIRE(token.rawText() == u8"\n");
        REQUIRE(std::holds_alternative<NoContent>(token.content()));
    }

    void testValueTokens() {
        verifyToken<Integer>(TokenType::Integer, u8"42", Integer{42});
        verifyToken<bool>(TokenType::Boolean, u8"true", true);
        verifyToken<Float>(TokenType::Float, u8"3.14", 3.14);
        verifyToken<String>(TokenType::Text, u8"hello", String{u8"hello"});
        verifyToken<Date>(TokenType::Date, u8"2024-01-02", Date{2024, 1, 2});
        Time time{12, 34, 56, 0, TimeOffset::utc()};
        verifyToken<Time>(TokenType::Time, u8"12:34:56z", time);
        DateTime dt{Date{2024, 1, 2}, time};
        verifyToken<DateTime>(TokenType::DateTime, u8"2024-01-02 12:34:56z", dt);
        Bytes bytes = Bytes::from({uint8_t{0x01}, uint8_t{0x02}});
        verifyToken<Bytes>(TokenType::Bytes, u8"<0102>", bytes);
        TimeDelta delta(TimeUnit::Seconds, 5);
        verifyToken<TimeDelta>(TokenType::TimeDelta, u8"5s", delta);
    }
};

