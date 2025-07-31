// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TokenDecoder.hpp"


#include <algorithm>
#include <ranges>


namespace erbsland::conf::impl {


void TokenDecoder::next() {
    if (_currentCharacter == Char::Error) {
        throw std::logic_error("TokenDecoder: An error was not correctly handled.");
    }
    try {
        // Capture and/or store the current character.
        if (!_transactions.empty()) {
            if (_currentCharacter == CharClass::LineBreakOrEnd) {
                throwInternalError(u8"There is an open transaction at the end of the line.");
            }
            _transactionBuffer.emplace_back(_currentCharacter);
        }
        // Read the next character.
        if (_characterBuffer.empty()) {
            _currentCharacter = _decoder->next();
        } else {
            _currentCharacter = _characterBuffer.back();
            _characterBuffer.pop_back();
        }
     } catch (const Error& error) {
         if (error.category() == ErrorCategory::Encoding || error.category() == ErrorCategory::Character) {
             // Delay encoding and (control-)character errors by setting the current character to the error mark.
             _hasUpcomingError = true;
             _currentCharacter = DecodedChar{Char::Error, _decoder->lastCharacterStartIndex(), error.location().position()};
             _currentError.category = error.category();
             _currentError.message = error.message();
             _currentError.location = error.location();
         } else {
             // Throw all other errors (IO, Internal) immediately.
             throw;
         }
     }
}


void TokenDecoder::checkForErrorAndThrowIt() const {
    if (_hasUpcomingError) {
        throw Error{_currentError.category, _currentError.message, _currentError.location};
    }
}


auto TokenDecoder::startTransaction(Transaction *transaction) noexcept -> std::size_t {
    assert(transaction != nullptr);
    _transactions.push_back(transaction);
    return _transactionBuffer.size();
}


void TokenDecoder::commitTransaction(const Transaction *transaction) noexcept {
    // Make sure the correct transaction is committed.
    assert(transaction != nullptr);
    assert(!_transactions.empty());
    assert(_transactions.back() == transaction);
    assert(transaction->state() == Transaction::State::Committed);
    // Remove the now obsolete transaction.
    popTransaction();
}


void TokenDecoder::rollbackTransaction(const Transaction *transaction) noexcept {
    // Make sure the correct transaction is rolled back.
    assert(transaction != nullptr);
    assert(!_transactions.empty());
    assert(_transactions.back() == transaction);
    assert(transaction->state() == Transaction::State::RolledBack);
    // Push the current character to the stack, as it will be replaced with then one when the
    // given transaction started.
    _characterBuffer.push_back(_currentCharacter);
    // Push all characters back into the buffer.
    for (std::size_t i = _transactionBuffer.size(); i > transaction->transactionBufferStartIndex(); --i) {
        _characterBuffer.emplace_back(_transactionBuffer.at(i - 1));
    }
    // Erase all characters from the buffer, that were stored in the given transaction.
    assert(_transactionBuffer.size() >= transaction->transactionBufferStartIndex());
    _transactionBuffer.erase(
        _transactionBuffer.begin() + static_cast<TokenTransactionBuffer::difference_type>(transaction->transactionBufferStartIndex()),
        _transactionBuffer.end());
    // Remove the now obsolete transaction.
    popTransaction();
    // Replace the current character with the one on top of the stack.
    // As we put the current character to the stack, we are sure there is at least one character on the stack.
    _currentCharacter = _characterBuffer.back();
    _characterBuffer.pop_back();
}


auto TokenDecoder::transactionCapturedSize(const Transaction *transaction) noexcept -> std::size_t {
    assert(transaction != nullptr);
    assert(!_transactions.empty());
    assert(transaction->state() == Transaction::State::Open);
    return _transactionBuffer.size() - transaction->transactionBufferStartIndex();
}


auto TokenDecoder::captureTransactionContent(
    const Transaction *transaction,
    const CaptureFn &captureFn) const noexcept -> String {

    // Captured characters are only accessible for open transactions.
    assert(captureFn != nullptr);
    assert(transaction != nullptr);
    assert(transaction->state() == Transaction::State::Open);
    String result;
    for (std::size_t i = transaction->transactionBufferStartIndex(); i < _transactionBuffer.size(); ++i) {
        captureFn(result, _transactionBuffer.at(i));
    }
    return result;
}


void TokenDecoder::popTransaction() noexcept {
    assert(!_transactions.empty());
    _transactions.pop_back();
}


}

