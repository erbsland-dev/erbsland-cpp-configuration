// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Generator.hpp"
#include "InternalView.hpp"
#include "LexerToken.hpp"
#include "TokenGenerator.hpp"

#include "decoder/TokenDecoder.hpp"


namespace erbsland::conf::impl {


class Lexer;
using LexerPtr = std::shared_ptr<Lexer>;


/// This lexer returns a low-level stream with tokens of the document syntax.
///
/// Each token contains the raw text of the document with the start and end positions, including tokens for
/// spacing, comments, and line-breaks. This is intentionally done to allow using this lexer for syntax highlighting.
///
/// On a successful run, there is *always* a last `EndOfData` token, with no raw text and no positions. This last
/// token makes sure that the exception that occurs after the last actual text is correctly propagated to the caller.
///
/// The method `tokens()` can only be called once.
///
/// @tested Individual parts tested in `Lexer...Test` unit tests.
///
class Lexer final {
public:
    /// Create a new lexer, using the given decoder.
    ///
    /// @param decoder The decoder to use.
    /// @return An instance of the lexer.
    ///
    [[nodiscard]] static auto create(CharStreamPtr decoder) noexcept -> LexerPtr {
        return std::make_shared<Lexer>(std::make_shared<TokenDecoder>(
            std::move(decoder)), PrivateTag{});
    }

    /// Create a new lexer, using the given decoder.
    ///
    /// @param decoder The buffered decoder to use.
    ///
    explicit Lexer(TokenDecoderPtr decoder, PrivateTag /*pt*/) noexcept : _decoder(std::move(decoder)) {
        assert(_decoder != nullptr);
    }

    // defaults
    ~Lexer() = default;

public:
    /// Access the source identifier for error messages.
    ///
    auto sourceIdentifier() const noexcept -> SourceIdentifierPtr;

    /// Get the tokens for the decoded document.
    ///
    /// You can call this method only once for a given decoder.
    ///
    /// @returns A stream of `LexerToken` objects, on success, always ending with a last `EndOfData` token.
    /// @throws Error in case of any error while processing the input.
    ///
    auto tokens() -> TokenGenerator;

    /// Get the digest from the tokenized document.
    ///
    /// Must be called *after* calling `tokens()` and read all tokens including the end-of-data token.
    /// The method must also be called before calling `close()`.
    ///
    /// @return The digest for the document, or empty if none was created.
    ///
    [[nodiscard]] auto digest() const -> Bytes;

    /// Get the algorithm that was used to create the hash digest for the document.
    ///
    [[nodiscard]] static auto hashAlgorithm() -> crypto::ShaHash::Algorithm;

    /// Closes this lexer, releasing the decoder and all resources.
    ///
    void close() noexcept;

private:
    [[nodiscard]] auto decoder() const noexcept -> TokenDecoder& { return *_decoder; }

public: // testing
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const Lexer &object) -> InternalViewPtr;
#endif

private:
    TokenDecoderPtr _decoder; ///< The token decoder.
    Bytes _digest; ///< The digest of the document.
};


}


