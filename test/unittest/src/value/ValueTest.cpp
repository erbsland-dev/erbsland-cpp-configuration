// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/value/Value.hpp>
#include <erbsland/conf/Location.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <memory>
#include <vector>


using namespace el::conf;


struct DoesToMethodReturnDefault {
    virtual ~DoesToMethodReturnDefault() = default;
    [[nodiscard]] virtual auto isDefault(ValuePtr value) const -> bool = 0;
};

template<typename Type>
using ValueToMethod = Type (Value::*)() const;
template <typename Type, ValueToMethod<Type> tMethod>
struct DoesToMethodReturnDefaultT : DoesToMethodReturnDefault {
    [[nodiscard]] auto isDefault(ValuePtr value) const -> bool override {
        return (value.get()->*tMethod)() == Type{};
    }
};

static std::vector<std::pair<ValueType, std::shared_ptr<DoesToMethodReturnDefault>>> const cDoesReturnDefault = {
    std::make_pair(
        ValueType::Integer,
        std::make_shared<DoesToMethodReturnDefaultT<int64_t, &Value::toInteger>>()),
    std::make_pair(
        ValueType::Boolean,
        std::make_shared<DoesToMethodReturnDefaultT<bool, &Value::toBoolean>>()),
    std::make_pair(
        ValueType::Float,
        std::make_shared<DoesToMethodReturnDefaultT<double, &Value::toFloat>>()),
    std::make_pair(
        ValueType::Text,
        std::make_shared<DoesToMethodReturnDefaultT<String, &Value::toText>>()),
    std::make_pair(
        ValueType::Date,
        std::make_shared<DoesToMethodReturnDefaultT<Date, &Value::toDate>>()),
    std::make_pair(
        ValueType::Time,
        std::make_shared<DoesToMethodReturnDefaultT<Time, &Value::toTime>>()),
    std::make_pair(
        ValueType::DateTime,
        std::make_shared<DoesToMethodReturnDefaultT<DateTime, &Value::toDateTime>>()),
    std::make_pair(
        ValueType::Bytes,
        std::make_shared<DoesToMethodReturnDefaultT<Bytes, &Value::toBytes>>()),
    std::make_pair(
        ValueType::TimeDelta,
        std::make_shared<DoesToMethodReturnDefaultT<TimeDelta, &Value::toTimeDelta>>()),
    std::make_pair(
        ValueType::RegEx,
        std::make_shared<DoesToMethodReturnDefaultT<String, &Value::toRegEx>>()),
};


TESTED_TARGETS(Value ValueType)
class ValueTest final : public el::UnitTest {
public:
    ValuePtr value;

    void requireDefaults(ValueType valueType) {
        REQUIRE(value != nullptr);
        REQUIRE(value->hasParent() == false);
        REQUIRE(value->parent() == nullptr);
        REQUIRE(value->type() == valueType);
        REQUIRE(value->hasLocation() == false);
        REQUIRE(value->location().isUndefined());
        REQUIRE(value->size() == 0);
        REQUIRE(value->value(0) == nullptr);
        // REQUIRE(value->value(u8"test") == nullptr);
        REQUIRE(value->begin() == value->end());
        for (auto const& [type, op] : cDoesReturnDefault) {
            if (type != valueType) {
                runWithContext(SOURCE_LOCATION(), [&]() {
                    REQUIRE(op->isDefault(value) == true);
                }, [&]() {
                    return std::format("Tested type = {}, failed default type = {}",
                        valueType.toText().toCharString(), type.toText().toCharString());
                });
            }
        }
    }

    void testValueTypes() {
        value = impl::Value::createInteger(70ll);
        WITH_CONTEXT(requireDefaults(ValueType::Integer));
        REQUIRE(value->toInteger() == 70ll);
        REQUIRE(value->toTextRepresentation() == u8"70");
        value = impl::Value::createInteger(0x1234'5678'abcd'ef01ll); // make sure 64bit are actually stored.
        REQUIRE(value->toInteger() == 0x1234'5678'abcd'ef01ll);
        REQUIRE(value->toTextRepresentation() == u8"1311768467750121217")
        value = impl::Value::createBoolean(true);
        WITH_CONTEXT(requireDefaults(ValueType::Boolean));
        REQUIRE(value->toBoolean() == true);
        REQUIRE(value->toTextRepresentation() == u8"true")
        value = impl::Value::createBoolean(false);
        REQUIRE(value->toBoolean() == false);
        REQUIRE(value->toTextRepresentation() == u8"false")
        value = impl::Value::createFloat(29.18e+20);
        WITH_CONTEXT(requireDefaults(ValueType::Float));
        REQUIRE(std::abs(value->toFloat() - 29.18e+20) < 1e-10);
        value = impl::Value::createText(u8"→ Text ←");
        WITH_CONTEXT(requireDefaults(ValueType::Text));
        REQUIRE(value->toText() == u8"→ Text ←");
        REQUIRE(value->toTextRepresentation() == u8"→ Text ←");
        value = impl::Value::createDate(Date(2024, 8, 21));
        WITH_CONTEXT(requireDefaults(ValueType::Date));
        REQUIRE(value->toDate() == Date(2024, 8, 21));
        REQUIRE(value->toTextRepresentation() == u8"2024-08-21");
        value = impl::Value::createTime(Time(23, 19, 27, 0, TimeOffset::utc()));
        WITH_CONTEXT(requireDefaults(ValueType::Time));
        REQUIRE(value->toTime() == Time(23, 19, 27, 0, TimeOffset::utc()));
        REQUIRE(value->toTextRepresentation() == u8"23:19:27z");
        value = impl::Value::createDateTime(DateTime(Date(2024, 8, 21), Time(23, 19, 27, 0, TimeOffset::utc())));
        WITH_CONTEXT(requireDefaults(ValueType::DateTime));
        REQUIRE(value->toDateTime() == DateTime(Date(2024, 8, 21), Time(23, 19, 27, 0, TimeOffset::utc())));
        REQUIRE(value->toTextRepresentation() == u8"2024-08-21 23:19:27z");
        value = impl::Value::createBytes(Bytes({std::byte{0x01}, std::byte{0x02}, std::byte{0xff}, std::byte{0x00}}));
        WITH_CONTEXT(requireDefaults(ValueType::Bytes));
        REQUIRE(value->toBytes() == Bytes({std::byte{0x01}, std::byte{0x02}, std::byte{0xff}, std::byte{0x00}}));
        REQUIRE(value->toTextRepresentation() == u8"0102ff00");
        value = impl::Value::createTimeDelta(TimeDelta(TimeUnit::Hours, 18));
        WITH_CONTEXT(requireDefaults(ValueType::TimeDelta));
        REQUIRE(value->toTimeDelta() == TimeDelta(TimeUnit::Hours, 18));
        REQUIRE(value->toTextRepresentation() == u8"18h");
        value = impl::Value::createRegEx(u8"^\\d+$");
        WITH_CONTEXT(requireDefaults(ValueType::RegEx));
        REQUIRE(value->toRegEx() == u8"^\\d+$");
        REQUIRE(value->toTextRepresentation() == u8"^\\d+$");
        value = impl::Value::createValueList({});
        WITH_CONTEXT(requireDefaults(ValueType::ValueList));
        REQUIRE(value->toList().empty() == true);
        REQUIRE(value->toTextRepresentation().empty());
        value = impl::Value::createSectionList();
        WITH_CONTEXT(requireDefaults(ValueType::SectionList));
        REQUIRE(value->toTextRepresentation().empty());
        value = impl::Value::createIntermediateSection();
        WITH_CONTEXT(requireDefaults(ValueType::IntermediateSection));
        REQUIRE(value->toTextRepresentation().empty());
        value = impl::Value::createSectionWithNames();
        WITH_CONTEXT(requireDefaults(ValueType::SectionWithNames));
        REQUIRE(value->toTextRepresentation().empty());
        value = impl::Value::createSectionWithTexts();
        WITH_CONTEXT(requireDefaults(ValueType::SectionWithTexts));
        REQUIRE(value->toTextRepresentation().empty());
    }

    void testLocation() {
        value = impl::Value::createInteger(1);
        REQUIRE(value->hasLocation() == false);
        auto sourceIdentifier = SourceIdentifier::createForFile(u8"main.elcl");
        value->setLocation(Location(sourceIdentifier, Position{10, 5}));
        REQUIRE(value->hasLocation() == true);
        REQUIRE(value->location() == Location(sourceIdentifier, Position{10, 5}));
        auto sourceIdentifier2 = SourceIdentifier::createForFile(u8"another.elcl");
        value->setLocation(Location(sourceIdentifier2, Position{7, 9}));
        REQUIRE(value->hasLocation() == true);
        REQUIRE(value->location() == Location(sourceIdentifier2, Position{7, 9}));
        value->setLocation({});
        REQUIRE(value->hasLocation() == false);
        REQUIRE(value->location() == Location());
    }
};

