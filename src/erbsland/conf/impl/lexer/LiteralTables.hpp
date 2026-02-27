// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "LexerToken.hpp"
#include "TokenType.hpp"


namespace erbsland::conf::impl::lexer {


// @tested via Lexer tests


/// Tables with literal constants of the language.
///
struct LiteralTables {
    struct ByteCountSuffix {
        int64_t factor;
    };

    struct TimeDeltaSuffix {
        TimeUnit unit;
    };

    using SuffixInfo = std::variant<ByteCountSuffix, TimeDeltaSuffix>;

    struct IdentifierInfo {
        TokenType type;
        Content value;
    };

    using IdentifierMap = std::unordered_map<String, IdentifierInfo>;
    using IntegerSuffixMap = std::unordered_map<String, SuffixInfo>;

    inline static const IdentifierMap identifierMap = {
        {u8"true", IdentifierInfo{TokenType::Boolean, Content{true}}},
        {u8"yes", IdentifierInfo{TokenType::Boolean, Content{true}}},
        {u8"enabled", IdentifierInfo{TokenType::Boolean, Content{true}}},
        {u8"on", IdentifierInfo{TokenType::Boolean, Content{true}}},
        {u8"false", IdentifierInfo{TokenType::Boolean, Content{false}}},
        {u8"no", IdentifierInfo{TokenType::Boolean, Content{false}}},
        {u8"disabled", IdentifierInfo{TokenType::Boolean, Content{false}}},
        {u8"off", IdentifierInfo{TokenType::Boolean, Content{false}}},
    };

    inline static const IntegerSuffixMap integerSuffixMap = {
        {u8"kb", ByteCountSuffix{1000}},
        {u8"mb", ByteCountSuffix{1000000}},
        {u8"gb", ByteCountSuffix{1000000000}},
        {u8"tb", ByteCountSuffix{1000000000000}},
        {u8"pb", ByteCountSuffix{1000000000000000}},
        {u8"eb", ByteCountSuffix{1000000000000000000}},
        {u8"zb", ByteCountSuffix{-1}},
        {u8"yb", ByteCountSuffix{-1}},
        {u8"kib", ByteCountSuffix{1024}},
        {u8"mib", ByteCountSuffix{1048576}},
        {u8"gib", ByteCountSuffix{1073741824}},
        {u8"tib", ByteCountSuffix{1099511627776}},
        {u8"pib", ByteCountSuffix{1125899906842624}},
        {u8"eib", ByteCountSuffix{1152921504606846976}},
        {u8"zib", ByteCountSuffix{-1}},
        {u8"yib", ByteCountSuffix{-1}},
        {u8"ns", TimeDeltaSuffix{TimeUnit::Nanoseconds}},
        {u8"nanosecond", TimeDeltaSuffix{TimeUnit::Nanoseconds}},
        {u8"nanoseconds", TimeDeltaSuffix{TimeUnit::Nanoseconds}},
        {u8"us", TimeDeltaSuffix{TimeUnit::Microseconds}},
        {u8"µs", TimeDeltaSuffix{TimeUnit::Microseconds}},
        {u8"microsecond", TimeDeltaSuffix{TimeUnit::Microseconds}},
        {u8"microseconds", TimeDeltaSuffix{TimeUnit::Microseconds}},
        {u8"ms", TimeDeltaSuffix{TimeUnit::Milliseconds}},
        {u8"millisecond", TimeDeltaSuffix{TimeUnit::Milliseconds}},
        {u8"milliseconds", TimeDeltaSuffix{TimeUnit::Milliseconds}},
        {u8"s", TimeDeltaSuffix{TimeUnit::Seconds}},
        {u8"second", TimeDeltaSuffix{TimeUnit::Seconds}},
        {u8"seconds", TimeDeltaSuffix{TimeUnit::Seconds}},
        {u8"m", TimeDeltaSuffix{TimeUnit::Minutes}},
        {u8"minute", TimeDeltaSuffix{TimeUnit::Minutes}},
        {u8"minutes", TimeDeltaSuffix{TimeUnit::Minutes}},
        {u8"h", TimeDeltaSuffix{TimeUnit::Hours}},
        {u8"hour", TimeDeltaSuffix{TimeUnit::Hours}},
        {u8"hours", TimeDeltaSuffix{TimeUnit::Hours}},
        {u8"d", TimeDeltaSuffix{TimeUnit::Days}},
        {u8"day", TimeDeltaSuffix{TimeUnit::Days}},
        {u8"days", TimeDeltaSuffix{TimeUnit::Days}},
        {u8"w", TimeDeltaSuffix{TimeUnit::Weeks}},
        {u8"week", TimeDeltaSuffix{TimeUnit::Weeks}},
        {u8"weeks", TimeDeltaSuffix{TimeUnit::Weeks}},
        {u8"month", TimeDeltaSuffix{TimeUnit::Months}},
        {u8"months", TimeDeltaSuffix{TimeUnit::Months}},
        {u8"year", TimeDeltaSuffix{TimeUnit::Years}},
        {u8"years", TimeDeltaSuffix{TimeUnit::Years}},
    };
};


}

