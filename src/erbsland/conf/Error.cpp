// Copyright (c) 2024-2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Error.hpp"


#include "impl/utf8/U8Format.hpp"

#include <vector>


namespace erbsland::conf {


auto Error::withLocation(const Location &location) const noexcept -> Error {
    auto copy = *this;
    copy._location = location;
    return copy;
}


auto Error::withNamePathAndLocation(const NamePath &namePath, const Location &location) const noexcept -> Error {
    auto copy = *this;
    copy._namePath = namePath;
    copy._location = location;
    return copy;
}


auto Error::withMessagePrefix(const String &prefix) const noexcept -> Error {
    auto copy = *this;
    copy._message = prefix + copy._message;
    return copy;
}


auto Error::withMessage(const String &message) const noexcept -> Error {
    auto copy = *this;
    copy._message = message;
    return copy;
}


auto Error::toText() const noexcept -> String {
    String result;
    result.append(_category.toText());
    result.append(u8" error");
    if (!_message.empty()) {
        result.append(u8": ");
        result.append(_message);
    }
    std::vector<String> parts;
    if (_namePath.has_value()) {
        parts.emplace_back(impl::u8format("name path \"{}\"", _namePath.value()));
    }
    if (_filePath.has_value()) {
        auto pathStr = String{_filePath.value().u8string()};
        parts.emplace_back(impl::u8format("file path \"{}\"", pathStr.toSafeText()));
    }
    if (_location.has_value() && !_location.value().isUndefined()) {
        parts.emplace_back(impl::u8format(u8"location \"{}\"", _location.value()));
    }
    if (!parts.empty()) {
        for (std::size_t i = 0; i < parts.size(); ++i) {
            if (i == 0) {
                result.append(u8" at ");
            } else {
                result.append(u8", ");
            }
            result.append(parts[i]);
        }
    }
    if (_errorCode.has_value()) {
        auto errorStr = String{_errorCode.value().message()};
        result.append(impl::u8format("; system error \"{}\"", errorStr.toSafeText()));
    }
    result.append(u8".");
    return result;
}


auto Error::what() const noexcept -> const char* {
    if (_whatBuffer.empty()) {
        updateWhatBuffer();
    }
    return _whatBuffer.c_str();
}


void Error::updateWhatBuffer() const noexcept {
    _whatBuffer = toText().toCharString();
}


}

