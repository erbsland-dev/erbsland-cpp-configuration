// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringSource.hpp"


namespace erbsland::conf::impl {


StringSource::StringSource(const String &text) : _stream{text.toCharString()} {
    _stream.exceptions(std::ios::badbit);
}


StringSource::StringSource(std::string &&text) : _stream{std::move(text)} {
    _stream.exceptions(std::ios::badbit);
}


StringSource::StringSource(const std::string &text) : _stream{text} {
    _stream.exceptions(std::ios::badbit);
}


auto StringSource::identifier() const noexcept -> SourceIdentifierPtr {
    static auto identifier = SourceIdentifier::createForText();
    return identifier;
}


}

