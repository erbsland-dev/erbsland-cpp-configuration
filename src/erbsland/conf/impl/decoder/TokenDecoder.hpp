// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Decoder.hpp"
#include "TokenTransactionBuffer.hpp"
#include "Transaction.hpp"
#include "TransactionStack.hpp"

#include "../CharStream.hpp"
#include "../LexerToken.hpp"

#include <cassert>
#include <deque>
#include <memory>
#include <vector>


namespace erbsland::conf::impl {


class TokenDecoder;
using TokenDecoderPtr = std::shared_ptr<TokenDecoder>;


/// A wrapper around a decoder tailored for decoding tokens.
///
/// - Adding transactions
/// - Adding exception helpers.
/// - Adding indentation states.
///
/// @tested `TokenDecoderTest`
///
class TokenDecoder final : public Decoder {
public:
    using CharacterBuffer = std::deque<DecodedChar>;

public:
    static auto create(CharStreamPtr decoder) noexcept -> TokenDecoderPtr {
        return std::make_shared<TokenDecoder>(std::move(decoder));
    }

    explicit TokenDecoder(CharStreamPtr decoder) noexcept : _decoder{std::move(decoder)} {
        assert(_decoder != nullptr);
    }

    // defaults
    TokenDecoder() = default;
    ~TokenDecoder() override = default;

public: // implement Decoder
    void initialize() override {
        _transactionBuffer.reserve(limits::maxLineLength);
        nextToken();
    }

    [[nodiscard]] auto character() const noexcept -> const Char& override {
        return _currentCharacter;
    }

    [[nodiscard]] auto location() const -> Location override {
        return Location{_decoder->source()->identifier(), characterPosition()};
    }

    [[nodiscard]] auto sourceIdentifier() const noexcept -> SourceIdentifierPtr override {
        return _decoder->source()->identifier();
    }

    void next() override;

    /// Check for an error and throw it.
    ///
    /// Exceptions from the lower layers (like character-, or encoding-errors) would propagate faster
    /// than the actual yield of tokens. Therefore, the point of a such error is marked using an "Error"-character
    /// that caused the exception being rethrown *after* the last successfully parsed token.
    ///
    /// Calling this method checks if the current character contains the error mark. In this case, an exception with
    /// the details found in `_currentError` is thrown.
    ///
    void checkForErrorAndThrowIt() const override;

public:
    /// Get the position of the current character.
    ///
    [[nodiscard]] auto characterPosition() const -> Position {
        return _currentCharacter.position();
    }

    /// Get the digest from the decoder.
    ///
    /// Must be called *after* receiving the end-of-data token to get the digest of the document.
    ///
    /// @return The digest for the document, or empty if none was created.
    ///
    [[nodiscard]] auto digest() const noexcept -> Bytes {
        return _decoder->digest();
    }

    /// Move to the next character and start a new token.
    ///
    void nextToken() {
        next();
        resetTokenStartPosition();
    }

    /// Access the token start position.
    ///
    [[nodiscard]] auto tokenStartPosition() const noexcept -> const Position& {
        return _tokenStartPosition;
    }

    /// Reset the token start position.
    ///
    void resetTokenStartPosition() {
        _tokenStartPosition = characterPosition();
    }

    /// Get the current token size in characters.
    ///
    /// @note Only works for single line tokens.
    ///
    [[nodiscard]] auto tokenSize() const noexcept -> int {
        assert(characterPosition().line() == _tokenStartPosition.line());
        return characterPosition().column() - _tokenStartPosition.column();
    }

public: // Constraining functions.
    /// Expect more content in the current line.
    ///
    void expectMoreInLine(const std::u8string_view message) const {
        if (_currentCharacter == CharClass::LineBreak) {
            throwSyntaxError(message);
        }
        if (_currentCharacter == Char::EndOfData) {
            throwUnexpectedEndOfDataError(message);
        }
    }

public: // create tokens from captured content
    /// Generic helper to create a token, capturing the current positions and raw text.
    ///
    template<typename T>
    [[nodiscard]] auto createToken(TokenType type, T &&value) -> LexerToken {
        // Capture all text up to the current character or the end of the document.
        auto capturedText = (
            _currentCharacter == Char::EndOfData ?
            _decoder->captureToEndOfLine() :
            _decoder->captureTo(_currentCharacter.index()));
        // Create the token and reset the start position.
        auto token = LexerToken{
            type,
            tokenStartPosition(),
            characterPosition(),
            std::move(capturedText),
            std::forward<T>(value)};
        resetTokenStartPosition();
        return token;
    }

    /// Create a token with no content.
    ///
    [[nodiscard]] auto createToken(TokenType type) -> LexerToken {
        return createToken(type, NoContent{});
    }

    /// Create the end-of-line token.
    ///
    [[nodiscard]] auto createEndOfLineToken() -> LexerToken {
        auto token = LexerToken{
            TokenType::LineBreak,
            tokenStartPosition(),
            characterPosition(),
            _decoder->captureToEndOfLine(),
            NoContent{}};
        nextToken();
        return token;
    }

    /// Create the end-of-data token.
    ///
    [[nodiscard]] auto createEndOfDataToken() -> LexerToken {
        return LexerToken{TokenType::EndOfData, {}, {}, {}, NoContent{}};
    }

public: // indentation handling.
    [[nodiscard]] auto hasIndentationPattern() const noexcept -> bool {
        return !_currentIndentationPattern.empty();
    }
    [[nodiscard]] auto indentationPattern() const noexcept -> const String& {
        return _currentIndentationPattern;
    }
    void setIndentationPattern(const String &pattern) noexcept {
        _currentIndentationPattern = pattern;
    }
    void clearIndentationPattern() noexcept {
        _currentIndentationPattern.clear();
    }

public: // implement TransactionHandler
    [[nodiscard]] auto startTransaction(Transaction *transaction) noexcept -> std::size_t override;
    void commitTransaction(const Transaction *transaction) noexcept override;
    void rollbackTransaction(const Transaction *transaction) noexcept override;
    [[nodiscard]] auto transactionCapturedSize(const Transaction *transaction) noexcept -> std::size_t override;
    [[nodiscard]] auto captureTransactionContent(const Transaction *transaction, const CaptureFn &captureFn) const noexcept -> String override;
    void popTransaction() noexcept;

public: // testing
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const TokenDecoder &object) -> InternalViewPtr {
        auto result = InternalView::create();
        result->setValue("decoder", *object._decoder);
        result->setValue("currentCharacter", object._currentCharacter);
        result->setValue("characterBuffer", InternalView::createList(10, object._characterBuffer.begin(), object._characterBuffer.end()));
        result->setValue("tokenStartPosition", object._tokenStartPosition);
        result->setValue("transactions", InternalView::createList(10, object._transactions.begin(), object._transactions.end()));
        result->setValue("currentIndentationPattern", object._currentIndentationPattern);
        auto currentError = InternalView::create();
        currentError->setValue("category", object._currentError.category.toText());
        currentError->setValue("message", object._currentError.message);
        currentError->setValue("location", object._currentError.location);
        result->setValue("currentError", currentError);
        return result;
    }
#endif

private:
    CharStreamPtr _decoder; ///< The wrapped decoder.
    DecodedChar _currentCharacter{Char::EndOfData, {}, {}}; ///< The current decoded character.
    CharacterBuffer _characterBuffer; ///< A character buffer to allow rollback of transactions.
    Position _tokenStartPosition; ///< The start position of the current token.
    TransactionStack _transactions; ///< A stack with transactions.
    TokenTransactionBuffer _transactionBuffer; ///< A buffer to stored the characters captured in a transaction.
    String _currentIndentationPattern; ///< The current indentation pattern.
    bool _hasUpcomingError{false}; ///< Set to `true` if an delayed error was scheduled.
    struct {
        ErrorCategory category; ///< The category.
        String message; ///< The message.
        Location location; ///< The location.
    } _currentError; ///< Details in case we got an error in the stream.
};


}


