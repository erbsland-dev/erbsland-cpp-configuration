// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Name.hpp"


#include "Error.hpp"

#include "impl/constants/Limits.hpp"
#include "impl/utf8/U8Decoder.hpp"
#include "impl/utf8/U8Format.hpp"
#include "impl/vr/RulesConstants.hpp"

#include <cassert>


namespace erbsland::conf {


auto Name::createRegular(const String &name) -> Name {
    return Name{NameType::Regular, normalize(name), impl::PrivateTag{}};
}


auto Name::createText(const String &text) -> Name {
    validateText(text);
    return Name{NameType::Text, text, impl::PrivateTag{}};
}


auto Name::createText(String &&text) -> Name {
    validateText(text);
    return Name{NameType::Text, Storage{std::move(text)}, impl::PrivateTag{}};
}


auto Name::createIndex(std::size_t index) -> Name {
    return Name{NameType::Index, index, impl::PrivateTag{}};
}


auto Name::createTextIndex(std::size_t index) -> Name {
    return Name{NameType::TextIndex, index, impl::PrivateTag{}};
}


auto Name::isReservedValidationRule() const noexcept -> bool {
    return std::holds_alternative<String>(_value) && std::get<String>(_value).starts_with(impl::vrc::cReservedPrefix);
}


auto Name::isEscapedReservedValidationRule() const noexcept -> bool {
    return std::holds_alternative<String>(_value) && std::get<String>(_value).starts_with(impl::vrc::cReservedEscape);
}


auto Name::withReservedVRPrefixRemoved() const noexcept -> Name {
    if (!isReservedValidationRule()) {
        return *this;
    }
    return Name{_type, std::get<String>(_value).substr(impl::vrc::cReservedPrefix.size()), impl::PrivateTag{}};
}


auto Name::asText() const noexcept -> String {
    if (std::holds_alternative<String>(_value)) {
        return std::get<String>(_value);
    }
    return impl::u8format("{}", std::get<std::size_t>(_value));
}


auto Name::asIndex() const noexcept -> std::size_t {
    if (std::holds_alternative<std::size_t>(_value)) {
        return std::get<std::size_t>(_value);
    }
    return 0;
}


auto Name::pathTextSize() const noexcept -> std::size_t {
    switch (_type) {
    case NameType::Regular:
        return std::get<String>(_value).size();
    case NameType::Text:
        return std::get<String>(_value).escapedSize(EscapeMode::FullTextName) + 2; // "text"
    case NameType::Index:
        return indexDigitCount() + 2; // [<index>]
    case NameType::TextIndex:
        return indexDigitCount() + 4; // ""[<index>]
    default:
        assert(false);
        return 0;
    }
}


auto Name::toPathText() const noexcept -> String {
    switch (_type) {
    case NameType::Regular:
        return std::get<String>(_value);
    case NameType::Text:
        return impl::u8format("\"{}\"", std::get<String>(_value).toEscaped(EscapeMode::FullTextName));
    case NameType::Index:
        return impl::u8format("[{}]", std::get<std::size_t>(_value));
    case NameType::TextIndex:
        return impl::u8format("\"\"[{}]", std::get<std::size_t>(_value));
    default:
        assert(false);
        return {};
    }
}


auto Name::normalize(const String &text) -> String {
    if (text.empty()) {
        throw Error{ErrorCategory::Syntax, u8"Regular names must not be empty."};
    }
    if (text.size() > limits::maxNameLength) {
        // As regular names must contain only 7-bit characters, this size check is enough.
        // It may give a confusing error message if an API user uses multibyte characters.
        throw Error{ErrorCategory::LimitExceeded, u8"The given name is too long."};
    }
    auto decoder = impl::U8Decoder{text};
    String result;
    result.reserve(text.size());
    std::size_t characterCount = 0;
    bool lastWasWordSeparator = false;
    decoder.decodeAll([&](const impl::Char character) {
        // No "if (characterCount >= limits::maxNameLength) { ... }", as the initial size check is sufficient.
        if (character == impl::Char::Space || character == impl::Char::Underscore) {
            if (result.empty()) {
                throw Error{
                    ErrorCategory::Syntax,
                    u8"A name must not start with space or underscore."};
            }
            if (lastWasWordSeparator) {
                throw Error{
                    ErrorCategory::Syntax,
                    u8"Two subsequent word separators (space, underscore) are not allowed."};
            }
            lastWasWordSeparator = true;
        } else if (character == impl::CharClass::DecimalDigit) {
            if (result.empty() || (result.size() == 1 && result.at(0) == impl::Char::At)) {
                throw Error{ErrorCategory::Syntax, u8"A name must not start with a number."};
            }
            lastWasWordSeparator = false;
        } else if (character == impl::CharClass::Letter) {
            lastWasWordSeparator = false;
        } else if (characterCount == 0 && character == impl::Char::At) {
            lastWasWordSeparator = false; // Allow the `@` as a first character to create meta-names.
        } else {
            throw Error{
                ErrorCategory::Syntax,
                impl::u8format(u8"Invalid character at position {}", characterCount)};
        }
        character.appendRegularNameTo(result);
        characterCount++;
    });
    if (result.back() == '_') {
        throw Error{ErrorCategory::Syntax, u8"A name must not end with a space or underscore."};
    }
    if (result == u8"@") {
        throw Error{ErrorCategory::Syntax, u8"A meta-name requires at least one letter."};
    }
    return result;
}


void Name::validateText(const String &text) {
    if (text.empty()) {
        throw Error{ErrorCategory::Syntax, u8"Text-names must not be empty."};
    }
    if (text.size() > limits::maxLineLength) {
        throw Error{ErrorCategory::LimitExceeded, u8"The given text-name exceeds the size limit."};
    }
    auto decoder = impl::U8Decoder{text};
    decoder.decodeAll([&](const impl::Char character) {
        if (character != impl::CharClass::ValidLang) {
            throw Error{
                ErrorCategory::Syntax,
                u8"The text-name contains a character that is not allowed in a configuration document."};
        }
    });
}


auto Name::meta(Meta metaName) -> const Name& {
    if (static_cast<std::size_t>(metaName) > allMetaNames().size()) {
        throw std::logic_error{"Unknown meta-name."};
    }
    return allMetaNames()[static_cast<std::size_t>(metaName)];
}


auto Name::metaVersion() -> const Name& {
    return meta(Meta::Version);
}


auto Name::metaSignature() -> const Name& {
    return meta(Meta::Signature);
}


auto Name::metaInclude() -> const Name& {
    return meta(Meta::Include);
}


auto Name::metaFeatures() -> const Name& {
    return meta(Meta::Features);
}


auto Name::allMetaNames() -> const MetaNameArray& {
    // the order of these meta-names must match the enum `Meta`.
    const static auto metaNames = MetaNameArray{
        createRegular(u8"@version"),
        createRegular(u8"@signature"),
        createRegular(u8"@include"),
        createRegular(u8"@features"),
    };
    return metaNames;
}


auto Name::emptyInstance() noexcept -> const Name & {
    static const auto empty = Name{};
    return empty;
}


auto Name::indexDigitCount() const noexcept -> std::size_t {
    auto value = std::get<std::size_t>(_value);
    std::size_t digits = 1;
    for (; value >= 10; value /= 10) {
        ++digits;
    }
    return digits;
}


#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
auto internalView(const Name &object) -> impl::InternalViewPtr {
    auto result = impl::InternalView::create();
    result->setValue("type", std::format("{}", object._type));
    if (std::holds_alternative<std::size_t>(object._value)) {
        result->setValue("index", std::to_string(std::get<std::size_t>(object._value)));
    } else {
        result->setValue("name", std::get<String>(object._value));
    }
    return result;
}
#endif


}

