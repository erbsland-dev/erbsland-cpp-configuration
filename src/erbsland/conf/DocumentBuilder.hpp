// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "impl/value/DocumentBuilder.hpp"
#include "impl/value/ValueMap.hpp"


namespace erbsland::conf {


/// Builds Configuration Documents Programmatically
///
/// The document builder allows building the value trees of configuration documents programmatically.
/// It expects a logical sequence of sections and values and raises exceptions on name collisions.
///
/// <b>Details:</b>
///
/// - The correct document syntax is fully checked when adding values. If the resulting document would become
///   erroneous, an `Error` (`Syntax`) is thrown.
/// - Values can only be added to existing sections.
/// - If you use a single name, when adding a value, it is automatically added to the last section.
/// - If you use more than one name in the name path, it is added to the specified section.
/// - When creating sections, this builder automatically creates intermediate sections and converts existing
///   ones into section maps.
/// - Name paths can be specified as text `Name` or `NamePath` objects.
///
/// <b>Limitations:</b>
///
/// - You must not use indexes or text-indexes in name paths to access specific elements in lists.
/// - This builder interface does not support adding locations to the elements.
///
/// <b>Example usage:</b>
///
/// @code
/// auto buildDocument() -> DocumentPtr {
///     DocumentBuilder builder;
///     builder.addSectionMap(u8"main");
///     builder.addValue(u8"value", String{"hello"});
///     return builder.getDocumentAndReset();
/// }
/// @endcode
///
class DocumentBuilder {
public:
    /// Default constructor.
    DocumentBuilder() = default;
    /// Default destructor.
    ~DocumentBuilder() = default;

    // disable copy and assign.
    DocumentBuilder(const DocumentBuilder&) = delete;
    auto operator=(const DocumentBuilder&) -> DocumentBuilder& = delete;
    DocumentBuilder(DocumentBuilder&&) = delete;
    auto operator=(DocumentBuilder&&) -> DocumentBuilder& = delete;

public:
    /// Add a section map with the given name path to the document.
    ///
    /// @param namePath The name path of the element.
    ///
    void addSectionMap(const NamePathLike &namePath) {
        _builder.addSectionMap(namePath);
    }

    /// Add a section list with the given name path to the document.
    ///
    /// @param namePath The name path of the element.
    ///
    void addSectionList(const NamePathLike &namePath) {
        _builder.addSectionList(namePath);
    }

    /// Add a value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The value for the new element.
    ///
    template<typename T>
    void addValue(const NamePathLike &namePath, const T &value) {
        _builder.addValueT(namePath, value);
    }

    /// Add an integer value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The integer value to add.
    void addInteger(const NamePathLike &namePath, const Integer value) {
        addValue<Integer>(namePath, value);
    }

    /// Add a boolean value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The boolean value to add.
    void addBoolean(const NamePathLike &namePath, const bool value) {
        addValue<bool>(namePath, value);
    }

    /// Add a float value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The floating-point value to add.
    void addFloat(const NamePathLike &namePath, const Float value) {
        addValue<Float>(namePath, value);
    }

    /// Add a text value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The text value to add.
    void addText(const NamePathLike &namePath, const String &value) {
        addValue<String>(namePath, value);
    }

    /// Add a date value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The date value to add.
    void addDate(const NamePathLike &namePath, const Date &value) {
        addValue<Date>(namePath, value);
    }

    /// Add a time value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The time value to add.
    void addTime(const NamePathLike &namePath, const Time &value) {
        addValue<Time>(namePath, value);
    }

    /// Add a date-time value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The date-time value to add.
    void addDateTime(const NamePathLike &namePath, const DateTime &value) {
        addValue<DateTime>(namePath, value);
    }

    /// Add a byte array value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The byte array value to add.
    void addBytes(const NamePathLike &namePath, const Bytes &value) {
        addValue<Bytes>(namePath, value);
    }

    /// Add a time delta value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The time delta value to add.
    void addTimeDelta(const NamePathLike &namePath, const TimeDelta &value) {
        addValue<TimeDelta>(namePath, value);
    }

    /// Add a regular expression value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The regular expression to add.
    void addRegEx(const NamePathLike &namePath, const RegEx &value) {
        addValue<RegEx>(namePath, value);
    }

    /// Reset the builder and discard the current document.
    ///
    /// This will reset the builder into its initial state and discard any document that is currently being built.
    ///
    void reset() {
        _builder.reset();
    }

    /// Get the document and reset the builder.
    ///
    /// This will finalize and return the currently built document and reset the builder into its initial state.
    ///
    /// @return The built document.
    ///
    [[nodiscard]] auto getDocumentAndReset() -> DocumentPtr {
        return _builder.getDocumentAndReset();
    }

private:
    impl::DocumentBuilder _builder;
};



}
