// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TransactionHandler.hpp"

#include "../Char.hpp"
#include "../../Error.hpp"
#include "../../Location.hpp"
#include "../../String.hpp"


namespace erbsland::conf::impl {


/// The base class for all character-based decoders.
///
class Decoder : public TransactionHandler {
public:
    Decoder() = default;
    ~Decoder() override = default;

    // disable copy and assign.
    Decoder(const Decoder &) = delete;
    auto operator=(const Decoder &) -> Decoder& = delete;
    Decoder(Decoder &&) = delete;
    auto operator=(Decoder &&) -> Decoder& = delete;

public:
    /// Initialize this decoder.
    ///
    virtual void initialize() = 0;

    /// Access the current character.
    ///
    [[nodiscard]] virtual auto character() const noexcept -> const Char& = 0;

    /// Get the current location.
    ///
    [[nodiscard]] virtual auto location() const -> Location = 0;

    /// Access the source identifier.
    ///
    [[nodiscard]] virtual auto sourceIdentifier() const noexcept -> SourceIdentifierPtr = 0;

    /// Capture the current character and decode the next.
    ///
    /// @throws Error (Encoding) In case of any encoding error.
    ///
    virtual void next() = 0;

public: // Throwing common exceptions.
    /// Throw the given error.
    ///
    [[noreturn]] void throwError(const ErrorCategory category, const std::u8string_view message) const {
        checkForErrorAndThrowIt();
        throw Error(category, String{message}, location());
    }

    /// In higher layers, control-character and encoding errors need to be delayed for correct error handling.
    ///
    /// This placeholder method allows checking if an error was encountered and needs to be propagated to
    /// the calling code.
    ///
    virtual void checkForErrorAndThrowIt() const {
        // not implemented in the base decoder.
    }

    /// Throw a syntax error.
    ///
    [[noreturn]] void throwSyntaxError(const std::u8string_view message) const {
        throwError(ErrorCategory::Syntax, message);
    }

    /// Throw a limit exceeded error.
    ///
    [[noreturn]] void throwLimitExceededError(const std::u8string_view message) const {
        throwError(ErrorCategory::LimitExceeded, message);
    }

    /// Throw an error if a number exceeds the 64-bit limit.
    ///
    [[noreturn]] void throwNumberLimitExceededError() const {
        throwLimitExceededError(u8"The number exceeds the 64-bit limit.");
    }

    /// Throw an error if the document ends at an unexpected location.
    ///
    [[noreturn]] void throwUnexpectedEndOfDataError() const {
        throwError(ErrorCategory::UnexpectedEnd, u8"Unexpected end of data.");
    }

    /// Throw an error if the document ends at an unexpected location.
    ///
    [[noreturn]] void throwUnexpectedEndOfDataError(const std::u8string_view message) const {
        throwError(ErrorCategory::UnexpectedEnd, message);
    }

    /// Throws an unexpected end or syntax error, depending on the current character.
    ///
    [[noreturn]] void throwSyntaxOrUnexpectedEndError(const std::u8string_view message) const {
        if (character() == Char::EndOfData) {
            throwUnexpectedEndOfDataError(message);
        }
        throwSyntaxError(message);
    }

    /// Throw an internal error.
    ///
    [[noreturn]] void throwInternalError(const std::u8string_view message) const {
        throwError(ErrorCategory::Internal, message);
    }

public: // Constraining functions.
    /// Expect the given Unicode character or character class.
    ///
    template<typename T, std::size_t N> requires (std::is_convertible_v<T, char32_t> || std::is_same_v<T, CharClass>)
    void expect(T expected, const char8_t (&message)[N]) {
        if (!character().isChar(expected)) {
            if (character() == Char::EndOfData) {
                throwUnexpectedEndOfDataError(std::u8string_view{message, N - 1});
            }
            throwSyntaxError(std::u8string_view{message, N - 1});
        }
    }

    /// Expect and skip the given character or character class.
    ///
    template<typename T, std::size_t N> requires (std::is_convertible_v<T, char32_t> || std::is_same_v<T, CharClass>)
    void expectAndNext(T expected, const char8_t (&message)[N]) {
        expect(expected, message);
        next();
    }

    /// Expect that the document continues.
    ///
    /// @param message The error message in case the character stream ends here.
    ///
    void expectMore(const std::u8string_view message) const {
        if (character() == Char::EndOfData) {
            throwUnexpectedEndOfDataError(message);
        }
    }
};


}

