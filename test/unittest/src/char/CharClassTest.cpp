// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/String.hpp>
#include <erbsland/conf/impl/char/Char.hpp>
#include <erbsland/conf/impl/char/CharClass.hpp>

#include <format>


using namespace el::conf;
using impl::Char;
using impl::CharClass;


TESTED_TARGETS(Char)
class CharClassTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testCharClasses() {
        struct Case { Char ch; CharClass cls; };
        const auto cases = std::vector<Case>{
            {Char{Char::Tab}, CharClass::Spacing},
            {Char{Char::NewLine}, CharClass::LineBreak},
            {Char{Char::At}, CharClass::NameStart},
            {Char{Char::LcA}, CharClass::Letter},
            {Char{Char::Digit9}, CharClass::LetterOrDigit},
            {Char{Char::Digit0}, CharClass::DecimalDigit},
            {Char{Char::UcF}, CharClass::HexDigit},
            {Char{Char::Colon}, CharClass::NameValueSeparator},
            {Char{Char::DoubleQuote}, CharClass::OpeningBracket},
            {Char{Char::Minus}, CharClass::SectionStart},
            {Char{Char::CommentStart}, CharClass::EndOfLineStart},
            {Char{Char::UcA}, CharClass::LetterA},
            {Char{Char::LcB}, CharClass::LetterB},
            {Char{Char::UcF}, CharClass::LetterF},
            {Char{Char::UcI}, CharClass::LetterI},
            {Char{Char::LcN}, CharClass::LetterN},
            {Char{Char::UcT}, CharClass::LetterT},
            {Char{Char::LcX}, CharClass::LetterX},
            {Char{Char::UcZ}, CharClass::LetterZ},
            {Char{Char::Plus}, CharClass::NumberStart},
            {Char{Char::LcT}, CharClass::TimeStart},
            {Char{Char::UcE}, CharClass::ExponentStart},
            {Char{Char::Digit1}, CharClass::BinaryDigit},
            {Char{Char::Minus}, CharClass::PlusOrMinus},
            {Char{Char::DoubleQuote}, CharClass::SectionNameStart},
            {Char{Char::Underscore}, CharClass::FormatIdentifierChar},
            {Char{Char::Micro}, CharClass::IntegerSuffixChar},
            {Char{Char::NewLine}, CharClass::LineBreakOrEnd},
            {Char{Char::CommentStart}, CharClass::ValidAfterValue},
            {Char{Char::LcA}, CharClass::ValidLang},
        };
        for (const auto &[ch, cls] : cases) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                REQUIRE(ch.isClass(cls));
                Char negative{U'!'};
                if (cls == CharClass::ValidLang) {
                    negative = Char{char32_t{0x001F}};
                }
                REQUIRE_FALSE(negative.isClass(cls));
            }, [&]() -> std::string {
                return std::format("class {} char U+{:04x}", static_cast<int>(cls), static_cast<uint32_t>(ch.raw()));
            });
        }
    }

    void testConversions() {
        REQUIRE_EQUAL(Char{Char::UcB}.toRegularName(), Char::LcB);
        REQUIRE_EQUAL(Char{Char::Space}.toRegularName(), Char::Underscore);

        String out;
        Char{Char::UcA}.appendLowerCaseTo(out);
        REQUIRE(out == u8"a");
        out.clear();
        Char{Char::Space}.appendRegularNameTo(out);
        REQUIRE(out == u8"_");

        Char c{Char::Digit9};
        REQUIRE_EQUAL(c.raw(), static_cast<char32_t>(Char::Digit9));
    }

    void testDigitConversions() {
        REQUIRE_EQUAL(Char{Char::Digit0}.toDecimalDigitValue(), uint8_t{0});
        REQUIRE_EQUAL(Char{Char::Digit9}.toDecimalDigitValue(), uint8_t{9});
        REQUIRE_EQUAL(Char{Char::UcA}.toDecimalDigitValue(), uint8_t{0});

        REQUIRE_EQUAL(Char{Char::Digit0}.toHexDigitValue(), uint8_t{0});
        REQUIRE_EQUAL(Char{Char::Digit9}.toHexDigitValue(), uint8_t{9});
        REQUIRE_EQUAL(Char{Char::LcA}.toHexDigitValue(), uint8_t{10});
        REQUIRE_EQUAL(Char{Char::UcF}.toHexDigitValue(), uint8_t{15});
        REQUIRE_EQUAL(Char{U'G'}.toHexDigitValue(), uint8_t{0});
    }
};

