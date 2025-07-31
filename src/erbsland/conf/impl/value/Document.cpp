// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Document.hpp"


#include "Section.hpp"
#include "Value.hpp"

#include <stack>


namespace erbsland::conf::impl {


auto Document::hasLocation() const noexcept -> bool {
    return !_location.isUndefined();
}


auto Document::location() const noexcept -> Location {
    return _location;
}


void Document::setLocation(const Location &newLocation) noexcept {
    _location = newLocation;
}


auto Document::size() const noexcept -> std::size_t {
    return _children.size();
}


auto Document::hasValue(const NamePathLike &namePath) const noexcept -> bool {
    return _children.hasValue(namePath);
}


auto Document::value(const NamePathLike &namePath) const noexcept -> conf::ValuePtr {
    return _children.value(namePath);
}


auto Document::valueOrThrow(const NamePathLike &namePath) const -> conf::ValuePtr {
    return _children.valueOrThrow(namePath, *this);
}


auto Document::begin() const noexcept -> ValueIterator {
    return _children.begin();
}


auto Document::end() const noexcept -> ValueIterator {
    return _children.end();
}


auto Document::toFlatValueMap() const noexcept -> FlatValueMap {
    std::stack<conf::ConstValuePtr> stack;
    auto thisDocument = shared_from_this();
    stack.push(thisDocument);
    FlatValueMap result;
    while (!stack.empty()) {
        auto value = stack.top();
        stack.pop();
        if (!value->empty()) {
            for (const auto &childValue : *value) {
                stack.push(childValue);
            }
        }
        if (value != thisDocument) { // do not add the document root.
            result.emplace(value->namePath(), value);
        }
    }
    return result;
}


void Document::setParent(const conf::ValuePtr &parent) {
    throw std::logic_error("The document must not have a parent.");
}


void Document::addValue(const ValuePtr &childValue) {
    _children.addValue(childValue);
}


}


