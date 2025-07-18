// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Decoder.hpp"
#include "TransactionStack.hpp"

#include "../Char.hpp"
#include "../Defaults.hpp"
#include "../Limits.hpp"

#include "../utf8/U8Decoder.hpp"


namespace erbsland::conf::impl {



/// A minimal decoder for the name lexer.
///
/// @tested `FastNameDecoderTest`
///
class FastNameDecoder final : public Decoder {
    using Buffer = std::span<const char8_t>;
    using CapturedChars = std::vector<Char>;

public:
    /// Create a new instance of the name decoder.
    ///
    /// @param buffer The buffer to decode.
    ///
    explicit FastNameDecoder(const Buffer buffer) : _buffer{buffer} {
        // remove a trailing zero mark.
        if (!_buffer.empty() && _buffer.back() == '\0') {
            _buffer = _buffer.subspan(0, _buffer.size() - 1);
        }
    }

    // defaults
    ~FastNameDecoder() override = default;
    FastNameDecoder(const FastNameDecoder &) = delete;
    FastNameDecoder(FastNameDecoder &&) = delete;
    auto operator=(const FastNameDecoder &) -> FastNameDecoder& = delete;
    auto operator=(FastNameDecoder &&) -> FastNameDecoder& = delete;

public: // implement Decoder
    void initialize() override {
        if (_buffer.empty()) {
            _currentChar = Char::EndOfData;
            return;
        }
        _charIndex = 0;
        _readIndex = 0;
        _currentChar = U8Decoder<const char8_t>::decodeChar(_buffer, _readIndex);
    }

    [[nodiscard]] auto character() const noexcept -> const Char& override {
        return _currentChar;
    }

    [[nodiscard]] auto location() const -> Location override {
        return Location{
            SourceIdentifier::create(defaults::namePathIdentifier, {}),
            Position{1, static_cast<int>(_readIndex)}};
    }

    [[nodiscard]] auto sourceIdentifier() const noexcept -> SourceIdentifierPtr override {
        return SourceIdentifier::create(defaults::namePathIdentifier, {});
    }

    void next() override {
        _charIndex = _readIndex;
        if (_readIndex >= _buffer.size()) {
            _currentChar = Char::EndOfData;
            return;
        }
        _currentChar = U8Decoder<const char8_t>::decodeChar(_buffer, _readIndex);
    }

public:
    /// Access the buffer
    ///
    [[nodiscard]] auto buffer() const noexcept -> const Buffer& { return _buffer; }

    /// Check if there is more.
    ///
    [[nodiscard]] auto hasNext() const noexcept -> bool {
        return _readIndex < _buffer.size();
    }

public: // implement TransactionHandler
    [[nodiscard]] auto startTransaction(Transaction *transaction) noexcept -> std::size_t override {
        assert(transaction != nullptr);
        _transactions.push_back(transaction);
        return _charIndex;
    }

    void commitTransaction(const Transaction *transaction) noexcept override {
        // Make sure the correct transaction is committed.
        assert(transaction != nullptr);
        assert(!_transactions.empty());
        assert(_transactions.back() == transaction);
        assert(transaction->state() == Transaction::State::Committed);
        // Remove the now obsolete transaction.
        popTransaction();
    }

    void rollbackTransaction(const Transaction *transaction) noexcept override {
        // Make sure the correct transaction is rolled back.
        assert(transaction != nullptr);
        assert(!_transactions.empty());
        assert(_transactions.back() == transaction);
        assert(transaction->state() == Transaction::State::RolledBack);
        _readIndex = transaction->transactionBufferStartIndex();
        _charIndex = _readIndex;
        _currentChar = U8Decoder<const char8_t>::decodeChar(_buffer, _readIndex);
        popTransaction();
    }

    [[nodiscard]] auto transactionCapturedSize(const Transaction *transaction) noexcept -> std::size_t override {
        assert(transaction != nullptr);
        assert(!_transactions.empty());
        assert(transaction->state() == Transaction::State::Open);
        return _charIndex - transaction->transactionBufferStartIndex();
    }

    [[nodiscard]] auto captureTransactionContent(
        const Transaction *transaction,
        const CaptureFn &captureFn) const noexcept -> String override {

        assert(captureFn != nullptr);
        assert(transaction != nullptr);
        assert(transaction->state() == Transaction::State::Open);
        const auto startIndex = transaction->transactionBufferStartIndex();
        const auto capturedSpan = _buffer.subspan(startIndex, _charIndex - startIndex);
        U8Decoder u8Decoder{capturedSpan};
        String result;
        result.reserve(capturedSpan.size());
        u8Decoder.decodeAll([&result, &captureFn](const Char &character) {
            captureFn(result, character);
        });
        return result;
    }

    void popTransaction() noexcept {
        assert(!_transactions.empty());
        _transactions.pop_back();
    }

private:
    Buffer _buffer; ///< The buffer to decode.
    Char _currentChar; ///< The current character.
    std::size_t _charIndex{}; ///< The index of the current character.
    std::size_t _readIndex{}; ///< The current read index.
    TransactionStack _transactions; ///< A stack with transactions.
};



}


