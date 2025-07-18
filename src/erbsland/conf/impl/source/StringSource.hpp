// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "StreamSource.hpp"

#include <sstream>


namespace erbsland::conf::impl {


/// A string source
///
/// @tested `StringSourceTest`
///
class StringSource final : public StreamSource {
public:
    /// Create a new string source.
    ///
    explicit StringSource(const String &text);
    explicit StringSource(std::string &&text);
    explicit StringSource(const std::string &text);

    // defaults
    ~StringSource() override = default;

public:
    [[nodiscard]] auto identifier() const noexcept -> SourceIdentifierPtr override;

protected:
    void openStream() override {};
    auto stream() noexcept -> Stream& override { return _stream; };
    void closeStream() noexcept override {};

private:
    std::istringstream _stream; ///< The stream to read the data.
};


}

