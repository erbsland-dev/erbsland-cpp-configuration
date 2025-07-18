// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Container.hpp"
#include "ValueMap.hpp"

#include "../../Document.hpp"

#include "../utf8/U8Format.hpp"


namespace erbsland::conf::impl {


class Document final : public conf::Document, public Container {
public:
    // defaults
    Document() = default;
    ~Document() override = default;

public: // implement Value
    [[nodiscard]] auto name() const noexcept -> Name override {
        return {};
    }
    [[nodiscard]] auto namePath() const noexcept -> NamePath override {
        return {};
    }
    [[nodiscard]] auto hasParent() const noexcept -> bool override {
        return false;
    }
    [[nodiscard]] auto parent() const noexcept -> conf::ValuePtr override {
        return {};
    }
    [[nodiscard]] auto type() const noexcept -> ValueType override {
        return ValueType::Document;
    }
    [[nodiscard]] auto hasLocation() const noexcept -> bool override {
        return !_location.isUndefined();
    }
    [[nodiscard]] auto location() const noexcept -> Location override {
        return _location;
    }
    void setLocation(const Location &newLocation) noexcept override {
        _location = newLocation;
    }
    [[nodiscard]] auto size() const noexcept -> std::size_t override {
        return _children.size();
    }
    [[nodiscard]] auto hasValue(std::size_t index) const noexcept -> bool override {
        return _children.hasValue(index);
    }
    [[nodiscard]] auto hasValue(const Name &name) const noexcept -> bool override {
        return _children.hasValue(name);
    }
    [[nodiscard]] auto hasValue(const NamePath &namePath) const noexcept -> bool override {
        return _children.hasValue(namePath);
    }
    [[nodiscard]] auto value(const NamePath &namePath) const noexcept -> conf::ValuePtr override {
        return _children.value(namePath);
    }
    [[nodiscard]] auto value(std::size_t index) const noexcept -> conf::ValuePtr override {
        return _children.value(index);
    }
    [[nodiscard]] auto value(const Name &name) const noexcept -> conf::ValuePtr override {
        return _children.value(name);
    }
    [[nodiscard]] auto begin() const noexcept -> ValueIterator override {
        return _children.begin();
    }
    [[nodiscard]] auto end() const noexcept -> ValueIterator override {
        return _children.end();
    }
    [[nodiscard]] auto toInteger() const noexcept -> Integer override {
        return {};
    }
    [[nodiscard]] auto toBoolean() const noexcept -> bool override {
        return false;
    }
    [[nodiscard]] auto toFloat() const noexcept -> Float override {
        return {};
    }
    [[nodiscard]] auto toText() const noexcept -> String override {
        return {};
    }
    [[nodiscard]] auto toDate() const noexcept -> Date override {
        return {};
    }
    [[nodiscard]] auto toTime() const noexcept -> Time override {
        return {};
    }
    [[nodiscard]] auto toDateTime() const noexcept -> DateTime override {
        return {};
    }
    [[nodiscard]] auto toBytes() const noexcept -> Bytes override {
        return {};
    }
    [[nodiscard]] auto toTimeDelta() const noexcept -> TimeDelta override {
        return {};
    }
    [[nodiscard]] auto toRegEx() const noexcept -> String override {
        return {};
    }
    [[nodiscard]] auto toList() const noexcept -> conf::ValueList override {
        return {};
    }
    [[nodiscard]] auto toTextRepresentation() const noexcept -> String override {
        return {};
    }
    [[nodiscard]] auto toTestText() const noexcept -> String override {
        return u8format(u8"{}()", type());
    }

public: // implement `Document`
    [[nodiscard]] auto toFlatValueMap() const noexcept -> FlatValueMap override;

public: // implement `Container`
    void setParent(const conf::ValuePtr &parent) override {
        throw std::logic_error("The document must not have a parent.");
    }

    void addValue(const ValuePtr &childValue) override {
        _children.addValue(childValue);
    }

private:
    Location _location; ///< The location of the document.
    ValueMap _children; ///< The map with the child values.
};


}

