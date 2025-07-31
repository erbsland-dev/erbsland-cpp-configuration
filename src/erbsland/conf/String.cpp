// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "String.hpp"


#include "Bytes.hpp"
#include "Error.hpp"
#include "ErrorCategory.hpp"

#include "impl/utf8/U8Format.hpp"
#include "impl/utf8/U8StringView.hpp"


namespace erbsland::conf {


auto String::fromCharString(const std::string &str) -> String {
    auto result = String{str};
    if (!impl::U8StringView{result}.isValid()) {
        throw Error{ErrorCategory::Encoding, u8"Invalid UTF-8 in string"};
    }
    return result;
}


auto String::fromCharString(const std::string_view strView) -> String {
    auto result = String{strView.data(), strView.size()};
    if (!impl::U8StringView{result}.isValid()) {
        throw Error{ErrorCategory::Encoding, u8"Invalid UTF-8 in string"};
    }
    return result;
}


auto String::escapedSize(EscapeMode mode) const noexcept -> std::size_t {
    return impl::U8StringView{*this}.escapedSize(mode);
}


auto String::toEscaped(const EscapeMode mode) const noexcept -> String {
    return impl::U8StringView{*this}.toEscaped(mode);
}


}

