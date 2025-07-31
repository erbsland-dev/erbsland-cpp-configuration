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
    [[nodiscard]] auto hasLocation() const noexcept -> bool override;
    [[nodiscard]] auto location() const noexcept -> Location override;
    void setLocation(const Location &newLocation) noexcept override;
    [[nodiscard]] auto size() const noexcept -> std::size_t override;
    [[nodiscard]] auto hasValue(const NamePathLike &namePath) const noexcept -> bool override;
    [[nodiscard]] auto value(const NamePathLike &namePath) const noexcept -> conf::ValuePtr override;
    [[nodiscard]] auto valueOrThrow(const NamePathLike &namePath) const -> conf::ValuePtr override;
    [[nodiscard]] auto begin() const noexcept -> ValueIterator override;
    [[nodiscard]] auto end() const noexcept -> ValueIterator override;
    [[nodiscard]] auto asInteger() const noexcept -> Integer override {
        return {};
    }
    [[nodiscard]] auto asBoolean() const noexcept -> bool override {
        return false;
    }
    [[nodiscard]] auto asFloat() const noexcept -> Float override {
        return {};
    }
    [[nodiscard]] auto asText() const noexcept -> String override {
        return {};
    }
    [[nodiscard]] auto asDate() const noexcept -> Date override {
        return {};
    }
    [[nodiscard]] auto asTime() const noexcept -> Time override {
        return {};
    }
    [[nodiscard]] auto asDateTime() const noexcept -> DateTime override {
        return {};
    }
    [[nodiscard]] auto asBytes() const noexcept -> Bytes override {
        return {};
    }
    [[nodiscard]] auto asTimeDelta() const noexcept -> TimeDelta override {
        return {};
    }
    [[nodiscard]] auto asRegEx() const noexcept -> RegEx override {
        return {};
    }
    [[nodiscard]] auto asValueList() const noexcept -> conf::ValueList override {
        return {};
    }

    [[nodiscard]] auto asIntegerOrThrow() const -> Integer override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::Integer);
    }
    [[nodiscard]] auto asBooleanOrThrow() const -> bool override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::Boolean);
    }
    [[nodiscard]] auto asFloatOrThrow() const -> Float override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::Float);
    }
    [[nodiscard]] auto asTextOrThrow() const -> String override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::Text);
    }
    [[nodiscard]] auto asDateOrThrow() const -> Date override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::Date);
    }
    [[nodiscard]] auto asTimeOrThrow() const -> Time override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::Time);
    }
    [[nodiscard]] auto asDateTimeOrThrow() const -> DateTime override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::DateTime);
    }
    [[nodiscard]] auto asBytesOrThrow() const -> Bytes override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::Bytes);
    }
    [[nodiscard]] auto asTimeDeltaOrThrow() const -> TimeDelta override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::TimeDelta);
    }
    [[nodiscard]] auto asRegExOrThrow() const -> RegEx override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::RegEx);
    }
    [[nodiscard]] auto asValueListOrThrow() const -> ValueList override {
        impl::Value::throwAsTypeMismatch(*this, ValueType::ValueList);
    }

    [[nodiscard]] auto toTextRepresentation() const noexcept -> String override {
        return {};
    }

public: // implement `Document`
    [[nodiscard]] auto toFlatValueMap() const noexcept -> FlatValueMap override;

public: // implement `Container`
    void setParent(const conf::ValuePtr &parent) override;
    void addValue(const ValuePtr &childValue) override;

private:
    Location _location; ///< The location of the document.
    ValueMap _children; ///< The map with the child values.
};


}

