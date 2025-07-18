// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "SourceResolver.hpp"

#include <bitset>
#include <filesystem>
#include <memory>


namespace erbsland::conf {


namespace impl { class U8Iterator; }

class FileSourceResolver;
using FileSourceResolverPtr = std::shared_ptr<FileSourceResolver>;


/// A file source resolver.
///
/// The file source resolver supports the recommended format to include files. It works with relative and
/// absolute paths and also has support for wildcards.
///
/// Here a few examples:
/// <code language="text">
/// @include "file:example.elcl"              # File in the same directory.
/// @include "file:sub/example.elcl"          # File in a subdirectory of the current configuration file.
/// @include "file:../example.elcl"           # File in the parent directory (if access rules allow it)
/// @include "file:/usr/local/example.elcl"   # Absolute path.
/// </code>
///
/// ## Wildcards
///
/// Please note: This implementation has no full globbing support that you would from a shell. The wildcard
/// support is limited to a maximum of *one* `**` and *one* `*` wildcard.
///
/// ### The '*' Wildcard
///
/// Each path can contain a maximum of *one* `*` wildcard in the *filename* portion of the path.
///
/// <code language="text">
/// @include "file:sub/*.elcl"  # Includes all files in the sub directory `sub` with the suffix `.elcl`
/// @include "file:server_*"    # Includes all files from the current directory that start with `server_`
/// </code>
///
/// Using multiple `*` Wildcards, or using it in a directory part of the path with result in an error.
///
/// ### The `**` Wildcard
///
/// Each path can contain a maximum of *one* `**` wildcard in the *directory* portion of the path.
///
/// <code language="text">
/// # Include all `server_*` ELCL configuration files from this directory and all subdirectories of the current path.
/// @include "file:**/server_*.elcl"
/// # Include all ELCL files in the `/usr/local/app/` directory and from all subdirectories.
/// @include "file:/usr/local/app/**/*.elcl"
/// </code>
///
/// The `**` wildcard must stand alone, with no prefixed or suffixed characters.
///
/// @note This class assumes that the `resolve` method is called from the parser, and therefore the text encoding
///     is already verified. It assumes there are no UTF-8 encoding errors in the text.
///
/// @note File and directory names can contain deliberately inserted UTF-8 encoding errors or other control characters.
///     For this reason, the resolver checks the UTF-8 encoding of all paths and stops if any incorrect UTF-8
///     encoding was found.
///
class FileSourceResolver : public SourceResolver {
public:
    /// Features of the file source resolver
    ///
    enum Feature : uint8_t {
        RecursiveWildcard, ///< Support for recursive wildcards.
        FilenameWildcard, ///< Support for filename wildcards.
        AbsolutePaths, ///< Support for absolute paths.
        WindowsUNCPath, ///< Support for Windows UNC paths.
        FileProtocol, ///< Support for the `file:` protocol prefix.

        _featureCount
    };

public:
    /// Create a new instance of the file source resolver.
    ///
    static auto create() -> FileSourceResolverPtr {
        return std::make_shared<FileSourceResolver>();
    }

    // defaults
    FileSourceResolver() = default;
    ~FileSourceResolver() override = default;

public: // Settings.
    /// Enable a feature
    ///
    void enable(Feature feature);

    /// Disable a feature
    ///
    void disable(Feature feature);

    /// Test if a feature enabled.
    ///
    [[nodiscard]] auto isEnabled(Feature feature) const -> bool;

public: // implement `SourceResolver`
    auto resolve(const SourceResolverContext &context) -> SourceListPtr override;

private:
    struct FilenamePattern {
        std::u8string_view prefix;
        std::u8string_view suffix;
        bool hasWildcard;

        [[nodiscard]] auto matches(const std::filesystem::directory_entry &entry) const noexcept -> bool;
    };

private:
    void removeFileProtocol(String &path) const noexcept;
    void normalizePathSeparators(String &path);
    static void verifyUncPath(const String &path);
    [[nodiscard]] static auto splitDirectoryAndFilename(
        const String &path) noexcept -> std::tuple<std::u8string_view, std::u8string_view>;
    [[nodiscard]] static auto getFilenamePattern(
        std::u8string_view filename) -> FilenamePattern;
    [[nodiscard]] static auto validateDirectoryWildcard(
        std::u8string_view directory) -> std::tuple<std::u8string_view, bool>;
    [[nodiscard]] static auto getBaseDirectory(
        const SourceIdentifierPtr &sourceIdentifier) -> std::filesystem::path;
    [[nodiscard]] auto buildDirectory(
        const SourceIdentifierPtr &sourceIdentifier,
        std::u8string_view directory) const -> std::filesystem::path;
    [[nodiscard]] static auto scanForPaths(
        const std::filesystem::path &directory,
        bool isRecursive,
        const FilenamePattern &filenamePattern) -> std::vector<std::filesystem::path>;
    [[nodiscard]] static auto createSourcesFromPaths(
        const std::vector<std::filesystem::path> &paths) -> SourceListPtr;
    [[noreturn]] static void throwError(
        String message,
        std::optional<std::filesystem::path> path = std::nullopt,
        std::optional<std::error_code> errorCode = std::nullopt);
    [[nodiscard]] static auto sortLess(
        const SourcePtr &a,
        const SourcePtr &b) noexcept -> bool;
    [[nodiscard]] static auto splitPath(
        const std::u8string &path) noexcept -> std::vector<std::u8string_view>;

private:
    std::bitset<_featureCount> _features{0b11111}; ///< Flags for the features.
};


}
