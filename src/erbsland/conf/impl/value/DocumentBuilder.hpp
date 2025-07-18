// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Document.hpp"
#include "DocumentBuilderStorage.hpp"
#include "ValueMap.hpp"

#include "../../Date.hpp"
#include "../../DateTime.hpp"
#include "../../Float.hpp"
#include "../../Integer.hpp"
#include "../../NamePath.hpp"
#include "../../Time.hpp"
#include "../../TimeDelta.hpp"


namespace erbsland::conf::impl {


/// The internal document builder used by the parser.
///
class DocumentBuilder {
public:
    // defaults
    DocumentBuilder() = default;
    ~DocumentBuilder() = default;

    // Disable copy and assign
    DocumentBuilder(const DocumentBuilder&) = delete;
    auto operator=(const DocumentBuilder&) -> DocumentBuilder& = delete;
    DocumentBuilder(DocumentBuilder&&) = delete;
    auto operator=(DocumentBuilder&&) -> DocumentBuilder& = delete;

public:
    /// Reset the document builder.
    ///
    /// Erases the currently built document and resets the builder to its initial state.
    ///
    void reset() noexcept;

    /// Add a section map to the document at the given name path.
    ///
    /// - Detects name conflicts.
    /// - Builds required intermediate sections.
    /// - Registers this as the current section and name path.
    ///
    /// @param namePath The name-path of the new section map.
    /// @param location The location where this new section is defined.
    /// @throws Error (Syntax, NameConflict) on any problem.
    ///
    void addSectionMap(const NamePath &namePath, const Location &location = {});

    /// Add or extend a section list in the document at the given name path.
    ///
    /// - Detects name conflicts.
    /// - Builds required intermediate sections.
    /// - Creates a new section list or extends an existing one.
    /// - Registers this as the current section and name path.
    ///
    /// @param namePath The name-path of the new section map.
    /// @param location The location where this new section is defined.
    /// @throws Error (Syntax, NameConflict) on any problem.
    ///
    void addSectionList(const NamePath &namePath, const Location &location = {});

    /// Add a value to this document.
    ///
    /// - Detects name conflicts.
    /// - Detects conflicts if a regular named value is added to a section with text names.
    /// - Converts an empty section with names into a section with texts if required.
    ///
    /// @param namePath Either a name path with two or more elements to resolve the path to a section and
    ///     add the value to it, or a single name element to add the value to the current section.
    /// @param value The value to add to the section.
    /// @param location The location of the assignment for error messages.
    /// @throws Error (Syntax, NameConflict) on any problem.
    ///
    void addValue(const NamePath &namePath, const ValuePtr &value, const Location &location = {});

    /// Get the built document and reset the builder.
    ///
    [[nodiscard]] auto getDocumentAndReset() noexcept -> std::shared_ptr<Document>;

public: // methods for the public interface.
    template<typename T>
    void addValueT(const NamePath &namePath, const T &value) {
        static_assert(false, "addValue is not implemented for this type.");
    }

    template<typename T> requires (std::is_integral_v<T>)
    void addValueT(const NamePath &namePath, const T &value) {
        addValue(namePath, Value::createInteger(static_cast<Integer>(value)));
    }

    template<typename T> requires (std::is_floating_point_v<T>)
    void addValueT(const NamePath &namePath, const T &value) {
        addValue(namePath, Value::createFloat(static_cast<Float>(value)));
    }

    void addRegEx(const NamePath &namePath, const String &value) {
        addValue(namePath, Value::createRegEx(value));
    }

private:
    DocumentBuilderStorage _storage;
};


template<>
inline void DocumentBuilder::addValueT<String>(const NamePath &namePath, const String &value) {
    addValue(namePath, Value::createText(value));
}

template<>
inline void DocumentBuilder::addValueT<std::u8string>(const NamePath &namePath, const std::u8string &value) {
    addValue(namePath, Value::createText(String{value}));
}

template<>
inline void DocumentBuilder::addValueT<std::string>(const NamePath &namePath, const std::string &value) {
    addValue(namePath, Value::createText(String{value}));
}

template<>
inline void DocumentBuilder::addValueT<bool>(const NamePath &namePath, const bool &value) {
    addValue(namePath, Value::createBoolean(value));
}

template<>
inline void DocumentBuilder::addValueT<Date>(const NamePath &namePath, const Date &value) {
    addValue(namePath, Value::createDate(value));
}

template<>
inline void DocumentBuilder::addValueT<Time>(const NamePath &namePath, const Time &value) {
    addValue(namePath, Value::createTime(value));
}

template<>
inline void DocumentBuilder::addValueT<DateTime>(const NamePath &namePath, const DateTime &value) {
    addValue(namePath, Value::createDateTime(value));
}

template<>
inline void DocumentBuilder::addValueT<Bytes>(const NamePath &namePath, const Bytes &value) {
    addValue(namePath, Value::createBytes(value));
}

template<>
inline void DocumentBuilder::addValueT<TimeDelta>(const NamePath &namePath, const TimeDelta &value) {
    addValue(namePath, Value::createTimeDelta(value));
}


}


