// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "FileSource.hpp"


#include "StreamTestInterface.hpp"

#include "../../Error.hpp"

#include <utility>


namespace erbsland::conf::impl {


using namespace std::filesystem;


FileSource::FileSource(std::filesystem::path path)
:
    _path{std::move(path)},
    _identifier{SourceIdentifier::createForFile(
        String{absolute(_path).lexically_normal().string()})} {
}


auto FileSource::identifier() const noexcept -> SourceIdentifierPtr {
    return _identifier;
}


void FileSource::openStream() {
    std::filesystem::path canonicalPath;
    try {
        canonicalPath = canonical(_path);
        if (!is_regular_file(canonicalPath)) {
            throw Error(
                ErrorCategory::IO,
                u8"The source path is no regular file.",
                Location{_identifier},
                canonicalPath);
        }
    } catch (const filesystem_error &error) {
        throw Error(
            ErrorCategory::IO,
            String(u8"File not found. Error: ") + String{error.code().message()}.toEscaped(EscapeMode::ErrorText),
            Location{_identifier});
    }
    _stream.exceptions(std::ios::badbit);
    try {
        _stream.open(canonicalPath, std::ios::in | std::ios::binary);
        ERBSLAND_CONF_STREAM_TEST(afterOpen);
    } catch (const std::ios_base::failure &error) {
        throw Error(
            ErrorCategory::IO,
            String(u8"Failed to open file. Error: ") + String{error.code().message()}.toEscaped(EscapeMode::ErrorText),
            Location{_identifier});
    }
}


void FileSource::closeStream() noexcept {
    // Disable all exceptions from this point, as we are not interested in errors while and after closing a file.
    _stream.exceptions({});
    try {
        if (_stream.is_open()) {
            _stream.close();
            ERBSLAND_CONF_STREAM_TEST(afterClose);
        }
    } catch (const std::ios_base::failure&) {
        // Should not happen unless the std-library isn't well written.
        // To be extra safe, ignore exceptions while closing the file.
    }
}


}

