// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "FileAccessCheck.hpp"


#include "impl/constants/Limits.hpp"
#include "impl/utf8/U8StringView.hpp"

#include <filesystem>


namespace erbsland::conf {


void FileAccessCheck::enable(const Feature feature) {
    _features.set(feature);
}


void FileAccessCheck::disable(const Feature feature) {
    _features.reset(feature);
}


auto FileAccessCheck::isEnabled(const Feature feature) const noexcept -> bool {
    return _features.test(feature);
}


auto FileAccessCheck::check(const AccessSources &sources) -> AccessCheckResult {
    if (sources.source == nullptr || sources.root == nullptr) {
        throwAccessError(u8"No document or root source given.");
    }
    if (sources.source->name() != impl::defaults::fileSourceIdentifier) {
        if (isEnabled(OnlyFileSources)) {
            throwAccessError(u8"Only file sources are permitted.");
        }
        return AccessCheckResult::Granted; // Grant access to all other sources.
    }
    fileAccessCheck(sources);
    return AccessCheckResult::Granted;
}


void FileAccessCheck::fileAccessCheck(const AccessSources &sources) const {
    // Sanity checks.
    if (sources.source->name() != impl::defaults::fileSourceIdentifier) {
        throw std::logic_error("This function only checks file sources.");
    }
    if (!(isEnabled(AnyDirectory) || isEnabled(SameDirectory) || isEnabled(Subdirectories))) {
        throwAccessError(u8"No directory access policies are configured. All file access is currently blocked.");
    }

    // The next part checks the relationship between the included and including document.
    if (sources.parent == nullptr) {
        return; // This is the root document, grant access to it.
    }
    auto sourcePath = extractSourcePath(sources);
    auto parentDirectory = extractParentDirectory(sources);
    canonicalizePaths(sourcePath, parentDirectory);
    if (isEnabled(LimitSize)) {
        if (file_size(sourcePath) > limits::maxDocumentSize) {
            throwAccessError(
                u8"The included file exceeds the maximum allowed size of 100MB.",
                sourcePath);
        }
    }
    if (!isEnabled(AnyDirectory)) {
        const bool isInSame = requireSourceInParentDirectory(sourcePath, parentDirectory);
        if (!isEnabled(SameDirectory) && isInSame) {
            throwAccessError(
                u8"Including files from the same directory as the parent file is not permitted by policy.",
                sourcePath);
        }
        if (!isEnabled(Subdirectories) && !isInSame) {
            throwAccessError(
                u8"Including files from subdirectories is not permitted by policy.",
                sourcePath);
        }
    }
}


void FileAccessCheck::throwAccessError(
    String message,
    std::filesystem::path path,
    std::optional<std::error_code> errorCode) {

    if (errorCode.has_value()) {
        throw Error(
            ErrorCategory::Access,
            std::move(message),
            std::move(path),
            std::move(errorCode).value());
    }
    throw Error(
        ErrorCategory::Access,
        std::move(message),
        std::move(path));
}


auto FileAccessCheck::extractSourcePath(const AccessSources &sources) const -> std::filesystem::path {
    auto sourcePath = std::filesystem::path{sources.source->path().toCharString()};
    if (isEnabled(RequireSuffix)) {
        const auto extension = String{sourcePath.extension().u8string()};
        if (extension.characterCompare(impl::defaults::fileSuffix, CaseSensitivity::CaseInsensitive) != std::strong_ordering::equal) {
            throwAccessError(
                u8"The included file does not have the suffix \".elcl\".",
                sourcePath);
        }
    }
    return sourcePath;
}


auto FileAccessCheck::extractParentDirectory(const AccessSources &sources) const -> std::filesystem::path {
    std::filesystem::path parentDirectory;
    if (sources.parent == nullptr || sources.parent->name() != impl::defaults::fileSourceIdentifier) {
        if (!isEnabled(AnyDirectory)) {
            throwAccessError(u8"Cannot verify the parent path because the including document it is not a local file.");
        }
    } else {
        parentDirectory = std::filesystem::path{sources.parent->path().toCharString()};
        if (!parentDirectory.has_parent_path()) {
            throwAccessError(u8"Could not determine the parent directory of the including file.");
        }
        parentDirectory = parentDirectory.parent_path();
    }
    return parentDirectory;
}


void FileAccessCheck::canonicalizePaths(std::filesystem::path &sourcePath, std::filesystem::path &parentDirectory) {
    try {
        sourcePath = canonical(sourcePath);
    } catch (const std::system_error &error) {
        throwAccessError(
            u8"Failed to resolve the canonical path of the included file.",
            sourcePath,
            error.code());
    }
    try {
        parentDirectory = canonical(parentDirectory);
    } catch (const std::system_error &error) {
        throwAccessError(
            u8"Failed to resolve the canonical path of the parent file's directory.",
            parentDirectory,
            error.code());
    }
}


auto FileAccessCheck::requireSourceInParentDirectory(
    const std::filesystem::path &sourcePath,
    const std::filesystem::path &parentDirectory) -> bool {

    if (!sourcePath.has_parent_path()) {
        throwAccessError(u8"Could not determine the parent directory of the including file.", sourcePath);
    }
    auto sourceDirectory = sourcePath.parent_path();
    auto parentIt = parentDirectory.begin();
    auto sourceIt = sourceDirectory.begin();
    while (parentIt != parentDirectory.end()) {
        if (sourceIt == sourceDirectory.end() || *parentIt != *sourceIt) {
            throwAccessError(
                u8"The included file is outside the allowed directory range of the parent file.",
                sourcePath);
        }
        ++parentIt;
        ++sourceIt;
    }
    return sourceIt == sourceDirectory.end();
}


}
