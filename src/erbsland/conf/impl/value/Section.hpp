// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ValueWithChildren.hpp"

#include <stdexcept>


namespace erbsland::conf::impl {


/// A generic class for all section-like containers.
///
/// @tested `ValueTest`
///
class Section : public ValueWithChildren {
public:
    explicit Section(const ValueType valueType) : _valueType{valueType} {}

public:
    [[nodiscard]] auto type() const noexcept -> ValueType override { return _valueType; };

    void transform(const ValueType targetType) override {
        if (_valueType == ValueType::IntermediateSection) {
            if (targetType != ValueType::SectionWithNames && targetType != ValueType::SectionWithTexts) {
                throw std::logic_error{"Cannot convert intermediate section into the chosen type."};
            }
            _valueType = targetType;
        } else if (_valueType == ValueType::SectionWithNames) {
            if (targetType != ValueType::SectionWithTexts) {
                throw std::logic_error{"Cannot convert section with names into the chosen type."};
            }
            _valueType = targetType;
        } else {
            throw std::logic_error{"Cannot convert section into the chosen type."};
        }
    }

private:
    ValueType _valueType;
};


/// The value implementation for the section with regular names.
///
/// @tested `ValueTest`
///
class SectionWithNames final : public Section {
public:
    SectionWithNames() : Section{ValueType::SectionWithNames} {}
};

/// The value implementation for the section with text names.
///
/// @tested `ValueTest`
///
class SectionWithTexts final : public Section {
public:
    SectionWithTexts() : Section{ValueType::SectionWithTexts} {}
};

/// The value implementation for an intermediate section.
///
/// @tested `ValueTest`
///
class IntermediateSection final : public Section {
public:
    IntermediateSection() : Section{ValueType::IntermediateSection} {}
};


}

