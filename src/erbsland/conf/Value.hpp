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
#include "RegEx.hpp"
#include "TestFormat.hpp"
#include "Time.hpp"
#include "TimeDelta.hpp"
#include "ValueIterator.hpp"
#include "ValueType.hpp"

#include "impl/SaturationMath.hpp"
#include "impl/utf8/U8Format.hpp"


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
    /// Default destructor.
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

    /// Test if there is a child-value with the given index, name or name-path.
    ///
    /// @param namePath A name-path, name or index.
    /// @return `true` if there is a value (and the name-path is valid).
    [[nodiscard]] virtual auto hasValue(const NamePathLike &namePath) const noexcept -> bool = 0;

    /// Get the child-value at the specified index, name or name-path.
    ///
    /// If no value is found at the given location, or the name-path contains syntax errors, the method
    /// returns a `nullptr`.
    ///
    /// @param namePath A name-path, name or index.
    /// @return The child value.
    [[nodiscard]] virtual auto value(const NamePathLike &namePath) const noexcept -> ValuePtr = 0;

    /// Get the child-value at the specified index, name or name-path.
    ///
    /// If no value is found at the given location, or the name-path contains syntax errors,
    /// an `Error` exception is thrown.
    ///
    /// @param namePath A name-path, name or index.
    /// @return The child value.
    /// @throws Error (NotFound, Syntax) if the value does not exist or the name-path contains syntax errors.
    [[nodiscard]] virtual auto valueOrThrow(const NamePathLike &namePath) const -> ValuePtr = 0;

    /// Get an iterator to the first child value.
    ///
    /// @return The value iterator.
    ///
    [[nodiscard]] virtual auto begin() const noexcept -> ValueIterator = 0;

    /// Get an iterator to the end of the child values.
    ///
    /// @return The value iterator.
    ///
    [[nodiscard]] virtual auto end() const noexcept -> ValueIterator = 0;

public: // conversions
    /// @name Access as Typed Value
    ///
    /// These methods return the contained value if it has the requested type. Otherwise, a default-constructed
    /// value of the corresponding type is returned, or in case of the <code>...OrThrow</code> variants,
    /// an `Error` (TypeMismatch) is thrown.
    ///
    /// No type conversion or coercion is performed.
    ///
    /// For example:
    /// - `asInteger()` returns the stored `Integer` if this value is of type `Integer`, or `Integer{}` otherwise.
    /// - `asText()` returns the stored `String` only if this value is of type `Text`, not if it's e.g. an `Integer`.
    ///
    /// To obtain a textual representation of any supported type (e.g. `Integer → "42"`), use `toTextRepresentation()`.
    ///
    /// @{

    /// Access as integer.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asInteger() const noexcept -> Integer = 0;
    /// Access as integer.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asIntegerOrThrow() const -> Integer = 0;
    /// Access as boolean.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asBoolean() const noexcept -> bool = 0;
    /// Access as boolean.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asBooleanOrThrow() const -> bool = 0;
    /// Access as a floating-point value.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asFloat() const noexcept -> Float = 0;
    /// Access as a floating-point value.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asFloatOrThrow() const -> Float = 0;
    /// Access as text.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asText() const noexcept -> String = 0;
    /// Access as text.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asTextOrThrow() const -> String = 0;
    /// Access as a `Date` instance.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asDate() const noexcept -> Date = 0;
    /// Access as a `Date` instance.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asDateOrThrow() const -> Date = 0;
    /// Access as a `Time` instance.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asTime() const noexcept -> Time = 0;
    /// Access as a `Time` instance.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asTimeOrThrow() const -> Time = 0;
    /// Access as a `DateTime` instance.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asDateTime() const noexcept -> DateTime = 0;
    /// Access as a `DateTime` instance.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asDateTimeOrThrow() const -> DateTime = 0;
    /// Access as a `Bytes` array.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asBytes() const noexcept -> Bytes = 0;
    /// Access as a `Bytes` array.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asBytesOrThrow() const -> Bytes = 0;
    /// Access as a `TimeDelta` instance.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asTimeDelta() const noexcept -> TimeDelta = 0;
    /// Access as a `TimeDelta` instance.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asTimeDeltaOrThrow() const -> TimeDelta = 0;
    /// Access as a regular expression text.
    /// @return The value of the requested type, or if this value has a different type, a default value.
    [[nodiscard]] virtual auto asRegEx() const noexcept -> RegEx = 0;
    /// Access as a regular expression text.
    /// @return The value of the requested type.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asRegExOrThrow() const -> RegEx = 0;
    /// Access as a value list.
    /// @important This call does *not* convert a single value into a list with one element.
    ///     Use the `getList()` methods if you like to have this behavior.
    /// @return A value list, or an empty list if this is no value list.
    [[nodiscard]] virtual auto asValueList() const noexcept -> ValueList = 0;
    /// Access as a value list.
    /// @important This call does *not* convert a single value into a list with one element.
    ///     Use the `getList()` methods if you like to have this behavior.
    /// @return A value list.
    /// @throws Error (TypeMismatch) if the value is of another type.
    [[nodiscard]] virtual auto asValueListOrThrow() const -> ValueList = 0;

    /// This is a convenience method, accessed this value as one of the supported types. It is implemented
    /// calling the various `to<Type>()` methods.
    ///
    /// @important
    /// - There are overloads for all integers, converting the signed 64-bit integer into the desired type.
    /// - There are overloads for all float types, converting a `double` into a `float` if necessary.
    /// - If the value exceeds the range of the target type:
    ///   - for `asType()` saturation logic is used - returning the max/min possible value for the chosen type.
    ///   - for `asTypeOrThrow()` a `TypeMismatch` exception is thrown.
    /// - The overload for `ValueList` works exactly like `asValueList()` and therefore *does not* convert
    ///   a single value into a list with one element. Use the `getList()` methods for this behavior.
    ///
    /// @tparam T The type to access this value as.
    /// @return The value or a default value.
    ///
    template<typename T>
    [[nodiscard]] auto asType() const noexcept -> T {
        static_assert(
            ValueType::from<T>().raw() != ValueType::Undefined,
            "no type support available for the specified template argument.");
        return {};
    }
    /// @overload
    template<typename T>
    [[nodiscard]] auto asTypeOrThrow() const -> T {
        static_assert(
            ValueType::from<T>().raw() != ValueType::Undefined,
            "no type support available for the specified template argument.");
        throw Error(ErrorCategory::TypeMismatch, "asTypeOrThrow() does not support the given type.");
    }

    /// @private
    template<typename T> requires std::is_integral_v<T>
    [[nodiscard]] auto asType() const noexcept -> T {
        return impl::saturatingCast<T, Integer>(asInteger());
    }

    /// @private
    template<typename T> requires std::is_integral_v<T>
    [[nodiscard]] auto asTypeOrThrow() const -> T {
        const auto value = asIntegerOrThrow();
        if (impl::willSaturatingCastOverflow<T, Integer>(asInteger())) {
            throw Error(ErrorCategory::TypeMismatch, u8"The value exceeds the expected range.", namePath(), location());
        }
        return static_cast<T>(value);
    }

    /// @private
    template<typename T> requires std::is_floating_point_v<T>
    [[nodiscard]] auto asType() const noexcept -> T {
        const auto value = asFloat();
        if constexpr (sizeof(T) < sizeof(Float)) {
            if (!std::isfinite(value)) {
                return static_cast<T>(value);
            }
            const auto max = static_cast<Float>(std::numeric_limits<T>::max());
            const auto lowest = static_cast<Float>(std::numeric_limits<T>::lowest());
            if (value > max) {
                return std::numeric_limits<T>::max();
            }
            if (value < lowest) {
                return std::numeric_limits<T>::lowest();
            }
        }
        return static_cast<T>(value);
    }

    /// @private
    template<typename T> requires std::is_floating_point_v<T>
    [[nodiscard]] auto asTypeOrThrow() const -> T {
        const auto value = asFloatOrThrow();
        if constexpr (sizeof(T) < sizeof(Float)) {
            if (!std::isfinite(value)) {
                return static_cast<T>(value);
            }
            const auto max = static_cast<Float>(std::numeric_limits<T>::max());
            const auto lowest = static_cast<Float>(std::numeric_limits<T>::lowest());
            if (value > max || value < lowest) {
                throw Error(ErrorCategory::TypeMismatch,
                    u8"The value exceeds the expected range.", namePath(), location());
            }
        }
        return static_cast<T>(value);
    }
    /// @}

    /// @name Get as Uniform Value Lists
    ///
    /// - Tries to get this value as uniform lists that consist of values of the same type.
    /// - If this is a single value that matches the type, a list with one element is returned.
    ///
    /// @{

    /// @return A list with values of this type, or an empty list on any problem.
    template<typename T>
    [[nodiscard]] auto asList() const noexcept -> std::vector<T>;

    /// @return A list with values of this type.
    /// @throws Error In case of any type mismatch or syntax error in the name path.
    template<typename T>
    [[nodiscard]] auto asListOrThrow() const -> std::vector<T>;
    /// @}

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
    /// The general format is <code>&lt;Type&gt;(&lt;value&gt;)</code>, where <code>&lt;Type&gt;</code> is one of the
    /// standardized type names and <code>&lt;value&gt;</code> the value representation as specified.
    /// For example, an integer value 5, is converted into the text <code>Integer(5)</code>.
    /// No additional info is added to sections.
    ///
    /// @param format The format of the output.
    /// @return The value in its test outcome representation.
    ///
    [[nodiscard]] auto toTestText(TestFormat format = {}) const noexcept -> String;

    /// Convert this value into a visual value tree.
    ///
    /// This method is useful for testing to get a visual representation of a parsed document, or
    /// a branch of the document.
    ///
    /// @param format The format of the output.
    /// @return A text with a visual tree representation of this value.
    ///
    [[nodiscard]] auto toTestValueTree(TestFormat format = {}) const noexcept -> String;

public: // Convenience methods.
    /// Test if this container is empty.
    ///
    /// @return `true` if the container is empty.
    ///
    [[nodiscard]] auto empty() const noexcept -> bool;

    /// Get the first value of a container.
    ///
    /// @return The first value or a `nullptr`.
    ///
    [[nodiscard]] auto firstValue() const noexcept -> ValuePtr;

    /// Get the last value of a container.
    ///
    /// @return The last value or a `nullptr`.
    ///
    [[nodiscard]] auto lastValue() const noexcept -> ValuePtr;

    /// @name Get a Value of a Given Type
    ///
    /// Tries to get a value at the given name-path with a given type.
    ///
    /// - If the name path is not valid,
    /// - or if there is no value at the name-path
    /// - or the value does not have the expected type,
    /// - a default value passed as `defaultValue` parameter is returned.
    /// - ... or, an exception is thrown for the <code>...OrThrow</code> methods.
    ///
    /// If types are converted, the same logic as described in `asType()` or `asTypeOrThrow()` applies.
    ///
    /// @{

    /// @param namePath The name path, name or index to resolve, relative to this value.
    /// @param defaultValue The default value returned if the value can't be resolved.
    /// @return the requested value or `defaultValue`.
    template<typename T>
    [[nodiscard]] auto get(const NamePathLike &namePath, T defaultValue = {}) const noexcept -> T {
        return {};
    }
    /// @private
    template<typename T> requires std::is_integral_v<T>
    [[nodiscard]] auto get(const NamePathLike &namePath, T defaultValue = {}) const noexcept -> T {
        return static_cast<T>(getInteger(namePath, static_cast<Integer>(defaultValue)));
    }
    /// @private
    template<typename T> requires std::is_floating_point_v<T>
    [[nodiscard]] auto get(const NamePathLike &namePath, T defaultValue = {}) const noexcept -> T {
        return static_cast<T>(getFloat(namePath, static_cast<Float>(defaultValue)));
    }
    /// @param namePath The name path, name or index to resolve, relative to this value.
    /// @throws Error If the value does not exist or has the wrong type.
    /// @return The requested value.
    template<typename T>
    [[nodiscard]] auto getOrThrow(const NamePathLike &namePath) const -> T {
        return std::logic_error("getOrThrow() not implemented for the given type.");
    }
    /// @private
    template<typename T> requires std::is_integral_v<T>
    [[nodiscard]] auto getOrThrow(const NamePathLike &namePath) const -> T {
        return static_cast<T>(getIntegerOrThrow(namePath));
    }
    /// @private
    template<typename T> requires std::is_floating_point_v<T>
    [[nodiscard]] auto getOrThrow(const NamePathLike &namePath) const -> T {
        return static_cast<T>(getFloatOrThrow(namePath));
    }
    /// @param namePath The name path, name or index to resolve, relative to this value.
    /// @param defaultValue The default value returned if the value can't be resolved.
    /// @return The value or `defaultValue` if there is no matching value at `namePath`.
    [[nodiscard]] auto getInteger(const NamePathLike &namePath, Integer defaultValue = {}) const noexcept -> Integer;
    /// @param namePath The name path, name or index to resolve, relative to this value.
    /// @return The requested value.
    /// @throws Error If the value does not exist or has the wrong type.
    [[nodiscard]] auto getIntegerOrThrow(const NamePathLike &namePath) const -> Integer;
    /// @copydoc getInteger
    [[nodiscard]] auto getBoolean(const NamePathLike &namePath, bool defaultValue = false) const noexcept -> bool;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getBooleanOrThrow(const NamePathLike &namePath) const -> bool;
    /// @copydoc getInteger
    [[nodiscard]] auto getFloat(const NamePathLike &namePath, Float defaultValue = {}) const noexcept -> Float;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getFloatOrThrow(const NamePathLike &namePath) const -> Float;
    /// @copydoc getInteger
    [[nodiscard]] auto getText(const NamePathLike &namePath, const String &defaultValue = {}) const noexcept -> String;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getTextOrThrow(const NamePathLike &namePath) const -> String;
    /// @copydoc getInteger
    [[nodiscard]] auto getDate(const NamePathLike &namePath, const Date &defaultValue = {}) const noexcept -> Date;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getDateOrThrow(const NamePathLike &namePath) const -> Date;
    /// @copydoc getInteger
    [[nodiscard]] auto getTime(const NamePathLike &namePath, const Time &defaultValue = {}) const noexcept -> Time;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getTimeOrThrow(const NamePathLike &namePath) const -> Time;
    /// @copydoc getInteger
    [[nodiscard]] auto getDateTime(const NamePathLike &namePath, const DateTime &defaultValue = {}) const noexcept -> DateTime;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getDateTimeOrThrow(const NamePathLike &namePath) const -> DateTime;
    /// @copydoc getInteger
    [[nodiscard]] auto getBytes(const NamePathLike &namePath, const Bytes &defaultValue = {}) const noexcept -> Bytes;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getBytesOrThrow(const NamePathLike &namePath) const -> Bytes;
    /// @copydoc getInteger
    [[nodiscard]] auto getTimeDelta(const NamePathLike &namePath, const TimeDelta &defaultValue = {}) const noexcept -> TimeDelta;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getTimeDeltaOrThrow(const NamePathLike &namePath) const -> TimeDelta;
    /// @copydoc getInteger
    [[nodiscard]] auto getRegEx(const NamePathLike &namePath, const RegEx &defaultValue = {}) const noexcept -> RegEx;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getRegExOrThrow(const NamePathLike &namePath) const -> RegEx;
    /// @param namePath The name path, name or index to resolve, relative to this value.
    /// @return The value list or an empty list if there is no matching value at `namePath`.
    [[nodiscard]] auto getValueList(const NamePathLike &namePath) const noexcept -> ValueList;
    /// @copydoc getIntegerOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getValueListOrThrow(const NamePathLike &namePath) const -> ValueList;
    /// @}

    /// @name Get Uniform Value Lists
    ///
    /// - Tries to get uniform lists that consist of values of the same type.
    /// - If there is a single value at the name path, a list with one element is returned.
    ///
    /// @{

    /// @param namePath The name-path, name or index of the value list.
    /// @return A list with values of this type, or an empty list on any problem.
    template<typename T>
    [[nodiscard]] auto getList(const NamePathLike &namePath) const noexcept -> std::vector<T>;

    /// @param namePath The name-path, name or index of the value list.
    /// @return A list with values of this type.
    /// @throws Error In case of any type mismatch or syntax error in the name path.
    template<typename T>
    [[nodiscard]] auto getListOrThrow(const NamePathLike &namePath) const -> std::vector<T>;
    /// @}

    /// @name Get a Section-Map or Section-List
    ///
    /// Tries to get a section map or section list at the given path. If the path does not exist (or contains
    /// syntax errors), either a nullptr is returned, or an exception is thrown (`...OrThrow()` methods).
    ///
    /// @{

    /// @param namePath The name path, name or index to resolve, relative to this value.
    /// @return The section map/list or nullptr.
    [[nodiscard]] auto getSectionWithNames(const NamePathLike &namePath) const noexcept -> ValuePtr;
    /// @param namePath The name path, name or index to resolve, relative to this value.
    /// @return The section map or section list.
    /// @throws Error if the name path has syntax errors, or if there is no matching section at the value-path.
    [[nodiscard]] auto getSectionWithNamesOrThrow(const NamePathLike &namePath) const -> ValuePtr;
    /// @copydoc getSectionWithNames(const NamePathLike&) const
    [[nodiscard]] auto getSectionWithTexts(const NamePathLike &namePath) const noexcept -> ValuePtr;
    /// @copydoc getSectionWithNamesOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getSectionWithTextsOrThrow(const NamePathLike &namePath) const -> ValuePtr;
    /// @copydoc getSectionWithNames(const NamePathLike&) const
    [[nodiscard]] auto getSectionList(const NamePathLike &namePath) const noexcept -> ValuePtr;
    /// @copydoc getSectionWithNamesOrThrow(const NamePathLike&) const
    [[nodiscard]] auto getSectionListOrThrow(const NamePathLike &namePath) const -> ValuePtr;
    /// @}

    /// @name Tests for a Value Type
    ///
    /// Test if a value is of a certain type.
    ///
    /// @{

    /// @return `true` if the value has the tested type.
    [[nodiscard]] auto isInteger() const noexcept -> bool { return type() == ValueType::Integer; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isBoolean() const noexcept -> bool { return type() == ValueType::Boolean; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isFloat() const noexcept -> bool { return type() == ValueType::Float; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isText() const noexcept -> bool { return type() == ValueType::Text; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isDate() const noexcept -> bool { return type() == ValueType::Date; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isTime() const noexcept -> bool { return type() == ValueType::Time; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isDateTime() const noexcept -> bool { return type() == ValueType::DateTime; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isBytes() const noexcept -> bool { return type() == ValueType::Bytes; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isTimeDelta() const noexcept -> bool { return type() == ValueType::TimeDelta; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isRegEx() const noexcept -> bool { return type() == ValueType::RegEx; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isValueList() const noexcept -> bool { return type() == ValueType::ValueList; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isDocument() const noexcept -> bool { return type() == ValueType::Document; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isRoot() const noexcept -> bool { return type() == ValueType::Document; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isSectionWithNames() const noexcept -> bool { return type() == ValueType::SectionWithNames; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isSectionWithTexts() const noexcept -> bool { return type() == ValueType::SectionWithTexts; }
    /// @copydoc isInteger() const
    [[nodiscard]] auto isSectionList() const noexcept -> bool { return type() == ValueType::SectionList; }

    /// Test if this value is a list.
    ///
    /// Tests if this value is a list, like a section list or value list with child-elements that can be
    /// iterated in a sequence.
    ///
    /// @return `true` if this value is a list.
    ///
    [[nodiscard]] auto isList() const noexcept -> bool { return type().isList(); }

    /// Test if this value is a name-value map.
    ///
    /// Tests if this value is a name-value map, like a section with names, section with texts,
    /// intermediate section or a document.
    ///
    [[nodiscard]] auto isMap() const noexcept -> bool { return type().isMap(); }
    /// @}
};


template <typename T>
auto Value::asListOrThrow() const -> std::vector<T> {
    constexpr auto expectedType = ValueType::from<T>();
    static_assert(
        expectedType.raw() != ValueType::Undefined,
        "no type support available for the specified template argument.");
    if (type() == expectedType) { // convert a single value into a list.
        return std::vector<T>{{asType<T>()}};
    }
    if (type() != ValueType::ValueList) {
        throw Error(
            ErrorCategory::TypeMismatch,
            impl::u8format(
                u8"Expected a list of '{}' values, but got a single value of type '{}'.",
                expectedType,
                type()),
            namePath(),
            location());
    }
    const auto valueList = asValueList();
    std::vector<T> result;
    result.reserve(valueList.size());
    for (const auto &value : valueList) {
        if (value->type() != expectedType) {
            throw Error(
                ErrorCategory::TypeMismatch,
                impl::u8format(
                    u8"Expected all values in the list to be of type '{}', but found an element of type '{}'.",
                    expectedType,
                    value->type()),
                value->namePath(),
                value->location());
        }
        result.push_back(value->asType<T>());
    }
    return result;
}


template <typename T>
auto Value::getListOrThrow(const NamePathLike &namePath) const -> std::vector<T> {
    auto valueAtPath = valueOrThrow(namePath);
    constexpr auto expectedType = ValueType::from<T>();
    static_assert(
        expectedType.raw() != ValueType::Undefined,
        "no type support available for the specified template argument.");
    return valueAtPath->asListOrThrow<T>();
}


template <typename T>
auto Value::getList(const NamePathLike &namePath) const noexcept -> std::vector<T> {
    try {
        return getListOrThrow<T>(namePath);
    } catch (const Error&) {
        return {};
    }
}


template <typename T>
auto Value::asList() const noexcept -> std::vector<T> {
    try {
        return asListOrThrow<T>();
    } catch (const Error&) {
        return {};
    }
}


template<> [[nodiscard]] inline auto Value::asType<bool>() const noexcept -> bool { return asBoolean(); }
template<> [[nodiscard]] inline auto Value::asType<String>() const noexcept -> String { return asText(); }
template<> [[nodiscard]] inline auto Value::asType<std::u8string>() const noexcept -> std::u8string { return asText().raw(); }
template<> [[nodiscard]] inline auto Value::asType<std::string>() const noexcept -> std::string { return asText().toCharString(); }
template<> [[nodiscard]] inline auto Value::asType<Date>() const noexcept -> Date { return asDate(); }
template<> [[nodiscard]] inline auto Value::asType<Time>() const noexcept -> Time { return asTime(); }
template<> [[nodiscard]] inline auto Value::asType<DateTime>() const noexcept -> DateTime { return asDateTime(); }
template<> [[nodiscard]] inline auto Value::asType<TimeDelta>() const noexcept -> TimeDelta { return asTimeDelta(); }
template<> [[nodiscard]] inline auto Value::asType<Bytes>() const noexcept -> Bytes { return asBytes(); }
template<> [[nodiscard]] inline auto Value::asType<RegEx>() const noexcept -> RegEx { return asRegEx(); }
template<> [[nodiscard]] inline auto Value::asType<ValueList>() const noexcept -> ValueList { return asValueList(); }

template<> [[nodiscard]] inline auto Value::asTypeOrThrow<bool>() const -> bool { return asBooleanOrThrow(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<String>() const -> String { return asTextOrThrow(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<std::u8string>() const -> std::u8string { return asTextOrThrow().raw(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<std::string>() const -> std::string { return asTextOrThrow().toCharString(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<Date>() const -> Date { return asDateOrThrow(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<Time>() const -> Time { return asTimeOrThrow(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<DateTime>() const -> DateTime { return asDateTimeOrThrow(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<TimeDelta>() const -> TimeDelta { return asTimeDeltaOrThrow(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<Bytes>() const -> Bytes { return asBytesOrThrow(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<RegEx>() const -> RegEx { return asRegExOrThrow(); }
template<> [[nodiscard]] inline auto Value::asTypeOrThrow<ValueList>() const -> ValueList { return asValueListOrThrow(); }

template<> [[nodiscard]] inline auto Value::get<bool>(const NamePathLike &namePath, bool defaultValue) const noexcept -> bool { return getBoolean(namePath, defaultValue); }
template<> [[nodiscard]] inline auto Value::get<String>(const NamePathLike &namePath, String defaultValue) const noexcept -> String { return getText(namePath, defaultValue); }
template<> [[nodiscard]] inline auto Value::get<std::u8string>(const NamePathLike &namePath, std::u8string defaultValue) const noexcept -> std::u8string { return getText(namePath, String{defaultValue}).raw(); }
template<> [[nodiscard]] inline auto Value::get<std::string>(const NamePathLike &namePath, std::string defaultValue) const noexcept -> std::string { return getText(namePath, String{defaultValue}).toCharString(); }
template<> [[nodiscard]] inline auto Value::get<Date>(const NamePathLike &namePath, Date defaultValue) const noexcept -> Date { return getDate(namePath, defaultValue); }
template<> [[nodiscard]] inline auto Value::get<Time>(const NamePathLike &namePath, Time defaultValue) const noexcept -> Time { return getTime(namePath, defaultValue); }
template<> [[nodiscard]] inline auto Value::get<DateTime>(const NamePathLike &namePath, DateTime defaultValue) const noexcept -> DateTime { return getDateTime(namePath, defaultValue); }
template<> [[nodiscard]] inline auto Value::get<TimeDelta>(const NamePathLike &namePath, TimeDelta defaultValue) const noexcept -> TimeDelta { return getTimeDelta(namePath, defaultValue); }
template<> [[nodiscard]] inline auto Value::get<Bytes>(const NamePathLike &namePath, Bytes defaultValue) const noexcept -> Bytes { return getBytes(namePath, defaultValue); }
template<> [[nodiscard]] inline auto Value::get<RegEx>(const NamePathLike &namePath, RegEx defaultValue) const noexcept -> RegEx { return getRegEx(namePath, defaultValue); }

template<> [[nodiscard]] inline auto Value::getOrThrow<bool>(const NamePathLike &namePath) const -> bool { return getBooleanOrThrow(namePath); }
template<> [[nodiscard]] inline auto Value::getOrThrow<String>(const NamePathLike &namePath) const -> String { return getTextOrThrow(namePath); }
template<> [[nodiscard]] inline auto Value::getOrThrow<std::u8string>(const NamePathLike &namePath) const -> std::u8string { return getTextOrThrow(namePath).raw(); }
template<> [[nodiscard]] inline auto Value::getOrThrow<std::string>(const NamePathLike &namePath) const -> std::string { return getTextOrThrow(namePath).toCharString(); }
template<> [[nodiscard]] inline auto Value::getOrThrow<Date>(const NamePathLike &namePath) const -> Date { return getDateOrThrow(namePath); }
template<> [[nodiscard]] inline auto Value::getOrThrow<Time>(const NamePathLike &namePath) const -> Time { return getTimeOrThrow(namePath); }
template<> [[nodiscard]] inline auto Value::getOrThrow<DateTime>(const NamePathLike &namePath) const -> DateTime { return getDateTimeOrThrow(namePath); }
template<> [[nodiscard]] inline auto Value::getOrThrow<TimeDelta>(const NamePathLike &namePath) const -> TimeDelta { return getTimeDeltaOrThrow(namePath); }
template<> [[nodiscard]] inline auto Value::getOrThrow<Bytes>(const NamePathLike &namePath) const -> Bytes { return getBytesOrThrow(namePath); }
template<> [[nodiscard]] inline auto Value::getOrThrow<RegEx>(const NamePathLike &namePath) const -> RegEx { return getRegExOrThrow(namePath); }


}


template <>
struct std::formatter<erbsland::conf::Value> : std::formatter<std::string> {
    auto format(const erbsland::conf::Value &value, format_context& ctx) const {
        return std::formatter<std::string>::format(value.toTextRepresentation().toCharString(), ctx);
    }
};
