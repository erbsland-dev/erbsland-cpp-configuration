// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/utf8/U8Iterator.hpp>
#include <erbsland/conf/Error.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <vector>


using namespace erbsland::conf;
using impl::U8Iterator;
using impl::Char;


TESTED_TARGETS(U8Iterator)
class U8IteratorTest final : public el::UnitTest {
public:
    void testBasicFunctionality() {
        String str = u8"text😀→Ėtext";
        auto it = U8Iterator::begin(str);
        auto itEnd = U8Iterator::end(str);
        REQUIRE(it != itEnd);
        REQUIRE(*it == U't');
        ++it;
        REQUIRE(it != itEnd);
        REQUIRE(*it == U'e');
        it += 1;
        REQUIRE(it != itEnd);
        REQUIRE(*it == U'x');
        it += 2;
        REQUIRE(it != itEnd);
        REQUIRE(*it == U'😀');
        ++it;
        REQUIRE(it != itEnd);
        REQUIRE(*it == U'→');
        it += 1;
        REQUIRE(it != itEnd);
        REQUIRE(*it == U'Ė');
        ++it;
        REQUIRE(*it == U't');
        it += 2;
        REQUIRE(*it == U'x');
        ++it;
        REQUIRE(*it == U't');
        ++it;
        REQUIRE(it == itEnd);
        REQUIRE(*it == Char::EndOfData);
    }

    void testEmptyString() {
        String str;
        auto it = U8Iterator::begin(str);
        auto itEnd = U8Iterator::end(str);
        REQUIRE(it == itEnd);
        REQUIRE(*it == Char::EndOfData);
    }

    void testPointer() {
        String str = u8"text😀→Ėtext";
        auto it = U8Iterator::begin(str);
        auto itEnd = U8Iterator::end(str);
        it += 4;
        REQUIRE(it != itEnd);
        REQUIRE(it->utf8Size() == 4);
        it += 100;
        REQUIRE(it == itEnd);
        REQUIRE(it->isValidUnicode() == false);
    }

    void testCopyAndAssignment() {
        String str = u8"text😀→Ėtext";
        auto it = U8Iterator::begin(str);
        auto it2 = U8Iterator::begin(str);
        auto itEnd = U8Iterator::end(str);
        REQUIRE(it == it2);
        REQUIRE(it != itEnd);
        REQUIRE(it2 != itEnd);
        it += 4;
        it2 += 6;
        REQUIRE(it != it2);
        REQUIRE(it != itEnd);
        REQUIRE(it2 != itEnd);
        REQUIRE(*it == U'😀');
        REQUIRE(*it2 == U'Ė');
        it = it2;
        REQUIRE(it == it2);
        REQUIRE(it != itEnd);
        REQUIRE(it2 != itEnd);
        REQUIRE(*it == U'Ė');
        REQUIRE(*it2 == U'Ė');
        it += 100;
        it2 += 100;
        REQUIRE(it == it2);
        REQUIRE(it == itEnd);
        REQUIRE(it2 == itEnd);
        REQUIRE(*it == Char::EndOfData);
        REQUIRE(*it2 == Char::EndOfData);
    }

    void testStringProcessing() {
        const String srcText = u8"/wprg1vq17rg6c134nfv557h40000gn/config/IncludedFile.elcl";
        String actualText;
        auto it = U8Iterator::begin(srcText);
        auto endIt = U8Iterator::end(srcText);
        for (; it != endIt; ++it) {
            if (*it == impl::CharClass::FilePathSeparator) {
                actualText.append(u8":");
            } else {
                actualText.append(*it);
            }
        }
        const String expectedText = u8":wprg1vq17rg6c134nfv557h40000gn:config:IncludedFile.elcl";
        REQUIRE_EQUAL(actualText, expectedText);
    }
};


