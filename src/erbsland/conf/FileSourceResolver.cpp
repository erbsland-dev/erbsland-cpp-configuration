// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "FileSourceResolver.hpp"


#include "impl/constants/Limits.hpp"
#include "impl/utf8/U8Format.hpp"
#include "impl/utf8/U8Iterator.hpp"
#include "impl/utf8/U8StringView.hpp"

#include <cstddef>


namespace erbsland::conf {


using impl::U8StringView;
using impl::U8Decoder;


void FileSourceResolver::enable(const Feature feature) {
    _features.set(feature);
}


void FileSourceResolver::disable(const Feature feature) {
    _features.reset(feature);
}


auto FileSourceResolver::isEnabled(const Feature feature) const -> bool {
    return _features.test(feature);
}


auto FileSourceResolver::resolve(const SourceResolverContext &context) -> SourceListPtr {
    try {
        // An empty include text is not valid.
        if (context.includeText.empty()) {
            throwError(u8"The include path is empty.");
        }
        // It makes no sense for having more than 500 characters.
        if (U8StringView{context.includeText}.length() > 500) {
            throwError(u8"The include path is too long.");
        }
        auto pathString = context.includeText;
        removeFileProtocol(pathString);
        normalizePathSeparators(pathString);
        auto [directory, filename] = splitDirectoryAndFilename(pathString);
        const auto filenamePattern = getFilenamePattern(filename);
        if (filenamePattern.hasWildcard && !isEnabled(FilenameWildcard)) {
            throwError(u8"The filename wildcard '*' is not supported.");
        }
        bool isRecursive = false;
        std::tie(directory, isRecursive) = validateDirectoryWildcard(directory);
        if (isRecursive && !isEnabled(RecursiveWildcard)) {
            throwError(u8"The recursive wildcard '**' is not supported.");
        }
        const auto directoryPath = buildDirectory(context.sourceIdentifier, directory);
        const auto paths = scanForPaths(directoryPath, isRecursive, filenamePattern);
        return createSourcesFromPaths(paths);
    } catch (const std::system_error &) {
        throwError(u8"An unexpected error prevents resolving this include pattern.");
    }
}


auto FileSourceResolver::FilenamePattern::matches(const std::filesystem::directory_entry &entry) const noexcept -> bool {
    if (!is_regular_file(entry)) {
        return false;
    }
    auto filename = entry.path().filename().u8string();
    if (hasWildcard) {
        if (!prefix.empty() && !filename.starts_with(prefix)) {
            return false;
        }
        if (!suffix.empty() && !filename.ends_with(suffix)) {
            return false;
        }
        return true;
    }
    return filename == prefix;
}


void FileSourceResolver::removeFileProtocol(String &path) const noexcept {
    if (path.starts_with(u8"file:")) {
        if (!isEnabled(FileProtocol)) {
            throwError(u8"File protocol prefix 'file:' is not supported.");
        }
        path.erase(0, 5);
    }
}


void FileSourceResolver::normalizePathSeparators(String &path) {
    // Replace all backslashes with slashes.
    for (std::size_t i = 0; i < path.size(); ++i) {
        if (path[i] == u8'\\') {
            path[i] = u8'/';
        }
    }
    std::size_t pos = 0;
    if (isEnabled(WindowsUNCPath) && path.starts_with(u8"//")) {
        verifyUncPath(path);
        pos = 2;
    }
    while (pos < path.size()) {
        pos = path.find(u8'/', pos);
        if (pos == std::string::npos) {
            break;
        }
        while (pos + 1 < path.size() && path[pos + 1] == u8'/') {
            path.erase(pos, 1);
        }
        pos += 1;
    }
    if (path.ends_with(u8'/')) {
        throwError(u8"An include path must not end with a path separator.");
    }
}


void FileSourceResolver::verifyUncPath(const String &path) {
    // On Windows, accept a UNC path, starting with //server/...
    auto pathView = std::u8string_view{path.raw()};
    auto slashPosAfterServerName = pathView.find(u8'/', 2);
    if (slashPosAfterServerName == std::string::npos) {
        throwError(u8"A slash is required after the Windows UNC path server name.");
    }
    if (slashPosAfterServerName == 2) {
        throwError(u8"The UNC path has no server name. Found three consecutive slashes.");
    }
    U8StringView{path.substr(2, slashPosAfterServerName - 2)}.forEachChar([](auto c) {
        if (c == impl::CharClass::InvalidWindowsServerName) {
            throwError(u8"The server name in the Windows UNC path contains invalid characters.");
        }
    });
    if (path.size() < slashPosAfterServerName + 2) {
        throwError(u8"There is no filename after the server in the UNC path.");
    }
}


auto FileSourceResolver::splitDirectoryAndFilename(
    const String &path) noexcept -> std::tuple<std::u8string_view, std::u8string_view> {

    auto pathView = std::u8string_view{path.raw()};
    auto lastSlashPos = pathView.rfind('/');
    if (lastSlashPos == std::u8string_view::npos) {
        return std::make_tuple(std::u8string_view{}, pathView);
    }
    return std::make_tuple(pathView.substr(0, lastSlashPos), pathView.substr(lastSlashPos + 1));
}


auto FileSourceResolver::getFilenamePattern(const std::u8string_view filename) -> FilenamePattern {
    if (filename.find(u8"***") != std::u8string::npos) {
        throwError(u8"The include path contains an unsupported wildcard pattern.");
    }
    if (filename.find(u8"**") != std::u8string::npos) {
        throwError(u8"An include path must not contain the recursive '**' wildcard in the filename.");
    }
    const auto asteriskCount = std::ranges::count(filename, u8'*');
    if (asteriskCount > 1) {
        throwError(u8"An include path must not contain more than one '*' wildcard in the filename.");
    }
    if (asteriskCount > 0) {
        auto asteriskPos = filename.find_first_of(u8'*');
        return FilenamePattern{
            .prefix = filename.substr(0, asteriskPos),
            .suffix = filename.substr(asteriskPos + 1),
            .hasWildcard = true};
    }
    return FilenamePattern{.prefix = filename, .suffix = {}, .hasWildcard = false};
}


auto FileSourceResolver::validateDirectoryWildcard(std::u8string_view directory) -> std::tuple<std::u8string_view, bool> {
    if (directory.find(u8"***") != std::u8string::npos) {
        throwError(u8"The include path contains an unsupported wildcard pattern.");
    }
    std::size_t recursiveWildcardCount = 0;
    std::size_t recursiveWildcardPos = 0;
    for (std::size_t i = 0; i < directory.size(); ++i) {
        if (directory[i] == u8'*') {
            const bool prevIsStar = (i > 0 && directory[i-1] == u8'*');
            const bool nextIsStar = (i + 1 < directory.size() && directory[i+1] == u8'*');
            if (!prevIsStar && !nextIsStar) {
                throwError(u8"An include path must not contain '*' wildcard in the directory.");
            }
            if (!prevIsStar && nextIsStar) {
                recursiveWildcardCount += 1;
                if (recursiveWildcardCount > 1) {
                    throwError(u8"An include path must not contain more than one '**' wildcard in the directory.");
                }
                recursiveWildcardPos = i;
            }
        }
    }
    if (recursiveWildcardCount > 0) {
        if (directory.find_first_of(u8'/', recursiveWildcardPos + 1) != std::u8string::npos) {
            throwError(u8"The recursive wildcard '**' must not be the last directory element in the path.");
        }
        if (directory.find_first_not_of(u8'/', recursiveWildcardPos + 2) != std::u8string::npos) {
            throwError(u8"The recursive wildcard '**' must be an individual path element.");
        }
        if (recursiveWildcardPos > 0 && directory[recursiveWildcardPos - 1] != u8'/') {
            throwError(u8"The recursive wildcard '**' must be an individual path element.");
        }
        if (recursiveWildcardPos == 0) {
            return std::make_tuple(std::u8string_view{}, true);
        }
        return std::make_tuple(directory.substr(0, recursiveWildcardPos - 1), true);
    }
    return std::make_tuple(directory, false);
}


auto FileSourceResolver::getBaseDirectory(const SourceIdentifierPtr &sourceIdentifier) -> std::filesystem::path {
    if (sourceIdentifier == nullptr) {
        throw std::logic_error{"sourceIdentifier must not be null"};
    }
    const auto errorPrefix = []() -> String {
        return {u8"Cannot determine the base directory the including document. "};
    };
    if (sourceIdentifier->name() != impl::defaults::fileSourceIdentifier) {
        throwError(errorPrefix() + u8"The document is not a file source.");
    }
    if (!U8StringView{sourceIdentifier->path()}.isValid()) {
        throwError(errorPrefix() + u8"The document path is not correctly UTF-8 encoded.");
    }
    auto result = std::filesystem::path{sourceIdentifier->path().toCharString()};
    if (!result.is_absolute()) {
        throwError(errorPrefix() + u8"The path of the document is not absolute.", result);
    }
    try {
        result = canonical(result);
    } catch (const std::system_error &error) {
        throwError(
            errorPrefix() + u8"The path of the document cannot be canonicalized.",
            result,
            error.code());
    }
    auto baseDirectory = result.parent_path();
    if (baseDirectory.empty()) {
        throwError(errorPrefix() + u8"Could not determine the directory of the document.", result);
    }
    if (!is_directory(baseDirectory)) {
        throwError(errorPrefix() + u8"The parent path of the document is not a directory.", baseDirectory);
    }
    return baseDirectory;
}


auto FileSourceResolver::buildDirectory(
    const SourceIdentifierPtr &sourceIdentifier,
    const std::u8string_view directory) const -> std::filesystem::path {

    std::filesystem::path result;
    if (directory.empty()) {
        result = getBaseDirectory(sourceIdentifier);
    } else {
        result = std::filesystem::path{directory};
        if (!result.is_absolute()) {
            result = getBaseDirectory(sourceIdentifier) / result;
        } else if (!isEnabled(AbsolutePaths)) {
            throwError(u8"Absolute include paths are not allowed.");
        }
    }
    try {
        if (!exists(result)) {
            throwError(u8"The base directory of an include path does not exist.", result);
        }
        result = canonical(result);
        if (!is_directory(result)) {
            throwError(u8"The base of an include path is not a directory.", result);
        }
    } catch (const std::system_error &error) {
        throwError(
            u8"Could not canonicalize the base directory of an include path.",
            result,
            error.code());
    }
    return result;
}


auto FileSourceResolver::scanForPaths(
    const std::filesystem::path &directory,
    const bool isRecursive,
    const FilenamePattern &filenamePattern) -> std::vector<std::filesystem::path> {

    std::vector<std::filesystem::path> paths;
    if (isRecursive) {
        std::ranges::for_each(
            std::filesystem::recursive_directory_iterator(directory, std::filesystem::directory_options::skip_permission_denied),
            [&](const std::filesystem::directory_entry &entry) {
                if (filenamePattern.matches(entry)) {
                    if (paths.size() >= limits::maxIncludeSources) {
                        throw Error(
                            ErrorCategory::LimitExceeded,
                            impl::u8format(u8"This include directive includes more than {} documents.", limits::maxIncludeSources));
                    }
                    paths.push_back(entry.path());
                }
            }
        );
    } else {
        if (filenamePattern.hasWildcard) {
            std::ranges::for_each(
                std::filesystem::directory_iterator(directory, std::filesystem::directory_options::skip_permission_denied),
                [&](const std::filesystem::directory_entry &entry) {
                    if (filenamePattern.matches(entry)) {
                        if (paths.size() >= limits::maxIncludeSources) {
                            throw Error(
                                ErrorCategory::LimitExceeded,
                                impl::u8format(u8"This include directive includes more than {} documents.", limits::maxIncludeSources));
                        }
                        paths.push_back(entry.path());
                    }
                }
            );
        } else {
            paths.push_back(directory / filenamePattern.prefix);
        }
    }
    return paths;
}


auto FileSourceResolver::createSourcesFromPaths(const std::vector<std::filesystem::path> &paths) -> SourceListPtr {
    auto result = std::make_shared<SourceList>();
    for (auto path : paths) {
        try {
            path = canonical(path);
            if (!is_regular_file(path)) {
                throwError(u8"The path of an included file is not a regular file.", path);
            }
        } catch (const std::system_error &error) {
            throwError(u8"Could not find the path of an included file.", path, error.code());
        }
        const auto pathStr = String{path.u8string()};
        if (!pathStr.isValidUtf8()) {
            throwError(u8"The path of an include file is not correctly UTF-8 encoded.");
        }
        result->push_back(Source::fromFile(path));
    }
    std::ranges::stable_sort(result->begin(), result->end(), sortLess);
    return result;
}


auto FileSourceResolver::sortLess(const SourcePtr &a, const SourcePtr &b) noexcept -> bool {
    const auto pathA = splitPath(a->path().raw());
    const auto pathB = splitPath(b->path().raw());
    auto itA = pathA.begin();
    auto itB = pathB.begin();
    for (; itA != pathA.end() && itB != pathB.end(); ++itA, ++itB) {
        const auto aIsDir = itA->ends_with(u8'/') || itA->ends_with(u8'\\');
        const auto bIsDir = itB->ends_with(u8'/') || itB->ends_with(u8'\\');
        if (aIsDir == bIsDir) {
            if (*itA == *itB) {
                continue; // check the next element.
            }
            return *itA < *itB;
        }
        return !aIsDir;
    }
    if (itA == pathA.end() && itB == pathB.end()) {
        return false; // equal
    }
    return itA == pathA.end(); // the shorter path = less.
}


auto FileSourceResolver::splitPath(const std::u8string &path) noexcept -> std::vector<std::u8string_view> {
    std::vector<std::u8string_view> result;
    const std::u8string_view pathView{path};
    std::size_t pos = 0;
    std::size_t lastPos = 0;
    while (pos < pathView.size()) {
        pos = pathView.find_first_of(u8"/\\", pos);
        if (pos == std::u8string::npos) {
            result.push_back(pathView.substr(lastPos));
            break;
        }
        pos += 1;
        result.push_back(pathView.substr(lastPos, pos - lastPos));
        lastPos = pos;
    }
    return result;
}


void FileSourceResolver::throwError(
    String message,
    std::optional<std::filesystem::path> path,
    std::optional<std::error_code> errorCode) {

    if (path.has_value()) {
        if (errorCode.has_value()) {
            throw Error{
                ErrorCategory::Syntax,
                std::move(message),
                std::move(path).value(),
                std::move(errorCode).value()};
        }
        throw Error{
            ErrorCategory::Syntax,
            std::move(message),
            std::move(path).value()};
    }
    throw Error{ErrorCategory::Syntax, std::move(message)};
}


}

