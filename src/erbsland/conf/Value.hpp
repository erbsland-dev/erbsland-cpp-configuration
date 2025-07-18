// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Bytes.hpp"
#include "Date.hpp"
#include "DateTime.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "Location.hpp"
#include "NamePath.hpp"
#include "Time.hpp"
#include "TimeDelta.hpp"
#include "ValueIterator.hpp"
#include "ValueType.hpp"


namespace erbsland::conf {


class Value;
using ConstValuePtr = std::shared_ptr<const Value>;
using ValueList = std::vector<ConstValuePtr>;


/// The base class and interface for all values.
///
/// @tested `ValueTest`
///
class Value : public std::enable_shared_from_this<Value> {
public:
    virtual ~Value() = default;

public: // basic properties.
    /// The name.
    ///
    [[nodiscard]] virtual auto name() const noexcept -> Name = 0;

    /// The name path.
    ///
    [[nodiscard]] virtual auto namePath() const noexcept -> NamePath = 0;

    /// Test if this value has a parent.
    ///
    [[nodiscard]] virtual auto hasParent() const noexcept -> bool = 0;

    /// The parent.
    ///
    [[nodiscard]] virtual auto parent() const noexcept -> ValuePtr = 0;

    /// The type of this value.
    ///
    [[nodiscard]] virtual auto type() const noexcept -> ValueType = 0;

public: // location
    /// Test if this value has location info.
    ///
    [[nodiscard]] virtual auto hasLocation() const noexcept -> bool = 0;

    /// Get the location info for this value.
    ///
    [[nodiscard]] virtual auto location() const noexcept -> Location = 0;

    /// Set the location info for this value.
    ///
    virtual void setLocation(const Location &newLocation) noexcept = 0;

public: // lists
    /// Get the number of children.
    ///
    [[nodiscard]] virtual auto size() const noexcept -> std::size_t = 0;

    /// @{
    /// Test if there is a child-value with the given index, name or name-path.
    ///
    /// @return `true` if there is a value.
    ///
    [[nodiscard]] virtual auto hasValue(std::size_t index) const noexcept -> bool = 0;
    [[nodiscard]] virtual auto hasValue(const Name &name) const noexcept -> bool = 0;
    [[nodiscard]] virtual auto hasValue(const NamePath &namePath) const noexcept -> bool = 0;
    /// @}

    /// @{
    /// Get the child-value at the specified index, name or name-path.
    ///
    /// @return The child value or a nullptr if the index is out of range.
    ///
    [[nodiscard]] virtual auto value(std::size_t index) const noexcept -> ValuePtr = 0;
    [[nodiscard]] virtual auto value(const Name &name) const noexcept -> ValuePtr = 0;
    [[nodiscard]] virtual auto value(const NamePath &namePath) const noexcept -> ValuePtr = 0;
    /// @}

    /// Get an iterator to the first child value.
    ///
    [[nodiscard]] virtual auto begin() const noexcept -> ValueIterator = 0;

    /// Get an iterator to the end of the child values.
    ///
    [[nodiscard]] virtual auto end() const noexcept -> ValueIterator = 0;

public: // conversions
    /// @{
    /// Convert this value to a given type.
    ///
    /// Only converts this type or returns its default value. For example, `toText()` does *not* convert
    /// an `Integer` value into text. Use the `toTextRepresentation()` function for such conversions.
    ///
    /// @return The value of the requested type, or if this value has a different type, a default value.
    ///
    [[nodiscard]] virtual auto toInteger() const noexcept -> Integer = 0;
    [[nodiscard]] virtual auto toBoolean() const noexcept -> bool = 0;
    [[nodiscard]] virtual auto toFloat() const noexcept -> Float = 0;
    [[nodiscard]] virtual auto toText() const noexcept -> String = 0;
    [[nodiscard]] virtual auto toDate() const noexcept -> Date = 0;
    [[nodiscard]] virtual auto toTime() const noexcept -> Time = 0;
    [[nodiscard]] virtual auto toDateTime() const noexcept -> DateTime = 0;
    [[nodiscard]] virtual auto toBytes() const noexcept -> Bytes = 0;
    [[nodiscard]] virtual auto toTimeDelta() const noexcept -> TimeDelta = 0;
    [[nodiscard]] virtual auto toRegEx() const noexcept -> String = 0;
    [[nodiscard]] virtual auto toList() const noexcept -> ValueList = 0;
    /// @}

    /// Convert this value to the given type.
    ///
    /// This is a convenience method, to convert a value to one of the supported types. It is implemented
    /// calling the various `to<Type>` methods.
    ///
    /// @important
    /// - There are overloads for all integers, converting the signed 64-bit integer into the desired type
    ///   with the risk of losing information.
    /// - There are overloads for all float types, converting a `double` into a `float` if necessary
    ///   with the risk of losing information.
    /// - In this implementation, it is not possible to convert regular expression types, as they use `String`
    ///   which only convert `Text` values using `toText()`.
    ///
    /// @tparam T The type to convert this value to.
    /// @return The value or a default value if it cannot be converted into the requested type.
    ///
    template<typename T>
    [[nodiscard]] auto convertTo() const noexcept -> T {
        return {};
    }

    template<typename T> requires std::is_integral_v<T>
    [[nodiscard]] auto convertTo() const noexcept -> T {
        return static_cast<T>(toInteger());
    }

    template<typename T> requires std::is_floating_point_v<T>
    [[nodiscard]] auto convertTo() const noexcept -> T {
        return static_cast<T>(toFloat());
    }

    /// Convert this value to its text representation.
    ///
    /// Converts the types: Text, Integer, Float, Boolean, Date, Time, Date-Time, Bytes, TimeDelta, RegEx.
    /// Sections and lists result in an empty string.
    ///
    /// @return A string with the text or text representation.
    ///
    [[nodiscard]] virtual auto toTextRepresentation() const noexcept -> String = 0;

    /// Convert this value to its test adapter representation.
    ///
    /// This is used by the test adapter to verify the value, as described in the language documentation.
    ///
    /// The general format is `<Type>(<value>)`, where `<Type>` is one of the standardized type names and
    /// `<value>` the value representation as specified. For example, an integer value 5, is converted into
    /// the text `Integer(5)`. No additional info is added to sections.
    ///
    [[nodiscard]] virtual auto toTestText() const noexcept -> String = 0;

public: // Convenience methods.
    /// Test if this container is empty.
    ///
    [[nodiscard]] auto empty() const noexcept -> bool ;

    /// Get the first value of a container.
    ///
    [[nodiscard]] auto firstValue() const noexcept -> ValuePtr ;

    /// Get the last value of a container.
    ///
    [[nodiscard]] auto lastValue() const noexcept -> ValuePtr ;

    /// @{
    /// Get a value and throw if it is not found.
    ///
    /// These functions work like `value()`, but instead of returning a nullptr when the value is not found
    /// they throw an `Error` exception.
    ///
    /// @see value(std::size)
    /// @return The child value.
    /// @throws Error (NotFound) if there is no value at the given index.
    ///
    [[nodiscard]] virtual auto valueOrThrow(std::size_t index) const -> ValuePtr;
    [[nodiscard]] virtual auto valueOrThrow(const Name &name) const -> ValuePtr;
    [[nodiscard]] virtual auto valueOrThrow(const NamePath &namePath) const -> ValuePtr;
    /// @}

    /// @{
    /// Get a value of a given type at the given name path.
    ///
    /// Try to get a value at the given value path with a given type. If there is no value at the name-path or
    /// the value does not have the expected type - a default value passed as `defaultValue` parameter is
    /// returned.
    ///
    /// @param namePath The name path to resolve, relative to this value.
    /// @param defaultValue The default value returned if the value can't be resolved.
    /// @return The value or `defaultValue` if there is no matching value at `namePath`.
    ///
    [[nodiscard]] auto getInteger(const NamePath &namePath, Integer defaultValue = {}) const noexcept -> Integer;
    [[nodiscard]] auto getBoolean(const NamePath &namePath, bool defaultValue = false) const noexcept -> bool;
    [[nodiscard]] auto getFloat(const NamePath &namePath, Float defaultValue = {}) const noexcept -> Float;
    [[nodiscard]] auto getText(const NamePath &namePath, const String &defaultValue = {}) const noexcept -> String;
    [[nodiscard]] auto getDate(const NamePath &namePath, const Date &defaultValue = {}) const noexcept -> Date;
    [[nodiscard]] auto getTime(const NamePath &namePath, const Time &defaultValue = {}) const noexcept -> Time;
    [[nodiscard]] auto getDateTime(const NamePath &namePath, const DateTime &defaultValue = {}) const noexcept -> DateTime;
    [[nodiscard]] auto getBytes(const NamePath &namePath, const Bytes &defaultValue = {}) const noexcept -> Bytes;
    [[nodiscard]] auto getTimeDelta(const NamePath &namePath, const TimeDelta &defaultValue = {}) const noexcept -> TimeDelta;
    [[nodiscard]] auto getRegEx(const NamePath &namePath, const String &defaultValue = {}) const noexcept -> String;
    [[nodiscard]] auto getList(const NamePath &namePath) const noexcept -> ValueList;
    /// @}

    /// @{
    /// Get a value of a given type at the given name path.
    ///
    /// These functions throw an `Error` if no value exists at
    /// `namePath` or if the value does not have the expected type.
    ///
    /// @param namePath The name path to resolve, relative to this value.
    /// @return The requested value.
    ///
    [[nodiscard]] auto getIntegerOrThrow(const NamePath &namePath) const -> Integer;
    [[nodiscard]] auto getBooleanOrThrow(const NamePath &namePath) const -> bool;
    [[nodiscard]] auto getFloatOrThrow(const NamePath &namePath) const -> Float;
    [[nodiscard]] auto getTextOrThrow(const NamePath &namePath) const -> String;
    [[nodiscard]] auto getDateOrThrow(const NamePath &namePath) const -> Date;
    [[nodiscard]] auto getTimeOrThrow(const NamePath &namePath) const -> Time;
    [[nodiscard]] auto getDateTimeOrThrow(const NamePath &namePath) const -> DateTime;
    [[nodiscard]] auto getBytesOrThrow(const NamePath &namePath) const -> Bytes;
    [[nodiscard]] auto getTimeDeltaOrThrow(const NamePath &namePath) const -> TimeDelta;
    [[nodiscard]] auto getRegExOrThrow(const NamePath &namePath) const -> String;
    [[nodiscard]] auto getListOrThrow(const NamePath &namePath) const -> ValueList;
    /// @}

    /// @{
    /// Test if a value is of a certain type.
    ///
    /// @return `true` if the value has the tested type.
    ///
    [[nodiscard]] auto isInteger() const noexcept -> bool { return type() == ValueType::Integer; }
    [[nodiscard]] auto isBoolean() const noexcept -> bool { return type() == ValueType::Boolean; }
    [[nodiscard]] auto isFloat() const noexcept -> bool { return type() == ValueType::Float; }
    [[nodiscard]] auto isText() const noexcept -> bool { return type() == ValueType::Text; }
    [[nodiscard]] auto isDate() const noexcept -> bool { return type() == ValueType::Date; }
    [[nodiscard]] auto isTime() const noexcept -> bool { return type() == ValueType::Time; }
    [[nodiscard]] auto isDateTime() const noexcept -> bool { return type() == ValueType::DateTime; }
    [[nodiscard]] auto isBytes() const noexcept -> bool { return type() == ValueType::Bytes; }
    [[nodiscard]] auto isTimeDelta() const noexcept -> bool { return type() == ValueType::TimeDelta; }
    [[nodiscard]] auto isRegEx() const noexcept -> bool { return type() == ValueType::RegEx; }
    [[nodiscard]] auto isValueList() const noexcept -> bool { return type() == ValueType::ValueList; }
    [[nodiscard]] auto isDocument() const noexcept -> bool { return type() == ValueType::Document; }
    [[nodiscard]] auto isRoot() const noexcept -> bool { return type() == ValueType::Document; }
    [[nodiscard]] auto isList() const noexcept -> bool { return type().isList(); }
    [[nodiscard]] auto isMap() const noexcept -> bool { return type().isMap(); }
    /// @}
};


template<> [[nodiscard]] inline auto Value::convertTo<bool>() const noexcept -> bool { return toBoolean(); }
template<> [[nodiscard]] inline auto Value::convertTo<String>() const noexcept -> String { return toText(); }
template<> [[nodiscard]] inline auto Value::convertTo<Date>() const noexcept -> Date { return toDate(); }
template<> [[nodiscard]] inline auto Value::convertTo<Time>() const noexcept -> Time { return toTime(); }
template<> [[nodiscard]] inline auto Value::convertTo<DateTime>() const noexcept -> DateTime { return toDateTime(); }
template<> [[nodiscard]] inline auto Value::convertTo<TimeDelta>() const noexcept -> TimeDelta { return toTimeDelta(); }
template<> [[nodiscard]] inline auto Value::convertTo<Bytes>() const noexcept -> Bytes { return toBytes(); }
template<> [[nodiscard]] inline auto Value::convertTo<ValueList>() const noexcept -> ValueList { return toList(); }


}


template <>
struct std::formatter<erbsland::conf::Value> : std::formatter<std::string> {
    auto format(const erbsland::conf::Value &value, format_context& ctx) const {
        return std::formatter<std::string>::format(value.toTextRepresentation().toCharString(), ctx);
    }
};

