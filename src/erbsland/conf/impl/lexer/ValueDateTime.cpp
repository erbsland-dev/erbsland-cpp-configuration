// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ValueDateTime.hpp"


#include "Number.hpp"

#include "../utilities/YieldMacros.hpp"


namespace erbsland::conf::impl::lexer {


auto scanTimeValue(TokenDecoder &decoder) -> std::optional<Time>;
auto scanDate(TokenDecoder &decoder) -> std::optional<Date>;


auto scanDateOrDateTime(TokenDecoder &decoder) -> std::optional<LexerToken> {
    if (decoder.character() != CharClass::DecimalDigit) {
        return std::nullopt;
    }
    auto dateTimeTransaction = Transaction{decoder};
    auto optDate = scanDate(decoder);
    if (!optDate) {
        return std::nullopt;
    }
    auto timeSeparatorTransaction = Transaction{decoder};
    if (decoder.character() == Char::Space || decoder.character() == CharClass::LetterT) {
        const bool hasLetterSeparator = decoder.character() == CharClass::LetterT;
        decoder.next();
        if (decoder.character() == CharClass::DecimalDigit) {
            if (auto optTime = scanTimeValue(decoder)) {
                timeSeparatorTransaction.commit();
                dateTimeTransaction.commit();
                return decoder.createToken(
                    TokenType::DateTime,
                    DateTime{std::move(optDate).value(), std::move(optTime).value()});
            }
        } else if (hasLetterSeparator) {
            decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a time value after a time separator.");
        }
    }
    timeSeparatorTransaction.rollback(); // only rollback the inner scope.
    dateTimeTransaction.commit(); // only commit the outer scope.
    return decoder.createToken(TokenType::Date, std::move(optDate).value());
}


auto scanTime(TokenDecoder &decoder) -> std::optional<LexerToken> {
    if (decoder.character() != CharClass::TimeStart) {
        return {};
    }
    auto transaction = Transaction{decoder};
    auto optTime = scanTimeValue(decoder);
    if (!optTime) {
        return {};
    }
    transaction.commit();
    return decoder.createToken(TokenType::Time, std::move(optTime).value());
}


/// Parse a time value.
///
/// @return If the value does not start with `XX:`, no value is returned instead of an error.
/// @throws Error If there is a syntax error after the initial `XX:`.
///
auto scanTimeValue(TokenDecoder &decoder) -> std::optional<Time> {
    if (decoder.character() == CharClass::LetterT) {
        decoder.next(); // Skip an optional T in front of the time.
        if (decoder.character() != CharClass::DecimalDigit) {
            return {}; // coverage: this case is already handled by the value-literal scan.
        }
    }
    int64_t hour = 0;
    int64_t minute = 0;
    int64_t second = 0;
    int64_t fraction = 0;
    auto offsetSign = Sign::Positive;
    int64_t offsetHour = -1; // local time
    int64_t offsetMinute = 0;
    parseNumber(decoder, NumberBase::Decimal, Sign::Positive, NumberSeparators::No, 2).assignTo(hour);
    if (hour < 0 || decoder.character() != Char::TimeSeparator) {
        return {};
    }
    decoder.next(); // From here, we expect to read a time and throw an error if the format does not match our expectations.
    if (hour > 23) {
        decoder.throwSyntaxError(u8"The hour in a time value must be in the range 00-23.");
    }
    if (decoder.character() != CharClass::DecimalDigit) {
        decoder.throwSyntaxOrUnexpectedEndError(u8"Expected the minute part after the colon for a time value.");
    }
    parseNumber(decoder, NumberBase::Decimal, Sign::Positive, NumberSeparators::No, 2).assignTo(minute);
    if (minute < 0) {
        decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a two digit minute part after the colon for a time value.");
    }
    if (minute > 59) {
        decoder.throwSyntaxError(u8"The minute in a time value must be in the range 00-59.");
    }
    // the second part is optional.
    if (decoder.character() == Char::TimeSeparator) {
        decoder.next();
        if (decoder.character() != CharClass::DecimalDigit) {
            decoder.throwSyntaxOrUnexpectedEndError(u8"Expected the second part after the second colon for a time value.");
        }
        parseNumber(decoder, NumberBase::Decimal, Sign::Positive, NumberSeparators::No, 2).assignTo(second);
        if (second < 0) {
            decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a two digit second part after the second colon for a time value.");
        }
        if (second > 59) {
            decoder.throwSyntaxError(u8"The second in a time value must be in the range 00-59.");
        }
        // optional second fraction.
        if (decoder.character() == Char::DecimalPoint) {
            decoder.next();
            if (decoder.character() != CharClass::DecimalDigit) {
                decoder.throwSyntaxOrUnexpectedEndError(u8"Expected the second fraction part after the decimal point.");
            }
            std::size_t digitCount = 0;
            parseNumber(decoder, NumberBase::Decimal, Sign::Positive, NumberSeparators::No).assignTo(fraction, digitCount);
            if (fraction < 0) {
                decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a fraction part after the decimal point.");
            }
            if (digitCount > 9) {
                decoder.throwSyntaxError(u8"The fraction part in a time must not exceed nine digits.");
            }
            constexpr std::size_t maxFractionDigits = 9;
            for (std::size_t i = 0; i < maxFractionDigits - digitCount; ++i) {
                fraction *= 10; // Shift the fraction to nanoseconds.
            }
        }
    }
    // may be followed by an offset.
    if (decoder.character() == CharClass::LetterZ) {
        decoder.next(); // consume the Z.
        offsetHour = 0;
    } else if (decoder.character() == CharClass::PlusOrMinus) {
        offsetSign = (decoder.character() == Char::Minus ? Sign::Negative : Sign::Positive);
        decoder.next();
        if (decoder.character() != CharClass::DecimalDigit) {
            decoder.throwSyntaxOrUnexpectedEndError(u8"Expected an offset hour.");
        }
        parseNumber(decoder, NumberBase::Decimal, Sign::Positive, NumberSeparators::No, 2).assignTo(offsetHour);
        if (offsetHour < 0) {
            decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a two digit offset hour.");
        }
        if (offsetHour > 23) {
            decoder.throwSyntaxError(u8"The offset hour must be in the range 00-23.");
        }
        if (decoder.character() == Char::TimeSeparator) {
            decoder.next();
            if (decoder.character() != CharClass::DecimalDigit) {
                decoder.throwSyntaxOrUnexpectedEndError(u8"Expected an offset minute.");
            }
            parseNumber(decoder, NumberBase::Decimal, Sign::Positive, NumberSeparators::No, 2).assignTo(offsetMinute);
            if (offsetMinute < 0) {
                decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a two digit offset minute.");
            }
            if (offsetMinute > 59) {
                decoder.throwSyntaxError(u8"The offset minute must be in the range 00-59.");
            }
        }
    }
    auto offset = TimeOffset{};
    if (offsetHour > -1) {
        offset = TimeOffset{
            offsetSign == Sign::Negative,
            static_cast<int>(offsetHour),
            static_cast<int>(offsetMinute),
            0
        };
    }
    return Time{
        static_cast<int>(hour),
        static_cast<int>(minute),
        static_cast<int>(second),
        static_cast<int>(fraction),
        offset
    };
}


/// Parse a date value.
///
/// @return If the value does not start with `XXXX-`, no value is returned instead of an error.
/// @throws Error If there is a syntax error after the initial `XXXX-`.
///
auto scanDate(TokenDecoder &decoder) -> std::optional<Date> {
    int64_t year = 0;
    int64_t month = 0;
    int64_t day = 0;
    parseNumber(decoder, NumberBase::Decimal, Sign::Positive, NumberSeparators::No, 4).assignTo(year);
    if (year < 0) {
        return {};
    }
    if (decoder.character() != Char::DateSeparator) {
        return {};
    }
    decoder.next();
    if (decoder.character() != CharClass::DecimalDigit) {
        decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a month part after the date separator.");
    }
    parseNumber(decoder, NumberBase::Decimal, Sign::Positive, NumberSeparators::No, 2).assignTo(month);
    if (month < 0) {
        decoder.throwSyntaxOrUnexpectedEndError(u8"Expected two digits for the month in a date.");
    }
    if (month < 1 || month > 12) {
        decoder.throwSyntaxError(u8"The month in a date value must be in the range 01-12.");
    }
    if (decoder.character() != Char::DateSeparator) {
        decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a date separator after the month.");
    }
    decoder.next();
    if (decoder.character() != CharClass::DecimalDigit) {
        decoder.throwSyntaxOrUnexpectedEndError(u8"Expected a day part after the date separator.");
    }
    parseNumber(decoder, NumberBase::Decimal, Sign::Positive, NumberSeparators::No, 2).assignTo(day);
    if (day < 0) {
        decoder.throwSyntaxOrUnexpectedEndError(u8"Expected two digits for the day in a date.");
    }
    if (day < 1 || day > 31) {
        decoder.throwSyntaxError(u8"The day in a date value must be in the range 01-31.");
    }
    if (!Date::isValid(static_cast<int>(year), static_cast<int>(month), static_cast<int>(day))) {
        decoder.throwSyntaxError(u8"This date does not exist.");
    }
    return Date{static_cast<int>(year), static_cast<int>(month), static_cast<int>(day)};
}


}



