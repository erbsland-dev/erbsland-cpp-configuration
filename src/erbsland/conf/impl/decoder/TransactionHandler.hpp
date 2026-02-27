// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../char/Char.hpp"

#include "../../String.hpp"


namespace erbsland::conf::impl {


class Transaction;


/// Interface to handle transactions when reading from a character stream.
///
/// @tested This is an integral part of `TokenDecoder` and `FastNameDecoder`.
///
class TransactionHandler {
public:
    /// A capture function to convert captured characters.
    ///
    using CaptureFn = std::function<void (String&, const Char&)>;

public:
    virtual ~TransactionHandler() = default;

public:
    /// Start a new transaction.
    ///
    /// This method is called from the constructor of the transaction.
    ///
    /// @param transaction The transaction to start.
    /// @return The start index in the transaction buffer.
    ///
    [[nodiscard]] virtual auto startTransaction(Transaction *transaction) noexcept -> std::size_t = 0;

    /// Commit the given transaction.
    ///
    /// This method is called when a transaction is committed. If there are no nested transactions, the
    /// characters held by the transaction are deleted, as no rollback is required anymore. When there are
    /// nested transactions, on the other hand, the captured characters are transferred to the next
    /// transaction.
    ///
    /// @param transaction The transaction that is committed.
    ///
    virtual void commitTransaction(const Transaction *transaction) noexcept = 0;

    /// Roll back the given transaction.
    ///
    /// This method is called from the destructor of the transaction. All captured characters must be put
    /// back to the decoder stack to restore the state when the transaction was started.
    ///
    /// @param transaction The transaction that is rolled back.
    ///
    virtual void rollbackTransaction(const Transaction *transaction) noexcept = 0;

    /// Get the length of the captured text for a transaction.
    ///
    [[nodiscard]] virtual auto transactionCapturedSize(const Transaction *transaction) noexcept -> std::size_t = 0;

    /// Access the captured text from a transaction.
    ///
    /// @param transaction The transaction to access - must be open.
    /// @param captureFn The capture function to convert the captured characters.
    /// @return The captured text.
    ///
    [[nodiscard]] virtual auto captureTransactionContent(const Transaction *transaction, const CaptureFn &captureFn) const noexcept -> String = 0;
};


}