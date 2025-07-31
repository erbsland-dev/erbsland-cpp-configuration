// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "NameType.hpp"
#include "String.hpp"

#include "impl/HashHelper.hpp"
#include "impl/InternalView.hpp"

#include <vector>


namespace erbsland::conf {


/// Represents a single name.
///
/// - A regular name is always converted into its normalized lower-case form.
/// - A text-name is kept as is.
/// - An index-name is neither normalized nor range checked.
///
/// @tested `NameTest`
///
class Name {
public:
    /// @private
    /// The storage type of this name (depends on the name type).
    ///
    /// - NameType::Regular -> String
    /// - NameType::Text -> String
    /// - NameType::Index -> std::size_t
    /// - NameType::TextIndex -> std::size_t
    ///
    using Storage = std::variant<String, std::size_t>;

    /// An enum to address predefined meta-names.
    ///
    enum class Meta : std::size_t { // NOLINT(*-enum-size)
        Version = 0,
        Signature,
        Include,
        Features,
        _count
    };

    /// @private
    /// The number of predefined meta-names.
    ///
    constexpr static auto metaNameCount = static_cast<std::size_t>(Meta::_count);

    /// The array-type to return all meta-names.
    ///
    using MetaNameArray = std::array<const Name, metaNameCount>;

public:
    /// @private
    /// Create a new unchecked name with the given type.
    ///
    /// @note For user code: Please use `createRegular()`, `createText()` and
    ///     `createIndex()` for future compatibility and additional syntax checks.
    ///
    /// @param type The type of the name.
    /// @param storage The text or index of the name.
    ///
    template<typename StorageFwd> requires std::is_convertible_v<StorageFwd, Storage>
    Name(const NameType type, StorageFwd &&storage, impl::PrivateTag /*pt*/) noexcept
        : _type{type}, _value{std::forward<StorageFwd>(storage)} {
    }

    /// Default constructor.
    Name() = default;
    /// Default destructor.
    ~Name() = default;

public: // builders
    /// Create a regular name.
    ///
    /// - Converts any valid name into its normalized form.
    /// - Spacing around the name is not allowed.
    ///
    /// @param name The name in any valid format. No spacing around the name is allowed.
    /// @throws Error (Syntax, LimitExceeded, Encoding) If there is any problem with the name.
    ///
    [[nodiscard]] static auto createRegular(const String &name) -> Name;

    /// Create a text name.
    ///
    /// @param text The text name (without the double quotes).
    /// @throws Error (Syntax, LimitExceeded, Encoding) If there is any problem with the name.
    ///
    [[nodiscard]] static auto createText(const String &text) -> Name;

    /// Create a text name.
    ///
    /// @param text The text name (without the double quotes).
    /// @throws Error (Syntax, LimitExceeded, Encoding) If there is any problem with the name.
    ///
    [[nodiscard]] static auto createText(String &&text) -> Name;

    /// Create an index name (for list elements).
    ///
    [[nodiscard]] static auto createIndex(std::size_t index) -> Name;

    /// Create a text index name (for text names in a section).
    ///
    [[nodiscard]] static auto createTextIndex(std::size_t index) -> Name;

public: // operators
    /// Compare two names lexicographically.
    ///
    /// @param other The other name to compare.
    /// @return A three-way comparison result.
    ///
    auto operator<=>(const Name &other) const -> std::strong_ordering = default;

    /// Test two names for equality.
    ///
    /// @param other The other name to compare.
    /// @return `true` if both names compare equal.
    ///
    auto operator==(const Name &other) const -> bool = default;

public: // accessors
    /// Get the type of this name.
    ///
    [[nodiscard]] auto type() const noexcept -> NameType { return _type; }

    /// Test if this is an empty regular name.
    ///
    /// An empty name is not valid and created by the default constructor.
    ///
    /// @return `true` if this name is empty and of type `Regular`, `false` otherwise.
    ///
    [[nodiscard]] auto empty() const noexcept -> bool {
        return _type == NameType::Regular && std::get<String>(_value).empty();
    }

    /// Test if this name is of type `Regular`.
    ///
    /// @return `true` if the name type is `Regular`, `false` otherwise.
    ///
    [[nodiscard]] auto isRegular() const noexcept -> bool { return _type == NameType::Regular; }

    /// Test if this name is of type `Text`.
    ///
    /// @return `true` if the name type is `Text`, `false` otherwise.
    ///
    [[nodiscard]] auto isText() const noexcept -> bool { return _type == NameType::Text; }

    /// Test if this name is of type `Index`.
    ///
    /// @return `true` if the name type is `Index`, `false` otherwise.
    ///
    [[nodiscard]] auto isIndex() const noexcept -> bool { return _type == NameType::Index; }

    /// Test if this name is of type `TextIndex`.
    ///
    /// @return `true` if the name type is `TextIndex`, `false` otherwise.
    ///
    [[nodiscard]] auto isTextIndex() const noexcept -> bool { return _type == NameType::TextIndex; }

    /// Test if this is a meta name (regular and starts with '@').
    ///
    /// @return `true` if the name is `Regular`, non-empty, and begins with an '@' character.
    ///
    [[nodiscard]] auto isMeta() const noexcept -> bool {
        return _type == NameType::Regular && !empty() && std::get<String>(_value).front() == '@';
    }

    /// Get the value as text.
    ///
    /// @return The value as text. An index is converted into text.
    ///
    [[nodiscard]] auto asText() const noexcept -> String;

    /// Get the value as an index.
    ///
    /// @return The value as index, or zero for Root, Regular and Text.
    ///
    [[nodiscard]] auto asIndex() const noexcept -> std::size_t;

    /// Fast, get the size of the path text.
    ///
    [[nodiscard]] auto pathTextSize() const noexcept -> std::size_t;

    /// Create a representation of the name for a name path.
    ///
    [[nodiscard]] auto toPathText() const noexcept -> String;

    /// Get a hash value for this name.
    ///
    [[nodiscard]] auto hash() const noexcept -> std::size_t {
        std::size_t result = 0;
        impl::hashCombine(result, _type);
        impl::hashCombine(result, _value);
        return result;
    }

public:
    /// Normalizes and verifies a regular name.
    ///
    /// - Tests if the name only contains valid characters.
    /// - Tests if the name does not exceed the length limit.
    ///
    /// @throws Error (Syntax, LimitExceeded, Encoding) in case of any problem.
    /// @return The normalized name.
    ///
    [[nodiscard]] static auto normalize(const String &text) -> String;

    /// Verifies a text name.
    ///
    /// - Test for encoding errors and not allowed zero code-points.
    /// - Test if the text exceeds the size limit.
    ///
    /// @throws Error (LimitExceeded, Encoding) in case of any problem.
    ///
    static void validateText(const String &text);

    /// @name Predefined Meta-Names
    /// @{

    /// @param metaName The meta-name enum.
    static auto meta(Meta metaName) -> const Name&;
    /// Get the "version" meta-name.
    static auto metaVersion() -> const Name&;
    /// Get the "signature" meta-name.
    static auto metaSignature() -> const Name&;
    /// Get the "include" meta-name.
    static auto metaInclude() -> const Name&;
    /// Get the "features" meta-name.
    static auto metaFeatures() -> const Name&;
    /// @}

    /// Access a list of all supported meta-names.
    ///
    static auto allMetaNames() -> const MetaNameArray&;

private:
    /// Get the decimal digit-count of the index.
    ///
    [[nodiscard]] auto indexDigitCount() const noexcept -> std::size_t;

public:
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const Name &object) -> impl::InternalViewPtr;
#endif

private:
    NameType _type{NameType::Regular}; ///< The type of this name
    Storage _value; ///< The value, depending on the type.
};


using NameList = std::vector<Name>;


}


template <>
struct std::hash<erbsland::conf::Name> {
    auto operator()(const erbsland::conf::Name& name) const noexcept -> std::size_t {
        return name.hash();
    }
};


template <>
struct std::formatter<erbsland::conf::Name> : std::formatter<std::string> {
    auto format(const erbsland::conf::Name &name, format_context& ctx) const {
        return std::formatter<std::string>::format(name.toPathText().toCharString(), ctx);
    }
};
