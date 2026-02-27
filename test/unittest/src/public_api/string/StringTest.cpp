// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/String.hpp>
#include <erbsland/conf/Bytes.hpp>
#include <erbsland/conf/Error.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>


using namespace el::conf;


TESTED_TARGETS(String)
class StringTest final : public el::UnitTest {
public:
    void testDefault() {
        String const str;
        REQUIRE(str.empty());
        REQUIRE(str.length() == 0);
        REQUIRE(str.size() == 0);
    }

    void testConstructor() {
        auto str = String{u8"utf8 😀"};
        REQUIRE(!str.empty());
        REQUIRE(str.length() == 9);
        REQUIRE(str.size() == 9);
        auto str2 = String("utf8 😀");
        REQUIRE(!str2.empty());
        REQUIRE(str2.length() == 9);
        REQUIRE(str2.size() == 9);
    }

    void testCopy() {
        auto str1 = String{u8" ➤ text 1"};
        auto str2 = String{str1};
        REQUIRE(str1 == str2);
        REQUIRE(str2 == u8" ➤ text 1");
    }

    void testAssign() {
        auto str1 = String{u8" ➤ text 1"};
        auto str2 = String{};
        REQUIRE(str1 != str2);
        str2 = str1;
        REQUIRE(str1 == str2);
        REQUIRE(str1 == u8" ➤ text 1");
        REQUIRE(str2 == u8" ➤ text 1");
    }

    void testConversion() {
        auto u8str = String{u8"utf8 😀"};
        auto str = u8str.toCharString();
        REQUIRE(str.length() == 9);
        REQUIRE(str == "utf8 😀");
        u8str = String{str};
        REQUIRE(u8str.length() == 9);
        REQUIRE(u8str == u8"utf8 😀");
    }

    void testIterator() {
        static const auto result = std::array<char8_t, 4>{u8't', u8'e', u8's', u8'*'};
        static const auto rResult = std::array<char8_t, 4>{u8'*', u8's', u8'e', u8't'};
        auto str = String{"tes*"};
        const String cStr = {"tes*"};
        for (auto it = str.begin(); it != str.end(); ++it) {
            REQUIRE(*it == result[std::distance(str.begin(), it)]);
        }
        for (auto it = str.cbegin(); it != str.cend(); ++it) {
            REQUIRE(*it == result[std::distance(str.cbegin(), it)]);
        }
        for (auto it = cStr.begin(); it != cStr.end(); ++it) {
            REQUIRE(*it == result[std::distance(cStr.begin(), it)]);
        }
        for (auto it = str.rbegin(); it != str.rend(); ++it) {
            REQUIRE(*it == rResult[std::distance(str.rbegin(), it)]);
        }
        for (auto it = str.crbegin(); it != str.crend(); ++it) {
            REQUIRE(*it == rResult[std::distance(str.crbegin(), it)]);
        }
        for (auto it = cStr.rbegin(); it != cStr.rend(); ++it) {
            REQUIRE(*it == rResult[std::distance(cStr.rbegin(), it)]);
        }
    }

    void testCompare()
    {
        auto str1 = String{u8"string 1"};
        auto str2 = String{u8"string 1"};
        auto str3 = String{u8"string 2"};

        // Test equality operator
        REQUIRE(str1 == str2);
        REQUIRE(!(str1 == str3));

        // Test inequality operator
        REQUIRE(str1 != str3);
        REQUIRE(!(str1 != str2));

        // Test less than operator
        REQUIRE(str1 < str3);
        REQUIRE(!(str3 < str1));
        REQUIRE(!(str1 < str2));

        // Test less than or equal operator
        REQUIRE(str1 <= str2);
        REQUIRE(str1 <= str3);
        REQUIRE(!(str3 <= str1));

        // Test greater than operator
        REQUIRE(str3 > str1);
        REQUIRE(!(str1 > str3));
        REQUIRE(!(str1 > str2));

        // Test greater than or equal operator
        REQUIRE(str1 >= str2);
        REQUIRE(str3 >= str1);
        REQUIRE(!(str1 >= str3));
    }

    void testSpaceshipOperator() {
        auto str1 = String{u8"alpha"};
        auto str2 = String{u8"alpha"};
        auto str3 = String{u8"beta"};

        // Test equality
        REQUIRE((str1 <=> str2) == 0);
        REQUIRE((str1 <=> str3) != 0);

        // Test less than
        REQUIRE((str1 <=> str3) < 0);
        REQUIRE((str3 <=> str1) > 0);

        // Test greater than
        REQUIRE((str3 <=> str1) > 0);
        REQUIRE((str1 <=> str3) < 0);
    }
    
    void testIndexOperator() {
        auto str = String{u8"Erbsland"};

        // Test valid index access
        REQUIRE(str[0] == u8'E');
        REQUIRE(str[6] == u8'n');
        REQUIRE(str[7] == u8'd');

        // Test boundaries
        REQUIRE(str[0] == str.front());
        REQUIRE(str[7] == str.back());
    }

    void testAtMethod() {
        auto str = String{u8"Erbsland"};

        // Test valid access using at()
        REQUIRE(str.at(0) == u8'E');
        REQUIRE(str.at(6) == u8'n');
        REQUIRE(str.at(7) == u8'd');

        // Test boundaries
        REQUIRE(str.at(0) == str.front());
        REQUIRE(str.at(7) == str.back());

        // Test out-of-bound access, expected to throw an exception
        REQUIRE_THROWS_AS(std::out_of_range, str.at(9));
    }

    void testReserveAndCapacity() {
        String str;

        // Reserve more capacity
        str.reserve(50);
        REQUIRE(str.capacity() >= 50); // Ensure that the capacity is at least 50

        // Verify that the size remains unchanged
        REQUIRE(str.size() == 0);
        REQUIRE(str.empty());

        // Add string to test reserve effect
        str = u8"short text";
        REQUIRE(str.size() == 10);

        // Reserve less and check that the capacity does not affect the string.
        str.reserve(5);
        REQUIRE(str.size() == 10);

        // Add more content and check capacity
        str = String{u8"this is a significantly longer text to test capacity"};
        REQUIRE(str.size() == 52); // Length of the new string
        REQUIRE(str.capacity() >= 52); // The capacity should be at least the size of the content

        // Check reserve when capacity increases
        str.reserve(100);
        REQUIRE(str.capacity() >= 100);
    }

    void testShrinkToFit() {
        String str = u8"short text";;
        REQUIRE(str.size() == 10);
        str.reserve(1000);
        REQUIRE(str.capacity() >= 1000);
        str.shrink_to_fit();
        REQUIRE(str.capacity() < 1000);
    }

    void testAppend() {
        auto str1 = String{u8"text 1"};
        auto str2 = String{u8"text 2"};
        auto str3 = str1 + str2;
        REQUIRE(str3 == u8"text 1text 2");

        // Test += operator
        str1 += str2;
        REQUIRE(str1 == u8"text 1text 2");

        // Reset str1 for further testing
        str1 = String{u8"text 1"};

        // Test append method
        str1.append(str2);
        REQUIRE(str1 == u8"text 1text 2");
    }

    void testAppendMixedStrings() {
        auto str1 = String{u8"prefix"};
        REQUIRE(str1 == u8"prefix");
        str1 += u8"⋅";
        REQUIRE(str1 == u8"prefix⋅");
        str1 += u8'x';
        REQUIRE(str1 == u8"prefix⋅x");
        str1.append(u8"↦");
        REQUIRE(str1 == u8"prefix⋅x↦");
        str1.append(u8':');
        REQUIRE(str1 == u8"prefix⋅x↦:");
        str1 += "⋅";
        REQUIRE(str1 == u8"prefix⋅x↦:⋅");
        str1 += '[';
        REQUIRE(str1 == u8"prefix⋅x↦:⋅[");
        str1.append(']');
        REQUIRE(str1 == u8"prefix⋅x↦:⋅[]");
    }

    void testSubstr() {
        auto str1 = String{u8"text 1"};
        auto str2 = str1.substr(0, 4);
        REQUIRE(str2 == u8"text");

        // Test single parameter version
        auto str3 = str1.substr(5);
        REQUIRE(str3 == u8"1");

        // Test exception case where pos is greater than the size of the string
        REQUIRE_THROWS_AS(std::out_of_range, str1.substr(10));
    }

    void testFindFunctions() {
        //                  0         1         2         3         4
        String const str{u8"The quick brown fox jumps over the lazy dog"};

        // Test find
        REQUIRE(str.find(u8"quick") == 4);
        REQUIRE(str.find(u8"fox") == 16);
        REQUIRE(str.find(u8"cat") == std::u8string::npos);

        // Test rfind
        REQUIRE(str.rfind(u8"the") == 31);
        REQUIRE(str.rfind(u8"quick") == 4);
        REQUIRE(str.rfind(u8"cat") == std::u8string::npos);

        // Test find_first_of
        REQUIRE(str.find_first_of(u8"aeiou") == 2);
        REQUIRE(str.find_first_of(u8"xyz") == 18);
        REQUIRE(str.find_first_of(u8"(:-)*") == std::u8string::npos);

        // Test find_last_of
        REQUIRE(str.find_last_of(u8"aeiou") == 41);
        REQUIRE(str.find_last_of(u8"xyz") == 38);
        REQUIRE(str.find_last_of(u8"(:-)*") == std::u8string::npos);

        // Test find_first_not_of
        REQUIRE(str.find_first_not_of(u8"Tabcdefghijklmnopqrstuvwxyz ") == std::u8string::npos);
        REQUIRE(str.find_first_not_of(u8"Tabcdefghijklmnopqrstuvwxy ") == 37);
        REQUIRE(str.find_first_not_of(u8" ") == 0);

        // Test find_last_not_of
        REQUIRE(str.find_last_not_of(u8"dog") == 39);
        REQUIRE(str.find_last_not_of(u8" ") == 42);
        REQUIRE(str.find_last_not_of(u8"") == 42);
    }

    void testStr() {
        auto str = String{u8"The quick brown fox jumps over the lazy dog"};
        REQUIRE(str == u8"The quick brown fox jumps over the lazy dog");
        REQUIRE(str.raw() == u8"The quick brown fox jumps over the lazy dog");
        str.raw()[0] = u8'X';
        REQUIRE(str.raw() == u8"Xhe quick brown fox jumps over the lazy dog");
        REQUIRE(str == u8"Xhe quick brown fox jumps over the lazy dog");
    }

    void testCompareLiterals() {
        auto str = String{u8"test😀"};
        REQUIRE(str == u8"test😀");
        REQUIRE(str != u8"another");
        REQUIRE(str == "test😀");
        REQUIRE(str != "another");
    }

    void testLiteralConstruction() {
        String str = u8"test😀";
        REQUIRE(str == u8"test😀");
        str = "test😆";
        REQUIRE(str == u8"test😆");
    }

    void testStdConstruction() {
        auto str = String{std::u8string{u8"test😀"}};
        REQUIRE(str == u8"test😀");
        str = String{std::string{"test😊"}};
        REQUIRE(str == u8"test😊");
    }
};

