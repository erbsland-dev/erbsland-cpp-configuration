// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "AssignmentStream.hpp"


#include "../utf8/U8Decoder.hpp"
#include "../utf8/U8Format.hpp"


namespace erbsland::conf::impl {


auto AssignmentStream::create(LexerPtr lexer) -> AssignmentStreamPtr  {
    return std::make_shared<AssignmentStream>(std::move(lexer), PrivateTag{});
}


AssignmentStream::AssignmentStream(LexerPtr lexer, PrivateTag)
:
    _lexer(std::move(lexer)) {

    assert(_lexer != nullptr);
}


auto AssignmentStream::assignments() -> AssignmentGenerator {
    initialize();
    // Read tokens until the end of the document is reached.
    while (token().type() != TokenType::EndOfData) {
        switch (token().type().raw()) {
        case TokenType::LineBreak:
        case TokenType::Indentation:
        case TokenType::Spacing:
        case TokenType::Comment:
            next(); // Consume empty lines, comments, indentation and spacing.
            break;
        case TokenType::MetaName:
            co_yield handleMetaValue();
            break;
        case TokenType::RegularName:
        case TokenType::TextName:
            co_yield handleValue();
            break;
        case TokenType::SectionMapOpen:
        case TokenType::SectionListOpen:
            co_yield handleSection();
            break;
        default:
            // Coverage: The lexer catches most errors, probably never used.
            throwSyntaxError(u8"Expected a section or named value, but got something else.");
        }
    }
    co_yield Assignment{}; // signal the end of the document.
    co_return;
}


void AssignmentStream::initialize() {
    _documentArea = DocumentArea::Root; // make it explicit.
    _currentSectionPath = {};
    _lexerGenerator = _lexer->tokens();
    _tokenIterator = _lexerGenerator.begin();
    _tokenIteratorEnd = _lexerGenerator.end();
    // Start with the first token from the token stream.
    if (_tokenIterator != _tokenIteratorEnd) {
        _token = *_tokenIterator;
        ++_tokenIterator;
    } else {
        // If the stream is unexpectedly empty, make sure the initial token is the end-of-data token.
        _token = LexerToken{TokenType::EndOfData};
    }
}


void AssignmentStream::next() {
    while (_tokenIterator != _tokenIteratorEnd) {
        _token = *_tokenIterator;
        ++_tokenIterator;

        if (_token.type() != TokenType::Spacing && _token.type() != TokenType::Comment) {
            return; // Found a meaningful token.
        }
    }

    // If we reached the end, set to EndOfData.
    _token = LexerToken{TokenType::EndOfData};
}


void AssignmentStream::expectNext() {
    next();
    if (token().type() == TokenType::EndOfData) {
        throwUnexpectedEndError(u8"Unexpected end of the document.");
    }
    if (token().type() == TokenType::LineBreak) {
        throwUnexpectedEndError(u8"Unexpected end of the the line.");
    }
}


void AssignmentStream::nextAndVerify(const TokenType expectedTokenType, const std::u8string_view errorMessage) {
    next();
    if (token().type() == TokenType::EndOfData) {
        throwUnexpectedEndError(u8"Unexpected end of the document.");
    }
    if (expectedTokenType != TokenType::LineBreak && token().type() == TokenType::LineBreak) {
        throwSyntaxError(u8"Unexpected end of the the line.");
    }
    if (token().type() != expectedTokenType) {
        throwSyntaxError(errorMessage);
    }
}


void AssignmentStream::verifyAndConsumeEndOfLine() {
    if (token().type() == TokenType::EndOfData) {
        return; // accept the end of the token stream.
    }
    if (token().type() == TokenType::LineBreak) {
        next(); // Consume the line-break token.
        return;
    }
    throwSyntaxError(u8"Expected the end of the line, or the end of the document.");
}


auto AssignmentStream::token() const -> const LexerToken & {
    return _token;
}


void AssignmentStream::throwSyntaxError(const std::u8string_view message) const {
    throw Error(ErrorCategory::Syntax, message, currentLocation());
}


void AssignmentStream::throwSyntaxError(const std::u8string_view message, const NamePath &namePath) const {
    throw Error(ErrorCategory::Syntax, message, currentLocation(), namePath);
}


void AssignmentStream::throwUnsupportedError(const std::u8string_view message) const {
    throw Error(ErrorCategory::Unsupported, message, currentLocation());
}


void AssignmentStream::throwUnexpectedEndError(const std::u8string_view message) const {
    throw Error(ErrorCategory::UnexpectedEnd, message, currentLocation());
}


void AssignmentStream::throwLimitExceededError(std::u8string_view message) const {
    throw Error(ErrorCategory::LimitExceeded, message, currentLocation());
}


auto AssignmentStream::currentLocation() const noexcept -> Location {
    return Location{_lexer->sourceIdentifier(), _token.begin()};
}


auto AssignmentStream::handleMetaValue() -> Assignment {
    auto name = Name{
        NameType::Regular,
        std::get<String>(token().content()),
        PrivateTag{}};
    const auto nameLocation = currentLocation();
    if (std::ranges::find(Name::allMetaNames(), name) == Name::allMetaNames().end()) {
        throwSyntaxError(u8"Unknown meta value name.", name);
    }
    if (name == Name::metaSignature() && token().begin().line() > 1) {
        throwSyntaxError(u8"Signature must be defined in the first line of the document.");
    }
    if ((name == Name::metaVersion() || name == Name::metaFeatures()) && _documentArea != DocumentArea::Root) {
        throwSyntaxError(u8"The version and features must be defined before the first section.");
    }
    expectNext(TokenType::NameValueSeparator);
    nextAndVerify(TokenType::Text, u8"Only single-line text is supported for a meta value or command.");
    auto text = std::get<String>(token().content());
    next();
    if (token().type() == TokenType::ValueListSeparator) {
        throwSyntaxError(u8"Only single text value is supported for a meta value or command.");
    }
    verifyAndConsumeEndOfLine();
    if (name == Name::metaVersion()) {
        if (_readMetaVersion) {
            throwSyntaxError(u8"The '@version' meta-value must be defined only once.");
        }
        if (text != defaults::languageVersion) {
            throwUnsupportedError(u8"This parser only supports version 1.0 of the configuration language.");
        }
        _readMetaVersion = true;
    } else if (name == Name::metaFeatures()) {
        if (_readMetaFeatures) {
            throwSyntaxError(u8"The '@features' meta-value must be defined only once.");
        }
        verifyFeatures(text);
        _readMetaFeatures = true;
    } else if (name == Name::metaInclude()) { // include
        // After each @include, reset the section path.
        _lastAbsolutePath = {};
        _currentSectionPath = {};
    }
    return Assignment{
        AssignmentType::MetaValue,
        NamePath(std::move(name)),
        nameLocation,
        Value::createText(std::move(text))};
}


auto AssignmentStream::handleValue() -> Assignment {
    const bool isTextName = (token().type() == TokenType::TextName);
    auto name = Name{ // The name is already checked and normalized by the lexer.
        isTextName ? NameType::Text : NameType::Regular,
        std::get<String>(token().content()),
        PrivateTag{}};
    auto nameLocation = currentLocation();
    const auto createAssignment = [&](ValuePtr &&value) noexcept -> Assignment {
        value->setLocation(nameLocation); // copy
        auto namePath = _currentSectionPath;
        namePath.append(name);
        return Assignment{
            AssignmentType::Value,
            std::move(namePath),
            std::move(nameLocation),
            value
        };
    };

    expectNext(TokenType::NameValueSeparator);
    next(); // get either a line-break or the start of a value.
    if (token().type() == TokenType::LineBreak) {
        // If we got a line-break, the value must be indented on the next line.
        expectNext(TokenType::Indentation);
        expectNext();
    }
    switch (token().type().raw()) {
    case TokenType::Integer: case TokenType::Float: case TokenType::Boolean:
    case TokenType::Text: case TokenType::Code: case TokenType::RegEx:
    case TokenType::Date: case TokenType::DateTime: case TokenType::Time: case TokenType::TimeDelta:
    case TokenType::Bytes: {
        auto valueList = handleValueOrValueList();
        if (valueList.size() == 1) {
            return createAssignment(std::move(valueList.front()));
        }
        auto value = Value::createValueList(std::move(valueList));
        return createAssignment(std::move(value));
    }
    case TokenType::MultiLineValueListSeparator: {
        auto valueList = handleMultiLineValueList();
        if (valueList.size() == 1) {
            return createAssignment(std::move(valueList.front()));
        }
        auto value = Value::createValueList(std::move(valueList));
        return createAssignment(std::move(value));
    }
    case TokenType::MultiLineTextOpen:
    case TokenType::MultiLineCodeOpen: {
        auto text = handleMultiLineText();
        auto value = Value::createText(std::move(text));
        return createAssignment(std::move(value));
    }
    case TokenType::MultiLineRegexOpen: {
        auto text = handleMultiLineRegEx();
        auto value = Value::createRegEx(RegEx{std::move(text), true});
        return createAssignment(std::move(value));
    }
    case TokenType::MultiLineBytesOpen: {
        auto data = handleMultiLineBytes();
        auto value = Value::createBytes(std::move(data));
        return createAssignment(std::move(value));
    }
    default:
        throw Error(ErrorCategory::Internal, u8"Unexpected token for value.");
    }
}


auto AssignmentStream::handleValueOrValueList() -> std::vector<ValuePtr> {
    std::vector<ValuePtr> valueList;
    ValuePtr value;
    while (!(token().type() == TokenType::LineBreak || token().type() == TokenType::EndOfData)) {
        switch (token().type().raw()) {
        case TokenType::Integer:
            value = Value::createInteger(std::get<Integer>(token().content()));
            break;
        case TokenType::Float:
            value = Value::createFloat(std::get<Float>(token().content()));
            break;
        case TokenType::Boolean:
            value = Value::createBoolean(std::get<bool>(token().content()));
            break;
        case TokenType::Text:
        case TokenType::Code:
            value = Value::createText(std::get<String>(token().content()));
            break;
        case TokenType::RegEx:
            value = Value::createRegEx(RegEx{std::get<String>(token().content()), false});
            break;
        case TokenType::Date:
            value = Value::createDate(std::get<Date>(token().content()));
            break;
        case TokenType::DateTime:
            value = Value::createDateTime(std::get<DateTime>(token().content()));
            break;
        case TokenType::Time:
            value = Value::createTime(std::get<Time>(token().content()));
            break;
        case TokenType::TimeDelta:
            value = Value::createTimeDelta(std::get<TimeDelta>(token().content()));
            break;
        case TokenType::Bytes:
            value = Value::createBytes(std::get<Bytes>(token().content()));
            break;
        default:
            throw Error(ErrorCategory::Internal, u8"Unexpected token type for value.");
        }
        value->setLocation(currentLocation());
        valueList.emplace_back(std::move(value));
        next(); // Consume the value
        if (token().type() == TokenType::ValueListSeparator) {
            next(); // Consume the seperator
        }
    }
    verifyAndConsumeEndOfLine();
    return valueList;
}


auto AssignmentStream::handleMultiLineValueList() -> std::vector<ValuePtr> {
    std::vector<ValuePtr> valueList;
    while (token().type() == TokenType::MultiLineValueListSeparator) {
        auto bulletLocation = currentLocation();
        expectNext(); // consume the list separator.
        auto subValueList = handleValueOrValueList();
        ValuePtr value;
        if (subValueList.size() == 1) {
            value = subValueList.front();
        } else {
            value = Value::createValueList(std::move(subValueList));
        }
        value->setLocation(bulletLocation);
        valueList.emplace_back(std::move(value));
        if (token().type() != TokenType::Indentation) {
            // When the next line doesn't start with an indentation, the multi-line list ends here.
            // Empty lines are not allowed in multi-line lists.
            break;
        }
        expectNext(TokenType::MultiLineValueListSeparator); // Consume the indentation and expect the next bullet
    }
    return valueList;
}


auto AssignmentStream::handleMultiLineText() -> String {
    expectNext(TokenType::LineBreak, TokenType::MultiLineCodeLanguage); // Consume the open sequence.
    if (token().type() == TokenType::MultiLineCodeLanguage) {
        expectNext(TokenType::LineBreak); // Consume the code language token.
    }
    expectNext(TokenType::Indentation, TokenType::LineBreak);
    bool isSecondLine = false;
    String text;
    while (token().type() == TokenType::Indentation || token().type() == TokenType::LineBreak) {
        // If the line is just a line-break, it is an empty line.
        if (token().type() != TokenType::LineBreak) {
            // If it isn't a line-break, it is an indentation.
            expectNext(TokenType::MultiLineCode, TokenType::MultiLineText,
                TokenType::MultiLineTextClose, TokenType::MultiLineCodeClose,
                TokenType::LineBreak);
            if (token().type() == TokenType::MultiLineTextClose || token().type() == TokenType::MultiLineCodeClose) {
                next(); // Consume the close sequence.
                verifyAndConsumeEndOfLine();
                break;
            }
            if (isSecondLine) {
                text.append(u8"\n");
            }
            if (token().type() != TokenType::LineBreak) {
                text.append(std::get<String>(token().content())); // append the text
                expectNext(TokenType::LineBreak);
            }
        } else {
            // empty line
            if (isSecondLine) {
                text.append(u8"\n");
            }
        }
        expectNext(TokenType::LineBreak, TokenType::Indentation);
        isSecondLine = true;
    }
    return text;
}


auto AssignmentStream::handleMultiLineRegEx() -> String {
    // Consume the open sequence and expect an indentation or an empty line.
    expectNext(TokenType::LineBreak);
    expectNext(TokenType::Indentation, TokenType::LineBreak);
    bool isSecondLine = false;
    String text;
    while (token().type() == TokenType::Indentation || token().type() == TokenType::LineBreak) {
        if (token().type() != TokenType::LineBreak) {
            expectNext(TokenType::MultiLineRegex, TokenType::MultiLineRegexClose, TokenType::LineBreak);
            if (token().type() == TokenType::MultiLineRegexClose) {
                next(); // Consume the close sequence.
                verifyAndConsumeEndOfLine();
                break;
            }
            if (isSecondLine) {
                text.append(u8"\n");
            }
            if (token().type() != TokenType::LineBreak) {
                text.append(std::get<String>(token().content())); // append the text
                expectNext(TokenType::LineBreak);
            }
        } else {
            // empty line
            if (isSecondLine) {
                text.append(u8"\n");
            }
        }
        expectNext(TokenType::LineBreak, TokenType::Indentation);
        isSecondLine = true;
    }
    return text;
}


auto AssignmentStream::handleMultiLineBytes() -> Bytes {
    // Consume the open sequence and expect, skip the format and expect an empty line or indentation.
    expectNext(TokenType::LineBreak, TokenType::MultiLineBytesFormat);
    if (token().type() == TokenType::MultiLineBytesFormat) {
        expectNext(TokenType::LineBreak); // Consume the format (ignored, as only hex is supported).
    }
    expectNext(TokenType::Indentation, TokenType::LineBreak);
    Bytes result;
    while (token().type() == TokenType::Indentation || token().type() == TokenType::LineBreak) {
        if (token().type() != TokenType::LineBreak) {
            expectNext(TokenType::MultiLineBytes, TokenType::MultiLineBytesClose, TokenType::LineBreak);
            if (token().type() == TokenType::MultiLineBytesClose) {
                next(); // Consume the close sequence.
                verifyAndConsumeEndOfLine();
                break;
            }
            if (token().type() != TokenType::LineBreak) {
                result.append(std::get<Bytes>(token().content())); // append the text
                expectNext(TokenType::LineBreak);
            }
        }
        expectNext(TokenType::Indentation, TokenType::LineBreak);
    }
    return result;
}


auto AssignmentStream::handleSection() -> Assignment {
    const bool isSectionList = token().type() == TokenType::SectionListOpen;
    bool isRelativePath = false;
    auto openLocation = currentLocation(); // Store the location where the section definition starts.
    expectNext(TokenType::NamePathSeparator, TokenType::RegularName, TokenType::TextName);
    NamePath namePath;
    if (token().type() == TokenType::NamePathSeparator) {
        // If the name starts with a name seperator, this describes a relative path.
        isRelativePath = true;
        expectNext(TokenType::RegularName, TokenType::TextName);
    }
    while (!(token().type() == TokenType::SectionListClose || token().type() == TokenType::SectionMapClose)) {
        if (namePath.size() >= limits::maxNamePathLength) {
            throwLimitExceededError(u8"A name path must not exceed 10 name components.");
        }
        namePath.append(Name{
            token().type() == TokenType::RegularName ? NameType::Regular : NameType::Text,
            std::get<String>(token().content()),
            PrivateTag{}});
        expectNext(TokenType::NamePathSeparator, TokenType::SectionListClose, TokenType::SectionMapClose);
        if (token().type() != TokenType::NamePathSeparator) {
            break; // if we didn't get a seperator, the section is closed.
        }
        expectNext(TokenType::RegularName, TokenType::TextName);
    }
    next(); // Consume the section closing sequence.
    verifyAndConsumeEndOfLine();
    // Handle relative paths at the end for better error reporting.
    if (isRelativePath) {
        if (_lastAbsolutePath.empty()) {
            throw Error{
                ErrorCategory::Syntax,
                u8"There is no absolute section definition before this relative one.",
                openLocation,
                namePath
            };
        }
        namePath.prepend(_lastAbsolutePath);
    } else {
        _lastAbsolutePath = namePath;
    }
    _currentSectionPath = namePath;
    _documentArea = DocumentArea::AfterSection;
    return Assignment{
        isSectionList ? AssignmentType::SectionList : AssignmentType::SectionMap,
        std::move(namePath),
        std::move(openLocation),
        {} // no value for sections.
    };
}


void AssignmentStream::verifyFeatures(const String &text) const {
    static const auto supportedFeatures = std::array<String, 19>{
        u8"core", u8"minimum", u8"standard", u8"advanced", u8"all",
        u8"float", u8"byte-count", u8"multi-line", u8"section-list", u8"value-list", u8"text-names",
        u8"date-time", u8"code", u8"byte-data", u8"include", u8"regex", u8"time-delta",
        u8"validation", u8"signature",
    };
    std::vector<String> requestedFeatures;
    String currentFeature;
    std::size_t currentCharacterIndex = 0;
    auto decoder = U8Decoder<const char8_t>{std::span(text.raw().data(), text.raw().size())};
    decoder.decodeAll([&](const Char character) {
        if (character == CharClass::Spacing) {
            // Add the current read feature and clear the string.
            if (!currentFeature.empty()) {
                requestedFeatures.emplace_back(currentFeature);
                currentFeature.clear();
            }
        } else if (character == CharClass::Letter || character == Char::Minus) {
            character.appendLowerCaseTo(currentFeature);
        } else {
            throw Error{
                ErrorCategory::Syntax,
                u8format(u8"Unsupported character in @features text at index {}.", currentCharacterIndex),
                currentLocation(),
                NamePath{Name::createRegular(u8"@features")}
            };
        }
        ++currentCharacterIndex;
    });
    if (!currentFeature.empty()) {
        requestedFeatures.emplace_back(currentFeature);
    }
    for (const auto &feature : requestedFeatures) {
        if (std::ranges::find(supportedFeatures, feature) == supportedFeatures.end()) {
            throw Error{
                ErrorCategory::Unsupported,
                u8format(u8"This parser does not support the feature '{}'.", feature),
                currentLocation()
            };
        }
    }
    // At this point, all features are successfully verified.
}


}

