// Copyright (c) 2024-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Lexer.hpp"


#include "Core.hpp"
#include "Number.hpp"
#include "Section.hpp"
#include "Value.hpp"

#include "../utilities/YieldMacros.hpp"


namespace erbsland::conf::impl {


auto Lexer::sourceIdentifier() const noexcept -> SourceIdentifierPtr {
    if (_decoder == nullptr) {
        return {};
    }
    return _decoder->sourceIdentifier();
}


auto Lexer::tokens() -> TokenGenerator {
    try {
        if (_decoder == nullptr) {
            throw Error{ErrorCategory::Internal, u8"You cannot read from a closed lexer."};
        }
        decoder().initialize();
        // This is the main state. We assume that the read character is always the first character of a new line.
        while (decoder().character() != Char::EndOfData) {
            if (decoder().character() == CharClass::Spacing) {
                // Manually handle spacing to improve the error reporting.
                EL_YIELD(lexer::expectSpacing(decoder())); // read the spacing
                // Now see what we get at this point
                if (decoder().character() == CharClass::EndOfLineStart) {
                    EL_YIELD_FROM(lexer::expectEndOfLine(decoder(), lexer::ExpectMore::No));
                } else {
                    if (decoder().character() == CharClass::NameStart) {
                        decoder().throwSyntaxError(
                            u8"Value names must appear at the beginning of a line without leading spaces.");
                    }
                    if (decoder().character() == CharClass::SectionStart) {
                        decoder().throwSyntaxError(
                            u8"Section declarations must start at the beginning of a line without any indentation.");
                    }
                    decoder().throwSyntaxOrUnexpectedEndError(
                        u8"Unexpected content after indentation: only a comments or an empty lines was expected at this point.");
                }
            } if (decoder().character() == CharClass::EndOfLineStart) {
                EL_YIELD_FROM(lexer::expectEndOfLine(decoder(), lexer::ExpectMore::No));
            } else if (decoder().character() == CharClass::NameStart) {
                EL_YIELD_FROM(lexer::expectNameAndValue(decoder()));
            } else if (decoder().character() == CharClass::SectionStart) { // We got any character that potentially starts a section
                EL_YIELD_FROM(lexer::expectSection(decoder()));
            } else {
                decoder().throwSyntaxError(u8"Expected a section, name or empty line, but got something else.");
            }
        }
        // Always return an end of data token as the last token in the stream.
        EL_YIELD(decoder().createEndOfDataToken());
        close();
        co_return;
    } catch (const Error&) {
        close();
        throw;
    }
}


auto Lexer::digest() const -> Bytes {
    return _digest;
}


auto Lexer::hashAlgorithm() -> crypto::ShaHash::Algorithm {
    return defaults::documentHashAlgorithm;
}


void Lexer::close() noexcept {
    // Store the digest before the decoder is deleted.
    if (_decoder != nullptr) {
        _digest = _decoder->digest();
    }
    _decoder.reset();
}


#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
namespace {
auto escapeRawText(const String &str) noexcept -> String {
    String result;
    for (auto c : str) {
        if (c == u8'"') {
            result += u8"\"";
        } else if (c == u8'\\') {
            result += u8"\\\\";
        } else if (c == u8'\n') {
            result += u8"\\n";
        } else if (c == u8'\r') {
            result += u8"\\r";
        } else if (c == u8'\t') {
            result += u8"\\t";
        } else {
            result.append(c);
        }
    }
    return result;
}
template<typename T>
auto visualizeValue(const T &/*value*/) noexcept -> String {
    throw std::runtime_error("Not implemented");
}
template<>
auto visualizeValue(const NoContent &/*value*/) noexcept -> String {
    return String{u8"No Content"};
}
template<>
auto visualizeValue(const Integer &value) noexcept -> String {
    return String{std::format("Integer: {}", value)};
}
template<>
auto visualizeValue(const Float &value) noexcept -> String {
    return String{std::format("Float: {}", value)};
}
template<>
auto visualizeValue(const bool &value) noexcept -> String {
    return String{std::format("Boolean: {}", value)};
}
template<>
auto visualizeValue(const String &value) noexcept -> String {
    auto escapedText = String{u8"String: \""};
    escapedText.append(escapeRawText(value));
    escapedText.append(u8"\"");
    return escapedText;
}
template<>
auto visualizeValue(const Date &value) noexcept -> String {
    return String{u8"Date: "} + value.toText();
}
template<>
auto visualizeValue(const Time &value) noexcept -> String {
    return String{u8"Time: "} + value.toText();
}
template<>
auto visualizeValue(const DateTime &value) noexcept -> String {
    return String{u8"Date/Time: "} + value.toText();
}
template<>
auto visualizeValue(const Bytes &value) noexcept -> String {
    auto hexText = String{"Bytes: "};
    const auto displayElements = std::min(static_cast<std::size_t>(32), static_cast<std::size_t>(value.size()));
    for (std::size_t i = 0; i < displayElements; ++i) {
        hexText.append(std::format("{:02x} ", static_cast<uint32_t>(value[i])));
    }
    if (value.size() > displayElements) {
        hexText.append(std::format("... ({} more bytes)", value.size() - displayElements));
    }
    return hexText;
}
template<>
auto visualizeValue(const TimeDelta &value) noexcept -> String {
    return String{u8"Time Delta: "} + value.toText();
}
}


auto internalView(const Lexer &object) -> InternalViewPtr {
    auto result = InternalView::create();
    if (object._decoder != nullptr) {
        result->setValue(u8"decoder", *object._decoder);
    } else {
        result->setValue(u8"decoder", u8"null");
    }
    return result;
}


// The internal view of the LexerToken is defined here avoiding linker warnings about empty units.
auto internalView(const LexerToken &token) noexcept -> InternalViewPtr {
    auto result = InternalView::create();
    result->setValue(u8"type", std::format("{}", token._type));
    if (token._begin.isUndefined()) {
        result->setValue(u8"begin", u8"undefined");
    } else {
        result->setValue(u8"begin", std::format("{}:{}", token._begin.line(), token._begin.column()));
    }
    if (token._end.isUndefined()) {
        result->setValue(u8"end", u8"undefined");
    } else {
        result->setValue(u8"end", std::format("{}:{}", token._end.line(), token._end.column()));
    }
    auto rawText = String{"\""};
    rawText.append(escapeRawText(token._rawText));
    rawText.append(u8"\"");
    result->setValue(u8"rawText", rawText);
    std::visit([&](auto &&value) {
        result->setValue(u8"value", visualizeValue(value));
    }, token._content);
    return result;
}
#endif


}

