// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "StreamSource.hpp"

#include <filesystem>
#include <fstream>


namespace erbsland::conf::impl {


/// A file source.
///
/// @tested `FileSourceTest`
///
class FileSource final : public StreamSource {
public:
    /// Create a new file system source.
    ///
    explicit FileSource(std::filesystem::path path);

    // defaults
    ~FileSource() override = default;

public: // Implement stream source.
    [[nodiscard]] auto identifier() const noexcept -> SourceIdentifierPtr override;

public: // Access the underlying path.
    [[nodiscard]] auto filesystemPath() const noexcept -> const std::filesystem::path& { return _path; }

protected:
    void openStream() override;
    auto stream() noexcept -> Stream& override { return _stream; };
    void closeStream() noexcept override;

private:
    std::filesystem::path _path; ///< The path from where this source reads its data.
    SourceIdentifierPtr _identifier; ///< The identifier `file:<path>` for this source.
    std::ifstream _stream; ///< The stream to read the data.
};


}

