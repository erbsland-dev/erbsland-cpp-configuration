// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ValueTestHelper.hpp"


TESTED_TARGETS(Document Value)
class ValueGetListTest final : public UNITTEST_SUBCLASS(ValueTestHelper) {
public:
    template<typename T>
    void requireIntegerList() {
        auto list = doc->getList<T>(u8"main.value_list");
        REQUIRE_EQUAL(list.size(), 3U);
        REQUIRE_EQUAL(list[0], static_cast<T>(1));
        REQUIRE_EQUAL(list[1], static_cast<T>(2));
        REQUIRE_EQUAL(list[2], static_cast<T>(3));

        REQUIRE_NOTHROW(list = doc->getListOrThrow<T>(u8"main.value_list"));
        REQUIRE_EQUAL(list.size(), 3U);
        REQUIRE_EQUAL(list[0], static_cast<T>(1));
        REQUIRE_EQUAL(list[1], static_cast<T>(2));
        REQUIRE_EQUAL(list[2], static_cast<T>(3));

        list = doc->getList<T>(u8"main.nok_value_list");
        REQUIRE(list.empty());

        try {
            list = doc->getListOrThrow<T>(u8"main.nok_value_list");
            REQUIRE(false);
        } catch (const Error &error) {
            REQUIRE_EQUAL(error.category(), ErrorCategory::TypeMismatch);
        }
    }

    TESTED_TARGETS(getList)
    void testGetIntegerList() {
        WITH_CONTEXT(setupTemplate1("1", "2", "3", "\"text\""));
        WITH_CONTEXT(requireIntegerList<uint8_t>())
        WITH_CONTEXT(requireIntegerList<int8_t>())
        WITH_CONTEXT(requireIntegerList<uint16_t>())
        WITH_CONTEXT(requireIntegerList<int16_t>())
        WITH_CONTEXT(requireIntegerList<uint32_t>())
        WITH_CONTEXT(requireIntegerList<int32_t>())
        WITH_CONTEXT(requireIntegerList<uint64_t>())
        WITH_CONTEXT(requireIntegerList<int64_t>())
        WITH_CONTEXT(requireIntegerList<int>())
        WITH_CONTEXT(requireIntegerList<unsigned int>())
        WITH_CONTEXT(requireIntegerList<short>())
        WITH_CONTEXT(requireIntegerList<unsigned short>())
        WITH_CONTEXT(requireIntegerList<long>())
        WITH_CONTEXT(requireIntegerList<unsigned long>())
        WITH_CONTEXT(requireIntegerList<long long>())
        WITH_CONTEXT(requireIntegerList<unsigned long long>())
    }

    TESTED_TARGETS(getList)
    void testGetStringList() {
        WITH_CONTEXT(setupTemplate1("\"one\"", "\"two\"", "\"three\"", "true"));

        auto list = doc->getList<String>(u8"main.value_list");
        REQUIRE_EQUAL(list.size(), 3U);
        REQUIRE_EQUAL(list[0], String{u8"one"});
        REQUIRE_EQUAL(list[1], String{u8"two"});
        REQUIRE_EQUAL(list[2], String{u8"three"});

        REQUIRE_NOTHROW(list = doc->getListOrThrow<String>(u8"main.value_list"));
        REQUIRE_EQUAL(list.size(), 3U);
        REQUIRE_EQUAL(list[0], String{u8"one"});
        REQUIRE_EQUAL(list[1], String{u8"two"});
        REQUIRE_EQUAL(list[2], String{u8"three"});

        list = doc->getList<String>(u8"main.nok_value_list");
        REQUIRE(list.empty());

        try {
            list = doc->getListOrThrow<String>(u8"main.nok_value_list");
            REQUIRE(false);
        } catch (const Error &error) {
            REQUIRE_EQUAL(error.category(), ErrorCategory::TypeMismatch);
        }
    }
};
