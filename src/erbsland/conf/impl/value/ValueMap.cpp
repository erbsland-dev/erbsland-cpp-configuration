// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ValueMap.hpp"


#include "Value.hpp"


namespace erbsland::conf::impl {


ValueMap::ValueMap(List &&valueList) : _valueList(std::move(valueList)) {
    std::size_t index = 0;
    for (const auto &value : _valueList) {
        value->setName(Name::createIndex(index));
        _valueMap.insert_or_assign(value->name(), value);
    }
}


auto ValueMap::empty() const noexcept -> bool {
    return _valueList.empty();
}


auto ValueMap::size() const noexcept -> std::size_t {
    return _valueList.size();
}


auto ValueMap::hasValue(const std::size_t index) const noexcept -> bool {
    return index < _valueList.size();
}


auto ValueMap::hasValue(const Name &name) const noexcept -> bool {
    return _valueMap.contains(name);
}


auto ValueMap::hasValue(const NamePath &namePath) const noexcept -> bool {
    if (namePath.empty()) {
        return false;
    }
    auto currentValue = value(namePath.at(0));
    for (std::size_t i = 1; i < namePath.size(); ++i) {
        currentValue = std::dynamic_pointer_cast<Value>(currentValue->value(namePath.at(i)));
        if (currentValue == nullptr) {
            return false;
        }
    }
    return true;
}


auto ValueMap::value(const std::size_t index) const noexcept -> ValuePtr {
    if (_valueList.size() <= index) {
        return {};
    }
    return _valueList[index];
}


auto ValueMap::value(const Name &name) const noexcept -> ValuePtr {
    auto it = _valueMap.find(name);
    if (it == _valueMap.end()) {
        return {};
    }
    return it->second;
}


auto ValueMap::value(const NamePath &namePath) const -> ValuePtr {
    if (namePath.empty()) {
        return {};
    }
    auto currentValue = value(namePath.at(0));
    for (std::size_t i = 1; i < namePath.size(); ++i) {
        currentValue = std::dynamic_pointer_cast<Value>(currentValue->value(namePath.at(i)));
        if (currentValue == nullptr) {
            return {};
        }
    }
    return currentValue;
}


auto ValueMap::begin() const noexcept -> ValueIterator {
    if (_valueMap.empty()) {
        return {};
    }
    return ValueIterator{_valueList.begin()};
}


auto ValueMap::end() const noexcept -> ValueIterator {
    if (_valueMap.empty()) {
        return {};
    }
    return ValueIterator{_valueList.end()};
}


void ValueMap::addValue(const ValuePtr &value) {
    if (value->name().empty()) {
        value->setName(Name::createIndex(_valueList.size()));
    }
    _valueList.push_back(value);
    _valueMap.insert_or_assign(value->name(), value);
}


}

