// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ValueTestHelper.hpp"


TESTED_TARGETS(Document Value)
class ValueGetMethodsTest final : public UNITTEST_SUBCLASS(ValueTestHelper) {
public:

    template<typename T, typename U>
    void requireFail(
        T (Value::*defaultFn)(const NamePathLike&, U) const,
        T (Value::*throwFn)(const NamePathLike&) const,
        U defaultValue) {

        const auto namePath = NamePath::fromText(u8"main.value");
        T result;
        REQUIRE_NOTHROW(result = (doc.get()->*defaultFn)(namePath, defaultValue));
        REQUIRE_EQUAL(result, defaultValue);
        REQUIRE_NOTHROW(result = doc->get<T>(namePath, defaultValue));
        REQUIRE_EQUAL(result, defaultValue);
        const auto name = NamePath::fromText(u8"value");
        const auto mainValue = doc->valueOrThrow(u8"main");
        REQUIRE_NOTHROW(result = (mainValue.get()->*defaultFn)(name, defaultValue));
        REQUIRE_EQUAL(result, defaultValue);
        REQUIRE_NOTHROW(result = mainValue->get<T>(name, defaultValue));
        REQUIRE_EQUAL(result, defaultValue);
        try {
            result = (doc.get()->*throwFn)(namePath);
            REQUIRE(false);
        } catch (const Error &e) {
            REQUIRE_EQUAL(e.category(), ErrorCategory::TypeMismatch);
        }
        try {
            result = doc->getOrThrow<T>(namePath);
            REQUIRE(false);
        } catch (const Error &e) {
            REQUIRE_EQUAL(e.category(), ErrorCategory::TypeMismatch);
        }
    }

    void requireGetValueListFail() {
        const auto namePath = NamePath::fromText(u8"main.value");
        ValueList result;
        REQUIRE_NOTHROW(result = doc->getValueList(namePath));
        REQUIRE_EQUAL(result, ValueList{});
        try {
            result = doc->getValueListOrThrow(namePath);
            REQUIRE(false);
        } catch (const Error &e) {
            REQUIRE_EQUAL(e.category(), ErrorCategory::TypeMismatch);
        }
        const auto name = NamePath::fromText(u8"value");
        const auto mainValue = doc->valueOrThrow(u8"main");
        REQUIRE_NOTHROW(result = mainValue->getValueList(name));
        REQUIRE_EQUAL(result,  ValueList{});
        try {
            result = mainValue->getValueListOrThrow(name);
            REQUIRE(false);
        } catch (const Error &e) {
            REQUIRE_EQUAL(e.category(), ErrorCategory::TypeMismatch);
        }
    }

    TESTED_TARGETS(asInteger)
    void testGetInteger() {
        WITH_CONTEXT(setupTemplate3("123"));
        // valid conversion
        const auto namePath = NamePath::fromText(u8"main.value");
        REQUIRE_EQUAL(doc->getInteger(namePath), 123);
        REQUIRE_EQUAL(doc->getIntegerOrThrow(namePath), 123);
        REQUIRE_EQUAL(doc->get<uint8_t>(namePath), 123U);
        REQUIRE_EQUAL(doc->getOrThrow<uint8_t>(namePath), 123U);
        REQUIRE_EQUAL(doc->get<int8_t>(namePath), 123);
        REQUIRE_EQUAL(doc->getOrThrow<int8_t>(namePath), 123);
        REQUIRE_EQUAL(doc->get<uint16_t>(namePath), 123U);
        REQUIRE_EQUAL(doc->getOrThrow<uint16_t>(namePath), 123U);
        REQUIRE_EQUAL(doc->get<int16_t>(namePath), 123);
        REQUIRE_EQUAL(doc->getOrThrow<int16_t>(namePath), 123);
        REQUIRE_EQUAL(doc->get<uint32_t>(namePath), 123U);
        REQUIRE_EQUAL(doc->getOrThrow<uint32_t>(namePath), 123U);
        REQUIRE_EQUAL(doc->get<int32_t>(namePath), 123);
        REQUIRE_EQUAL(doc->getOrThrow<int32_t>(namePath), 123);
        REQUIRE_EQUAL(doc->get<uint64_t>(namePath), 123U);
        REQUIRE_EQUAL(doc->getOrThrow<uint64_t>(namePath), 123U);
        REQUIRE_EQUAL(doc->get<int64_t>(namePath), 123);
        REQUIRE_EQUAL(doc->getOrThrow<int64_t>(u8"main.value"), 123);
        // all other types must fail.
        //WITH_CONTEXT(requireFail<Integer, Integer>(&Value::asInteger, &Value::asIntegerOrThrow, 789));
        WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, true));
        WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 987.654));
        WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{u8"hello"}));
        WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date(2024, 11, 10)));
        WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time(17, 22, 33, 100, {})));
        WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime(Date(2024, 11, 10), Time(17, 22, 33, 100, {}))));
        WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes::fromHex(u8"ffeedd")));
        WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta(TimeUnit::Minutes, 55)));
        WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx(u8"other")));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getBoolean)
    void testGetBoolean() {
        WITH_CONTEXT(setupTemplate3("true"));
        const auto path = NamePath::fromText(u8"main.value");
        REQUIRE_EQUAL(doc->getBoolean(path), true);
        REQUIRE_EQUAL(doc->getBooleanOrThrow(path), true);
        REQUIRE_EQUAL(doc->get<bool>(path, false), true);
        REQUIRE_EQUAL(doc->getOrThrow<bool>(path), true);
        WITH_CONTEXT(requireFail<Integer, Integer>(&Value::getInteger, &Value::getIntegerOrThrow, 0));
        // WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, false));
        WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 0.0));
        WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{u8""}));
        WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date{}));
        WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time{}));
        WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime{}));
        WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes{}));
        WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta{}));
        WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx{}));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getFloat)
    void testGetFloat() {
        WITH_CONTEXT(setupTemplate3("123.5"));
        const auto path = NamePath::fromText(u8"main.value");
        REQUIRE_LESS(std::abs(doc->getFloat(path, 0.0) - 123.5), std::numeric_limits<double>::epsilon());
        REQUIRE_LESS(std::abs(doc->getFloatOrThrow(path) - 123.5), std::numeric_limits<double>::epsilon());
        REQUIRE_LESS(std::abs(doc->get<double>(path, 0.0) - 123.5), std::numeric_limits<double>::epsilon());
        REQUIRE_LESS(std::abs(doc->getOrThrow<double>(path) - 123.5), std::numeric_limits<double>::epsilon());
        WITH_CONTEXT(requireFail<Integer, Integer>(&Value::getInteger, &Value::getIntegerOrThrow, 0));
        WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, false));
        // WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 0.0));
        WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{u8""}));
        WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date{}));
        WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time{}));
        WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime{}));
        WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes{}));
        WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta{}));
        WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx{}));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getText)
    void testGetText() {
        WITH_CONTEXT(setupTemplate3("\"text\""));
        const auto path = NamePath::fromText(u8"main.value");
        REQUIRE_EQUAL(doc->getText(path, String{}), String{u8"text"});
        REQUIRE_EQUAL(doc->getTextOrThrow(path), String{u8"text"});
        REQUIRE_EQUAL(doc->get<String>(path, String{}), String{u8"text"});
        REQUIRE_EQUAL(doc->getOrThrow<String>(path), String{u8"text"});
        REQUIRE_EQUAL(doc->get<std::string>(path, std::string{}), std::string{"text"});
        REQUIRE_EQUAL(doc->getOrThrow<std::string>(path), std::string{"text"});
        WITH_CONTEXT(requireFail<Integer, Integer>(&Value::getInteger, &Value::getIntegerOrThrow, 0));
        WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, false));
        WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 0.0));
        // WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{}));
        WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date{}));
        WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time{}));
        WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime{}));
        WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes{}));
        WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta{}));
        WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx{}));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getDate)
    void testGetDate() {
        WITH_CONTEXT(setupTemplate3("2025-01-20"));
        const auto path = NamePath::fromText(u8"main.value");
        const auto expected = Date(2025, 1, 20);
        REQUIRE_EQUAL(doc->getDate(path, Date{}), expected);
        REQUIRE_EQUAL(doc->getDateOrThrow(path), expected);
        REQUIRE_EQUAL(doc->get<Date>(path, Date{}), expected);
        REQUIRE_EQUAL(doc->getOrThrow<Date>(path), expected);
        WITH_CONTEXT(requireFail<Integer, Integer>(&Value::getInteger, &Value::getIntegerOrThrow, 0));
        WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, false));
        WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 0.0));
        WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{}));
        WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time{}));
        // WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date{}));
        WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime{}));
        WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes{}));
        WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta{}));
        WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx{}));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getTime)
    void testGetTime() {
        WITH_CONTEXT(setupTemplate3("14:08:32"));
        const auto path = NamePath::fromText(u8"main.value");
        const auto expected = Time(14, 8, 32, 0, TimeOffset());
        REQUIRE_EQUAL(doc->getTime(path, Time{}), expected);
        REQUIRE_EQUAL(doc->getTimeOrThrow(path), expected);
        REQUIRE_EQUAL(doc->get<Time>(path, Time{}), expected);
        REQUIRE_EQUAL(doc->getOrThrow<Time>(path), expected);
        WITH_CONTEXT(requireFail<Integer, Integer>(&Value::getInteger, &Value::getIntegerOrThrow, 0));
        WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, false));
        WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 0.0));
        WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{}));
        WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date{}));
        // WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time{}));
        WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime{}));
        WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes{}));
        WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta{}));
        WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx{}));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getDateTime)
    void testGetDateTime() {
        WITH_CONTEXT(setupTemplate3("2025-01-20 14:08:32"));
        const auto path = NamePath::fromText(u8"main.value");
        const auto expected = DateTime(Date(2025, 1, 20), Time(14, 8, 32, 0, TimeOffset()));
        REQUIRE_EQUAL(doc->getDateTime(path, DateTime{}), expected);
        REQUIRE_EQUAL(doc->getDateTimeOrThrow(path), expected);
        REQUIRE_EQUAL(doc->get<DateTime>(path, DateTime{}), expected);
        REQUIRE_EQUAL(doc->getOrThrow<DateTime>(path), expected);
        WITH_CONTEXT(requireFail<Integer, Integer>(&Value::getInteger, &Value::getIntegerOrThrow, 0));
        WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, false));
        WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 0.0));
        WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{}));
        WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date{}));
        WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time{}));
        // WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime{}));
        WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes{}));
        WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta{}));
        WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx{}));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getBytes)
    void testGetBytes() {
        WITH_CONTEXT(setupTemplate3("<01 02 03>"));
        const auto path = NamePath::fromText(u8"main.value");
        const auto expected = Bytes::fromHex(u8"010203");
        REQUIRE_EQUAL(doc->getBytes(path, Bytes{}), expected);
        REQUIRE_EQUAL(doc->getBytesOrThrow(path), expected);
        REQUIRE_EQUAL(doc->get<Bytes>(path, Bytes{}), expected);
        REQUIRE_EQUAL(doc->getOrThrow<Bytes>(path), expected);
        WITH_CONTEXT(requireFail<Integer, Integer>(&Value::getInteger, &Value::getIntegerOrThrow, 0));
        WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, false));
        WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 0.0));
        WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{}));
        WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date{}));
        WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time{}));
        WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime{}));
        // WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes{}));
        WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta{}));
        WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx{}));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getTimeDelta)
    void testGetTimeDelta() {
        WITH_CONTEXT(setupTemplate3("10 years"));
        const auto path = NamePath::fromText(u8"main.value");
        const auto expected = TimeDelta(TimeUnit::Years, 10);
        REQUIRE_EQUAL(doc->getTimeDelta(path, TimeDelta{}), expected);
        REQUIRE_EQUAL(doc->getTimeDeltaOrThrow(path), expected);
        REQUIRE_EQUAL(doc->get<TimeDelta>(path, TimeDelta{}), expected);
        REQUIRE_EQUAL(doc->getOrThrow<TimeDelta>(path), expected);
        WITH_CONTEXT(requireFail<Integer, Integer>(&Value::getInteger, &Value::getIntegerOrThrow, 0));
        WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, false));
        WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 0.0));
        WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{}));
        WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date{}));
        WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time{}));
        WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime{}));
        WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes{}));
        // WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta{}));
        WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx{}));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getRegEx)
    void testGetRegEx() {
        WITH_CONTEXT(setupTemplate3("/regex/"));
        const auto path = NamePath::fromText(u8"main.value");
        const auto expected = RegEx(u8"regex");
        REQUIRE_EQUAL(doc->getRegEx(path, RegEx{}), expected);
        REQUIRE_EQUAL(doc->getRegExOrThrow(path), expected);
        REQUIRE_EQUAL(doc->get<RegEx>(path, RegEx{}), expected);
        REQUIRE_EQUAL(doc->getOrThrow<RegEx>(path), expected);
        WITH_CONTEXT(requireFail<Integer, Integer>(&Value::getInteger, &Value::getIntegerOrThrow, 0));
        WITH_CONTEXT(requireFail<bool, bool>(&Value::getBoolean, &Value::getBooleanOrThrow, false));
        WITH_CONTEXT(requireFail<Float, Float>(&Value::getFloat, &Value::getFloatOrThrow, 0.0));
        WITH_CONTEXT(requireFail<String, const String&>(&Value::getText, &Value::getTextOrThrow, String{}));
        WITH_CONTEXT(requireFail<Date, const Date&>(&Value::getDate, &Value::getDateOrThrow, Date{}));
        WITH_CONTEXT(requireFail<Time, const Time&>(&Value::getTime, &Value::getTimeOrThrow, Time{}));
        WITH_CONTEXT(requireFail<DateTime, const DateTime&>(&Value::getDateTime, &Value::getDateTimeOrThrow, DateTime{}));
        WITH_CONTEXT(requireFail<Bytes, const Bytes&>(&Value::getBytes, &Value::getBytesOrThrow, Bytes{}));
        WITH_CONTEXT(requireFail<TimeDelta, const TimeDelta&>(&Value::getTimeDelta, &Value::getTimeDeltaOrThrow, TimeDelta{}));
        // WITH_CONTEXT(requireFail<RegEx, const RegEx&>(&Value::getRegEx, &Value::getRegExOrThrow, RegEx{}));
        WITH_CONTEXT(requireGetValueListFail());
    }

    TESTED_TARGETS(getValueList)
    void testGetValueList() {
        WITH_CONTEXT(setupTemplate3("1, 2, 3"));
        const auto path = NamePath::fromText(u8"main.value");
        const auto list = doc->getValueList(path);
        REQUIRE_EQUAL(list.size(), 3U);
        REQUIRE_EQUAL(list[0]->asInteger(), 1);
        REQUIRE_EQUAL(list[1]->asInteger(), 2);
        REQUIRE_EQUAL(list[2]->asInteger(), 3);
        REQUIRE_EQUAL(doc->getValueListOrThrow(path).size(), 3U);
    }

    TESTED_TARGETS(getSectionList)
    void testGetSectionList() {
        WITH_CONTEXT(setupTemplate1("1"));
        const auto path = NamePath::fromText(u8"list");
        const auto sec = doc->getSectionList(path);
        REQUIRE_EQUAL(sec->size(), 3U);
        REQUIRE_EQUAL(doc->getSectionListOrThrow(path)->size(), 3U);
    }

    TESTED_TARGETS(getSectionWithNames)
    void testGetSectionWithNames() {
        WITH_CONTEXT(setupTemplate1("1"));
        const auto path = NamePath::fromText(u8"main");
        const auto sec = doc->getSectionWithNames(path);
        REQUIRE_EQUAL(sec->size(), 11U);
        REQUIRE_EQUAL(doc->getSectionWithNamesOrThrow(path)->size(), 11U);
    }

    TESTED_TARGETS(getSectionWithTexts)
    void testGetSectionWithTexts() {
        WITH_CONTEXT(setupTemplate1("1"));
        const auto path = NamePath::fromText(u8"main.text");
        const auto sec = doc->getSectionWithTexts(path);
        REQUIRE_EQUAL(sec->size(), 3U);
        REQUIRE_EQUAL(doc->getSectionWithTextsOrThrow(path)->size(), 3U);

        const auto invalidPath = NamePath::fromText(u8"main.value1");
        ValuePtr invalidSec;
        REQUIRE_NOTHROW(invalidSec = doc->getSectionWithTexts(invalidPath));
        REQUIRE_EQUAL(invalidSec, nullptr);
    }
};
