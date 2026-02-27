// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
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
    if (!result.isValidUtf8()) {
        throw Error{ErrorCategory::Encoding, u8"Invalid UTF-8 in string"};
    }
    return result;
}


auto String::fromCharString(const std::string_view strView) -> String {
    auto result = String{strView.data(), strView.size()};
    if (!result.isValidUtf8()) {
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


auto String::isValidUtf8() const noexcept -> bool {
    return impl::U8StringView{*this}.isValid();
}


auto String::characterLength() const -> std::size_t {
    return impl::U8StringView{*this}.length();
}


auto String::characterCompare(const String &other, const CaseSensitivity caseSensitivity) const -> std::strong_ordering {
    const auto &comparator = (caseSensitivity == CaseSensitivity::CaseSensitive)
        ? impl::Char::compare
        : impl::Char::compareCaseInsensitive;
    return impl::U8StringView{*this}.compare(other, comparator);
}


auto String::nameCompare(const String &other) const -> std::strong_ordering {
    return impl::U8StringView{*this}.compare(other, impl::Char::compareName);
}


auto String::startsWith(const String &other, const CaseSensitivity caseSensitivity) const -> bool {
    const auto &comparator = (caseSensitivity == CaseSensitivity::CaseSensitive)
        ? impl::Char::compare
        : impl::Char::compareCaseInsensitive;
    return impl::U8StringView{*this}.startsWith(other, comparator);
}


auto String::contains(const String &other, const CaseSensitivity caseSensitivity) const -> bool {
    const auto &comparator = (caseSensitivity == CaseSensitivity::CaseSensitive)
        ? impl::Char::compare
        : impl::Char::compareCaseInsensitive;
    return impl::U8StringView{*this}.contains(other, comparator);
}


auto String::endsWith(const String &other, const CaseSensitivity caseSensitivity) const -> bool {
    const auto &comparator = (caseSensitivity == CaseSensitivity::CaseSensitive)
        ? impl::Char::compare
        : impl::Char::compareCaseInsensitive;
    return impl::U8StringView{*this}.endsWith(other, comparator);
}


auto String::split(const char32_t character, std::optional<std::size_t> maxSplits) const -> StringList {
    return impl::U8StringView{*this}.split(impl::Char{character}, maxSplits);
}


auto String::join(const StringList &parts) const -> String {
    return impl::U8StringView{*this}.join(parts);
}


auto String::transformed(const std::function<char32_t(char32_t)> &transformer) const -> String {
    return impl::U8StringView{*this}.transformed32(transformer);
}


void String::forEachCharacter(const std::function<void(char32_t)> &fn) const {
    impl::U8StringView{*this}.forEachChar32(fn);
}


auto String::toSafeText(const std::size_t maximumSize) const -> String {
    return impl::U8StringView{*this}.toSafeText(maximumSize);
}


}

