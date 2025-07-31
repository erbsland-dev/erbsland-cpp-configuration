// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "AccessCheck.hpp"

#include <bitset>
#include <filesystem>


namespace erbsland::conf {


class FileAccessCheck;
using FileAccessCheckPtr = std::shared_ptr<FileAccessCheck>;


/// A basic file access check.
///
/// By default, the `CanonicalizePath`, `SameDirectory`, `Subdirectories` features are activated.
///
/// - If neither `SameDirectory`, `Subdirectories` or `AnyDirectory` is set, all file sources are rejected.
/// - If a file is included from a non-file source and `AnyDirectory` is not set, the source is rejected.
///
class FileAccessCheck : public AccessCheck {
public:
    /// The feature flags controlling file access restrictions.
    ///
    enum Feature : uint8_t {
        /// Allow *included sources* to be in the same directory as the including document (recommended, default).
        ///
        /// Example: If the including document has the path `config/main.elcl` documents that are in the directory
        /// `config`, like `config/other.elcl` are accepted.
        ///
        /// If this feature is disabled, documents in the same directory as the including document are rejected.
        ///
        SameDirectory,

        /// Allow *included sources* in subdirectories of the parent document (recommended, default).
        ///
        /// Example: If the including document has the path `config/main.elcl` documents that are in subdirectories of
        /// `config`, like `config/sub/other.elcl` are accepted.
        ///
        /// If this feature is disabled, documents in subdirectories of the including document are rejected.
        ///
        Subdirectories,

        /// Not Recommended: Allow *included sources* in any directory.
        ///
        /// Included sources can be anywhere in the filesystem and on shares. Paths can point anywhere.
        ///
        AnyDirectory,

        /// Only allow file sources and reject everything else.
        ///
        /// If this feature is enabled, this access check only accepts file sources. Sources of any other type
        /// e.g. "text" sources are rejected. If this feature is disabled, which is the default, this check only
        /// focuses on "file" sources and grants access to any other sources.
        ///
        /// Granting non-file source is designed to allow chaining multiple checks.
        ///
        OnlyFileSources,

        /// Limit the maximum size of a file to 100MB (recommended, default).
        ///
        LimitSize,

        /// Only allow file sources with an `.elcl ` suffix.
        ///
        /// If this feature is set, this access check only accepts file sources with an `.elcl` suffix.
        ///
        RequireSuffix,

        _featureCount,
    };

public:
    /// Create a custom file access check instance.
    ///
    static auto create() -> FileAccessCheckPtr {
        return std::make_shared<FileAccessCheck>();
    }

    /// Default constructor.
    FileAccessCheck() = default;
    /// Default destructor.
    ~FileAccessCheck() override = default;

public:
    /// Enable a feature.
    ///
    void enable(Feature feature);

    /// Disable a feature
    ///
    void disable(Feature feature);

    /// Test if a feature is enabled.
    ///
    [[nodiscard]] auto isEnabled(Feature feature) const noexcept -> bool ;

public: // implement `AccessCheck`
    auto check(const AccessSources &sources) -> AccessCheckResult override;

protected:
    /// Check the file access.
    ///
    /// @param sources The sources structure to check. Must contain a valid file source.
    ///
    void fileAccessCheck(const AccessSources &sources) const;

    /// Throw an access error.
    ///
    /// @param message The error message.
    /// @param path The affected path.
    /// @param errorCode An optional error code.
    ///
    [[noreturn]] static void throwAccessError(
        String message,
        std::filesystem::path path = {},
        std::optional<std::error_code> errorCode = {});

private:
    auto extractSourcePath(const AccessSources &sources) const -> std::filesystem::path;
    auto extractParentDirectory(const AccessSources &sources) const -> std::filesystem::path;
    static void canonicalizePaths(
        std::filesystem::path &sourcePath,
        std::filesystem::path &parentDirectory);
    [[nodiscard]] static auto requireSourceInParentDirectory(
        const std::filesystem::path &sourcePath,
        const std::filesystem::path &parentDirectory) -> bool;

private:
    std::bitset<_featureCount> _features{0b10011}; ///< The enabled features
};


}
