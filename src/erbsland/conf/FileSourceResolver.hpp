// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "SourceResolver.hpp"

#include <bitset>
#include <filesystem>
#include <memory>


namespace erbsland::conf {


class FileSourceResolver;
using FileSourceResolverPtr = std::shared_ptr<FileSourceResolver>;


/// A file source resolver.
///
/// The file source resolver supports the recommended format to include files. It works with relative and
/// absolute paths and also has support for wildcards.
///
/// Here are a few examples:
///
/// @code
/// &at;include "file:example.elcl"              # File in the same directory.
/// &at;include "file:sub/example.elcl"          # File in a subdirectory of the current configuration file.
/// &at;include "file:../example.elcl"           # File in the parent directory (if access rules allow it)
/// &at;include "file:/usr/local/example.elcl"   # Absolute path.
/// @endcode
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

    /// Default constructor.
    FileSourceResolver() = default;
    /// Default destructor.
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
