// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DefaultValueFactory.hpp"


#include <string_view>


namespace erbsland::conf::vr::builder::detail {


namespace {


template<typename T>
auto createScalarListValue(const std::vector<T> &values) -> impl::ValuePtr {
    std::vector<impl::ValuePtr> list;
    list.reserve(values.size());
    for (const auto &value : values) {
        list.emplace_back(createDefaultValue(value));
    }
    return impl::Value::createValueList(std::move(list));
}


template<typename T>
auto createScalarMatrixValue(const std::vector<std::vector<T>> &values) -> impl::ValuePtr {
    std::vector<impl::ValuePtr> rows;
    rows.reserve(values.size());
    for (const auto &row : values) {
        rows.emplace_back(createScalarListValue(row));
    }
    return impl::Value::createValueList(std::move(rows));
}


}


auto createDefaultValue(const Integer value) -> impl::ValuePtr {
    return impl::Value::createInteger(value);
}


auto createDefaultValue(const bool value) -> impl::ValuePtr {
    return impl::Value::createBoolean(value);
}


auto createDefaultValue(const Float value) -> impl::ValuePtr {
    return impl::Value::createFloat(value);
}


auto createDefaultValue(const String &value) -> impl::ValuePtr {
    return impl::Value::createText(value);
}


auto createDefaultValue(const char *value) -> impl::ValuePtr {
    return impl::Value::createText(String::fromCharString(std::string_view{value}));
}


auto createDefaultValue(const std::string &value) -> impl::ValuePtr {
    return impl::Value::createText(String::fromCharString(value));
}


auto createDefaultValue(const Date &value) -> impl::ValuePtr {
    return impl::Value::createDate(value);
}


auto createDefaultValue(const Time &value) -> impl::ValuePtr {
    return impl::Value::createTime(value);
}


auto createDefaultValue(const DateTime &value) -> impl::ValuePtr {
    return impl::Value::createDateTime(value);
}


auto createDefaultValue(const Bytes &value) -> impl::ValuePtr {
    return impl::Value::createBytes(value);
}


auto createDefaultValue(const TimeDelta &value) -> impl::ValuePtr {
    return impl::Value::createTimeDelta(value);
}


auto createDefaultValue(const RegEx &value) -> impl::ValuePtr {
    return impl::Value::createRegEx(value);
}


auto createDefaultValue(const std::vector<Integer> &values) -> impl::ValuePtr {
    return createScalarListValue(values);
}


auto createDefaultValue(const std::vector<bool> &values) -> impl::ValuePtr {
    return createScalarListValue(values);
}


auto createDefaultValue(const std::vector<Float> &values) -> impl::ValuePtr {
    return createScalarListValue(values);
}


auto createDefaultValue(const std::vector<String> &values) -> impl::ValuePtr {
    return createScalarListValue(values);
}


auto createDefaultValue(const std::vector<Bytes> &values) -> impl::ValuePtr {
    return createScalarListValue(values);
}


auto createDefaultValue(const std::vector<std::vector<Integer>> &values) -> impl::ValuePtr {
    return createScalarMatrixValue(values);
}


auto createDefaultValue(const std::vector<std::vector<Float>> &values) -> impl::ValuePtr {
    return createScalarMatrixValue(values);
}


}
