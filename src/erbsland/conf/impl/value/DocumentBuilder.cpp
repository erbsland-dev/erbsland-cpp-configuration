// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DocumentBuilder.hpp"


#include "Document.hpp"
#include "Value.hpp"

#include <stdexcept>


namespace erbsland::conf::impl {


void DocumentBuilder::reset() noexcept {
    _storage.reset();
}


void DocumentBuilder::addSectionMap(const NamePath &namePath, const Location &location) {
    if (namePath.empty()) {
        throw Error{
            ErrorCategory::Syntax,
            u8"Can not create section with empty name path.",
            location
        };
    }
    if (namePath.containsIndex()) {
        throw Error{
            ErrorCategory::Syntax,
            u8"Can not create section with an index in the name path.",
            location,
            namePath
        };
    }
    auto [parentValue, value] = _storage.resolveForSection(namePath, location);
    if (value != nullptr) {
        // If there is already a value in place, check if it is an intermediate section.
        if (value->type() != ValueType::IntermediateSection) {
            throw Error{
                ErrorCategory::NameConflict,
                u8"A section or value with the same name already exists. This is a conflict with the new section.",
                location,
                namePath
            };
        }
        // transform the intermediate section into a regular one.
        value->transform(ValueType::SectionWithNames);
        value->setLocation(location); // update its location.
    } else {
        // there is no existing element
        value = Value::createSectionWithNames();
        value->setName(namePath.back());
        _storage.addChildValue(parentValue, namePath, location, value);
    }
    _storage.updateLastSection(value, namePath);
}


void DocumentBuilder::addSectionList(const NamePath &namePath, const Location &location) {
    if (namePath.empty()) {
        throw Error{
            ErrorCategory::Syntax,
            u8"Can not create section list with empty name path.",
            location
        };
    }
    if (namePath.containsIndex()) {
        throw Error{
            ErrorCategory::Syntax,
            u8"Can not create section list with an index in the name path.",
            location,
            namePath
        };
    }
    if (namePath.back().isText()) {
        throw Error{
            ErrorCategory::Syntax,
            u8"Can not create section list with a text name.",
            location,
            namePath
        };
    }
    auto [parentValue, value] = _storage.resolveForSection(namePath, location);
    if (value != nullptr) {
        // Only an existing list section is accepted.
        if (value->type() != ValueType::SectionList) {
            throw Error{
                ErrorCategory::NameConflict,
                u8"A section map or value with the same name already exists. This is a conflict with the new section list.",
                location,
                namePath
            };
        }
        // Add a new element to the existing section list.
        parentValue = value;
        value = Value::createSectionWithNames();
        _storage.addChildValue(parentValue, namePath, location, value);
    } else {
        // there is no existing element, create a new list with one element.
        value = Value::createSectionList();
        value->setName(namePath.back());
        _storage.addChildValue(parentValue, namePath, location, value);
        parentValue = value;
        value = Value::createSectionWithNames();
        _storage.addChildValue(parentValue, namePath, location, value);
    }
    _storage.updateLastSection(value, namePath);
}


void DocumentBuilder::addValue(
    const NamePath &namePath,
    const ValuePtr &value,
    const Location &location) {

    if (value == nullptr) {
        throw std::invalid_argument{"value must not be null."};
    }
    if (value->type().isUndefined()) {
        throw std::logic_error{"Can not add an undefined value."};
    }
    if (value->type().isMap() || value->type() == ValueType::SectionList) {
        throw std::logic_error{"Use the 'addSection...' methods for adding containers."};
    }
    const auto sectionValue = _storage.resolveForValue(namePath, location);
    if (value->name().empty()) {
        value->setName(namePath.back());
    }
    if (sectionValue != nullptr && sectionValue->hasValue(value->name())) {
        throw Error{
            ErrorCategory::NameConflict,
            u8"A value with the same name already exists.",
            location,
            namePath
        };
    }
    _storage.addChildValue(sectionValue, namePath, location, value);
}


auto DocumentBuilder::getDocumentAndReset() noexcept -> std::shared_ptr<Document> {
    return _storage.getDocumentAndReset();
}


}

