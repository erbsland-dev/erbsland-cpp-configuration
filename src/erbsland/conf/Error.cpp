// Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Error.hpp"


#include "impl/utf8/U8Format.hpp"
#include "impl/utf8/U8StringView.hpp"


namespace erbsland::conf {


auto Error::withLocation(const Location &location) const noexcept -> Error {
    auto copy = *this;
    copy._location = std::move(location);
    return copy;
}


auto Error::toText() const noexcept -> String {
    String result;
    if (_message.empty()) {
        result.append(_category.toText());
        result.append(u8" Error");
    } else {
        result.append(_category.toText());
        result.append(u8": ");
        result.append(_message);
    }
    if (_namePath.has_value()) {
        result.append(u8" name path = ");
        result.append(_namePath.value().toText());
    }
    if (_filePath.has_value()) {
        auto pathStr = String{_filePath.value().u8string()};
        pathStr = impl::U8StringView{pathStr}.toSafeText();
        result.append(impl::u8format(" file path = \"{}\"", pathStr));
    }
    if (_errorCode.has_value()) {
        auto safeErrorStr = String{_errorCode.value().message()};
        safeErrorStr = impl::U8StringView{safeErrorStr}.toSafeText();
        result.append(impl::u8format(" system error = \"{}\"", safeErrorStr));
    }
    if (_location.has_value() && !_location.value().isUndefined()) {
        result.append(u8" at location = ");
        result.append(_location.value().toText());
    }
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

