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
/// Details:
/// - The correct document syntax is fully checked when adding values. If the resulting document would get
///   erroneous, an `Error` (Syntax) is thrown.
/// - Values can only be added to existing sections.
/// - If you use a single name, when adding a value, it is automatically added to the last section.
/// - If you use more than one name in the name path, it is added to the specified section.
/// - When creating sections, this builder automatically creates immediate sections and convert existing
///   ones into section maps.
/// - Name paths can be specified using text, or as `NamePath` objects.
///
/// Limitations:
/// - You must not use indexes or text-indexes in name paths to access specific elements in lists.
/// - Regular expression values can only be added using `addRegEx()`, there is no `addValue()` overload.
/// - This builder interface does not support adding locations to the elements.
///
/// Example usage:
/// <code language="cpp">
/// auto buildDocument() -> DocumentPtr {
///     DocumentBuilder builder;
///     builder.addSectionMap(u8"main");
///     builder.addValue(u8"value", String{"hello"});
///     return builder.getDocumentAndReset();
/// }
/// </code>
///
class DocumentBuilder {
public:
    // defaults
    DocumentBuilder() = default;
    ~DocumentBuilder() = default;

    // disable copy and assign.
    DocumentBuilder(const DocumentBuilder&) = delete;
    auto operator=(const DocumentBuilder&) -> DocumentBuilder& = delete;
    DocumentBuilder(DocumentBuilder&&) = delete;
    auto operator=(DocumentBuilder&&) -> DocumentBuilder& = delete;

public:
    /// @{
    /// Add a section map with the given name path to the document.
    ///
    /// @param namePath The name path of the element.
    ///
    void addSectionMap(const NamePath &namePath) {
        _builder.addSectionMap(namePath);
    }
    void addSectionMap(const String &namePath) {
        addSectionMap(NamePath::fromText(namePath));
    }
    /// @}

    /// @{
    /// Add a section list with the given name path to the document.
    ///
    /// @param namePath The name path of the element.
    ///
    void addSectionList(const NamePath &namePath) {
        _builder.addSectionList(namePath);
    }
    void addSectionList(const String &namePath) {
        addSectionList(NamePath::fromText(namePath));
    }
    /// @}

    /// @{
    /// Add a value to the document.
    ///
    /// @param namePath The name path of the element.
    /// @param value The value for the new element.
    ///
    template<typename T>
    void addValue(const NamePath &namePath, const T &value) {
        _builder.addValueT(namePath, value);
    }
    template<typename T>
    void addValue(const String &namePath, const T &value) {
        addValue<T>(NamePath::fromText(namePath), value);
    }
    void addInteger(const NamePath &namePath, const Integer value) {
        addValue<Integer>(namePath, value);
    }
    void addInteger(const String &namePath, const Integer value) {
        addValue<Integer>(NamePath::fromText(namePath), value);
    }
    void addBoolean(const NamePath &namePath, const bool value) {
        addValue<bool>(namePath, value);
    }
    void addBoolean(const String &namePath, const bool value) {
        addValue<bool>(NamePath::fromText(namePath), value);
    }
    void addFloat(const NamePath &namePath, const Float value) {
        addValue<Float>(namePath, value);
    }
    void addFloat(const String &namePath, const Float value) {
        addValue<Float>(NamePath::fromText(namePath), value);
    }
    void addText(const NamePath &namePath, const String &value) {
        addValue<String>(namePath, value);
    }
    void addText(const String &namePath, const String &value) {
        addValue<String>(NamePath::fromText(namePath), value);
    }
    void addDate(const NamePath &namePath, const Date &value) {
        addValue<Date>(namePath, value);
    }
    void addDate(const String &namePath, const Date &value) {
        addValue<Date>(NamePath::fromText(namePath), value);
    }
    void addTime(const NamePath &namePath, const Time &value) {
        addValue<Time>(namePath, value);
    }
    void addTime(const String &namePath, const Time &value) {
        addValue<Time>(NamePath::fromText(namePath), value);
    }
    void addDateTime(const NamePath &namePath, const DateTime &value) {
        addValue<DateTime>(namePath, value);
    }
    void addDateTime(const String &namePath, const DateTime &value) {
        addValue<DateTime>(NamePath::fromText(namePath), value);
    }
    void addBytes(const NamePath &namePath, const Bytes &value) {
        addValue<Bytes>(namePath, value);
    }
    void addBytes(const String &namePath, const Bytes &value) {
        addValue<Bytes>(NamePath::fromText(namePath), value);
    }
    void addTimeDelta(const NamePath &namePath, const TimeDelta &value) {
        addValue<TimeDelta>(namePath, value);
    }
    void addTimeDelta(const String &namePath, const TimeDelta &value) {
        addValue<TimeDelta>(NamePath::fromText(namePath), value);
    }
    void addRegEx(const NamePath &namePath, const String &value) {
        _builder.addRegEx(namePath, value);
    }
    void addRegEx(const String &namePath, const String &value) {
        addRegEx(NamePath::fromText(namePath), value);
    }
    /// @}

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

