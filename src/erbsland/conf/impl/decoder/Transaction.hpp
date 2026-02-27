// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TransactionHandler.hpp"

#include "../utilities/InternalView.hpp"

#include "../../String.hpp"

#include <cassert>
#include <memory>


namespace erbsland::conf::impl {


class Transaction;
using TransactionPtr = std::unique_ptr<Transaction>;


/// A transaction scope that allows backtracking.
///
/// @tested This is an integral part of `TokenDecoder` and `FastNameDecoder`.
///
class Transaction final {
public:
    /// The state of this transaction.
    ///
    enum class State : uint8_t {
        Open, ///< The transaction is open and receiving characters.
        Committed, ///< The transaction is closed and marked as committed.
        RolledBack, ///< The transaction is closed and marked as rolled back.
    };

public:
    /// Create a new transaction scope for this lexer.
    ///
    /// @param handler The transaction handler.
    ///
    explicit Transaction(TransactionHandler &handler) noexcept : _transactionHandler{handler} {
        _transactionBufferStartIndex = handler.startTransaction(this);
    }

    /// When not committed, roll the transaction back on destruction.
    ///
    ~Transaction() {
        // If at this point, the transaction is still open, roll it back.
        if (_state == State::Open) {
            _state = State::RolledBack;
            _transactionHandler.rollbackTransaction(this);
        }
    }

    // Disallow copy and assign.
    Transaction(const Transaction&) = delete;
    Transaction(Transaction &&) = delete;
    auto operator=(const Transaction&) -> Transaction& = delete;
    auto operator=(Transaction &&) -> Transaction& = delete;

public: // user functions
    /// Get the number of captured characters or bytes.
    ///
    /// @return The number of captured characters or bytes (depending on the backend).
    ///
    [[nodiscard]] auto capturedSize() const noexcept -> std::size_t {
        return _transactionHandler.transactionCapturedSize(this);
    }

    /// Access the captured text.
    ///
    [[nodiscard]] auto captured(const TransactionHandler::CaptureFn &captureFn) const noexcept -> String {
        return _transactionHandler.captureTransactionContent(this, captureFn);
    }

    /// Access the captured text as a string.
    ///
    [[nodiscard]] auto capturedString() const noexcept -> String {
        return captured([](String &result, const Char &character) {
            character.appendTo(result);
        });
    }

    /// Access the captured text as a lower-case string.
    ///
    [[nodiscard]] auto capturedLowerCaseString() const noexcept -> String {
        return captured([](String &result, const Char &character) {
            character.appendLowerCaseTo(result);
        });
    }

    /// Commit this transaction.
    ///
    void commit() noexcept {
        assert(_state == State::Open); // A transaction must be open to be committed
        _state = State::Committed;
        _transactionHandler.commitTransaction(this);
    }

    /// Roll the transaction back.
    ///
    void rollback() noexcept {
        assert(_state == State::Open); // A transaction must be open to be rolled back
        _state = State::RolledBack;
        _transactionHandler.rollbackTransaction(this);
    }

public: // backend functions
    /// Get the state of this transaction.
    ///
    [[nodiscard]] auto state() const noexcept -> State {
        return _state;
    }

    /// Get the transaction buffer start index.
    ///
    [[nodiscard]] auto transactionBufferStartIndex() const noexcept -> std::size_t {
        return _transactionBufferStartIndex;
    }

public:
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(Transaction *object) -> InternalViewPtr {
        auto result = InternalView::create();
        result->setValue("transactionBufferStartIndex", object->_transactionBufferStartIndex);
        switch (object->_state) {
        case State::Open:
            result->setValue(u8"state", u8"open");
            break;
        case State::Committed:
            result->setValue(u8"state", u8"committed");
            break;
        case State::RolledBack:
            result->setValue(u8"state", u8"rolled-back");
            break;
        }
        return result;
    }
#endif

private:
    TransactionHandler &_transactionHandler; ///< The token decoder.
    std::size_t _transactionBufferStartIndex{0}; ///< The start index in the transaction buffer, where this transaction started.
    State _state{State::Open}; ///< The state of this transaction.
};


}

