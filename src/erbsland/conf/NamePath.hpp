// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Name.hpp"

#include "impl/InternalView.hpp"

#include <algorithm>
#include <ranges>


namespace erbsland::conf {


/// A name-path.
///
/// This class represents a name path that points to elements in a configuration document. It allows building
/// paths freely, using individual name elements.
///
/// Please note that other than the two different forms in configuration documents, relative and absolute paths use
/// the same text representation when converted from and into text.
/// Relative paths have no name separator prefixed, as this is the case in configuration documents.
/// This lack of difference is due the API treats both forms the same.
/// Addressing a value is always done using relative paths, and the element on which you call the "value" method
/// decides if you start to resolve the path from the root or from a branch in the document.
///
/// @tested `NamePathTest`, `NamePathLexerTest`
///
class NamePath final {
public:
    // Required definitions to be used with std algorithms.
    // Only provide const access to the elements of a name path.
    using iterator = NameList::const_iterator;
    using const_iterator = NameList::const_iterator;
    using value_type = Name;
    using reference = const Name&;
    using const_reference = const Name&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = const Name*;
    using const_pointer = const Name*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using const_iterator_category = std::forward_iterator_tag;
    using iterator_category = std::forward_iterator_tag;

public:
    /// @{
    /// Create a name path with one single name.
    ///
    NamePath(const Name &name) : _names{1, name} {} // NOLINT(*-explicit-constructor)
    NamePath(Name &&name) { // NOLINT(*-explicit-constructor)
        _names.emplace_back(std::move(name));
    }
    /// @}

    /// Create a name path from the given sequence of names.
    ///
    explicit NamePath(NameList names)
        : _names{std::move(names)} {
    }

    /// Create a name path from the given sequence of names.
    ///
    explicit NamePath(const std::span<const Name> names)
        : _names{names.begin(), names.end()} {
    }

    /// Create a name path from the given sequence of names.
    ///
    explicit NamePath(
        const NameList::const_iterator begin,
        const NameList::const_iterator end)
    :
        _names{begin, end} {
    }

    // defaults
    NamePath() = default;
    ~NamePath() = default;

public: // operators
    auto operator<=>(const NamePath&) const -> std::strong_ordering = default;
    auto operator==(const NamePath&) const -> bool = default;

public: // accessors and tests.
    /// Test if this is an empty path.
    ///
    [[nodiscard]] auto empty() const noexcept -> bool {
        return _names.empty();
    }

    /// Get the number of elements in this path.
    ///
    [[nodiscard]] auto size() const noexcept -> std::size_t {
        return _names.size();
    }

    /// Access one name in the name path.
    ///
    [[nodiscard]] auto at(const std::size_t index) const -> const Name& {
        return _names.at(index);
    }

    /// Access the first element.
    ///
    [[nodiscard]] auto front() const -> const Name& {
        return _names.front();
    }

    /// Access the last element.
    ///
    [[nodiscard]] auto back() const -> const Name& {
        return _names.back();
    }

public: // tests
    /// Test if this path contains an index (index or text-index).
    ///
    [[nodiscard]] auto containsIndex() const noexcept -> bool {
        return std::any_of(_names.begin(), _names.end(), [](const auto& name) {
            return name.isIndex() || name.isTextIndex();
        });
    }

public: // iterators
    /// Get an iterator to the first name in the path.
    ///
    [[nodiscard]] auto begin() const noexcept -> NameList::const_iterator {
        return _names.begin();
    }

    /// Get an iterator to the end of the path.
    ///
    [[nodiscard]] auto end() const noexcept -> NameList::const_iterator {
        return _names.end();
    }

    /// Access a view of all elements.
    ///
    [[nodiscard]] auto view() const noexcept -> std::span<const Name> {
        return std::span{_names};
    }

public: // working with the name path.
    /// Get the parent path.
    ///
    [[nodiscard]] auto parent() const noexcept {
        if (size() <= 1) {
            return NamePath{};
        }
        return NamePath(_names.begin(), _names.end() - 1);
    }

    /// Append a name to this path.
    ///
    /// @param name The name to append to this path.
    ///
    template<typename Fwd> requires std::is_convertible_v<Fwd, Name>
    void append(Fwd&& name) noexcept {
        _names.emplace_back(std::forward<Fwd>(name));
    }

    /// Append another name path to this path.
    ///
    /// @param namePath The name path to append.
    ///
    void append(const NamePath &namePath) noexcept {
        if (namePath.empty()) {
            return;
        }
        _names.insert(_names.end(), namePath._names.begin(), namePath._names.end());
    }

    /// Prepend another name path in front of this path.
    ///
    /// @param namePath The name path to prepend.
    ///
    void prepend(const NamePath &namePath) noexcept {
        if (namePath.empty()) {
            return;
        }
        _names.insert(_names.begin(), namePath._names.begin(), namePath._names.end());
    }

public: // conversion
    /// Convert this name path into a string.
    ///
    /// @return The name path in text form, or an empty string for an empty path or if the path is not valid.
    ///
    [[nodiscard]] auto toText() const noexcept -> String;

    /// Convert a name path from a text.
    ///
    /// Convert name paths for accessing value elements, therefore, it supports the name path extensions for the API.
    /// Create a name path from the given text: e.g. "main.server[2].path", or just "main",
    /// "[1]", "\"text\"", "\"\"[1]" as the path may start at any value element in the value-tree.
    ///
    /// @param text The path in its text form.
    /// @throws Error (Syntax, Encoding, Limit) In case of any problem with the name.
    ///
    [[nodiscard]] static auto fromText(const String& text) -> NamePath;

public:
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const NamePath &object) -> impl::InternalViewPtr;
#endif

private:
    NameList _names; ///< The list of names that build the name path.
};


}


template <>
struct std::hash<erbsland::conf::NamePath> {
    auto operator()(const erbsland::conf::NamePath& namePath) const noexcept -> std::size_t {
        std::size_t result = 0;
        for (const auto& name : namePath) {
            erbsland::conf::impl::hashCombine(result, name);
        }
        return result;
    }
};


template <>
struct std::formatter<erbsland::conf::NamePath> : std::formatter<std::string> {
    auto format(const erbsland::conf::NamePath &namePath, format_context& ctx) const {
        return std::formatter<std::string>::format(namePath.toText().toCharString(), ctx);
    }
};

