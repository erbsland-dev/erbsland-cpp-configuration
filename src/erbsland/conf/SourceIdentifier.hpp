// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "String.hpp"

#include "impl/constants/Defaults.hpp"
#include "impl/utf8/U8StringView.hpp"
#include "impl/utilities/InternalView.hpp"

#include <memory>
#include <utility>


namespace erbsland::conf {


class SourceIdentifier;
using SourceIdentifierPtr = std::shared_ptr<SourceIdentifier>;


/// Lightweight identifier for a configuration source.
///
/// Instances of this class are usually shared between locations so that the
/// parser and higher layers can refer to the same source without copying the
/// underlying name and path strings.
///
/// @tested `SourceIdentifierTest`
///
class SourceIdentifier {
public:
    /// Create a new source identifier with explicit name and path.
    ///
    /// @param name The name of the source.
    /// @param path The path of the source.
    ///
    SourceIdentifier(String name, String path, impl::PrivateTag) noexcept
        : _name{std::move(name)}, _path{std::move(path)} {
    };

    /// Factory function to create a shared source identifier.
    ///
    /// @param name The source name.
    /// @param path The source path.
    /// @return Shared-pointer to the new instance.
    ///
    [[nodiscard]] static auto create(String name, String path) noexcept -> SourceIdentifierPtr {
        return std::make_shared<SourceIdentifier>(std::move(name), std::move(path), impl::PrivateTag{});
    }

    /// Create a new source identifier for a file.
    ///
    /// @param path The source path.
    /// @return Shared-pointer to the new instance.
    ///
    [[nodiscard]] static auto createForFile(String path) noexcept -> SourceIdentifierPtr {
        return std::make_shared<SourceIdentifier>(
            impl::defaults::fileSourceIdentifier,
            std::move(path), impl::PrivateTag{});
    }

    /// Create a new source identifier for text.
    ///
    /// @return Shared-pointer to the new instance.
    ///
    [[nodiscard]] static auto createForText() noexcept -> SourceIdentifierPtr {
        return std::make_shared<SourceIdentifier>(impl::defaults::textSourceIdentifier, String{}, impl::PrivateTag{});
    }

    /// Default destructor.
    ~SourceIdentifier() = default;

public: // operators
    /// Compare this source identifier to another for equality.
    ///
    /// @param other The other identifier to compare.
    /// @return `true` if both identifiers have the same name and path.
    ///
    [[nodiscard]] auto operator==(const SourceIdentifier &other) const noexcept -> bool {
        return _name == other._name && _path == other._path;
    }

    /// Compare this source identifier to another for inequality.
    ///
    /// @param other The other identifier to compare.
    /// @return `true` if the identifiers differ.
    ///
    [[nodiscard]] auto operator!=(const SourceIdentifier &other) const noexcept -> bool {
        return !operator==(other);
    };

public: // accessors
    /// Get the name of the source.
    ///
    [[nodiscard]] auto name() const noexcept -> const String& { return _name; }

    /// Get the path of the source.
    ///
    [[nodiscard]] auto path() const noexcept -> const String& { return _path; }

public: // conversion
    /// Get a text representation of this source identifier.
    ///
    /// @return A text representation of the identifier.
    ///
    [[nodiscard]] auto toText() const noexcept -> String {
        const auto safePath = _path.toSafeText();
        if (_name.empty() || _path.empty()) {
            if (!_name.empty()) {
                return _name;
            }
            return String{u8"unknown:"} + safePath;
        }
        return _name + String{u8":"} + safePath;
    }

public: // helpers
    /// A helper function to easily compare two source identifier pointers.
    ///
    /// @param a The first identifier.
    /// @param b The second identifier.
    /// @return `true` if both identifier pointers are either nullptr, or compare to the same values.
    ///
    static auto areEqual(const SourceIdentifierPtr &a, const SourceIdentifierPtr &b) noexcept -> bool {
        if (a == nullptr && b == nullptr) {
            return true;
        }
        if (a != nullptr && b != nullptr) {
            return *a == *b;
        }
        return false;
    }

public: // testing
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const SourceIdentifier &object) -> impl::InternalViewPtr {
        auto view = impl::InternalView::create();
        view->setValue(u8"name", object._name);
        view->setValue(u8"path", object._path);
        return view;
    }
#endif

private:
    String _name; ///< The name of the source.
    String _path; ///< The path of the source.
};


}


template <>
struct std::formatter<erbsland::conf::SourceIdentifier> : std::formatter<std::string> {
    auto format(const erbsland::conf::SourceIdentifier &sourceIdentifier, format_context& ctx) const {
        return std::formatter<std::string>::format(sourceIdentifier.toText().toCharString(), ctx);
    }
};
