// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "AccessCheck.hpp"
#include "Document.hpp"
#include "SignatureValidator.hpp"
#include "Source.hpp"
#include "SourceResolver.hpp"
#include "StringConvertible.hpp"

#include "impl/parser/ParserSettings.hpp"

#include <optional>


namespace erbsland::conf {


/// This parser reads the Erbsland Configuration Language.
///
/// *Multithreading*: This parser is **reentrant**, and therefore it can be used in multiple threads, as long each
/// thread uses an individual instance of the parser.
///
/// @tested `ParserAccessTest`, `ParserBasicTest`, `ParserComplianceTest`, `ParserErrorClassTest`,
///     `ParserIncludeTest`, `ParserSignatureTest`
///
class Parser final {
public:
    /// Default constructor.
    Parser() = default;
    /// Default destructor.
    ~Parser() = default;

public:
    /// Set a custom source resolver used to resolve include directives while parsing.
    ///
    /// By default, an instance of `FileSourceResolver` is used, which supports file-based includes, as specified
    /// in the format recommended in the documentation.
    ///
    /// @param sourceResolver The custom source resolver, or `nullptr` to disable the `include` meta-command.
    ///
    void setSourceResolver(const SourceResolverPtr &sourceResolver) noexcept;

    /// Set a custom access check.
    ///
    /// By default, an instance of `FileAccessCheck` with default options is used. This instance limits included files
    /// to the same directory and subdirectories of the including configuration.
    ///
    /// @param accessCheck An instance of a source access check implementation, or `nullptr` to disable
    ///     the `include` meta-command.
    ///
    void setAccessCheck(const AccessCheckPtr &accessCheck) noexcept;

    /// Set a signature validator.
    ///
    /// By default, no signature validator is set. This allows parsing all unsigned configuration documents.
    /// Documents with a `signature` meta-value get rejected by the parser.
    ///
    /// @param signatureValidator An instance of a signature validator implementation, or `nullptr` to
    ///     disable signature validation.
    ///
    void setSignatureValidator(const SignatureValidatorPtr &signatureValidator) noexcept;

    /// Parse the given source into a configuration document and throw an exception on any error.
    ///
    /// @param source The source to parse. Should be closed.
    /// @return The root node of the parsed configuration tree.
    /// @throws Error if there was any problem with the parsed source or document.
    ///
    auto parseOrThrow(const SourcePtr &source) -> DocumentPtr;

    /// Parse the given source into a configuration document.
    ///
    /// @param source The source to parse. Should be closed.
    /// @return The root node of the parsed configuration tree or nullptr on any error.
    ///     Use `lastError()` to access the last error.
    ///
    auto parse(const SourcePtr &source) -> DocumentPtr;

    /// Access the last error.
    ///
    /// @return The error object of the last error.
    ///
    auto lastError() const noexcept -> Error;

public: // convenience methods
    /// @name Convenience Methods
    /// These methods are convenience methods to call `parse` or `parseOrThrow`.
    /// They construct a `Source` from the given parameters and call `parse` or `parseOrThrow` using this source.
    /// @{

    /// Parse the given file into a configuration document and throw an exception on error.
    [[nodiscard]] auto parseFileOrThrow(const String &path) -> DocumentPtr {
        return parseOrThrow(Source::fromFile(path));
    }
    /// Parse the given file into a configuration document and throw an exception on error.
    [[nodiscard]] auto parseFileOrThrow(const std::filesystem::path &path) -> DocumentPtr {
        return parseOrThrow(Source::fromFile(path));
    }
    /// Parse the given file into a configuration document and return a null pointer on error.
    [[nodiscard]] auto parseFile(const String &path) -> DocumentPtr {
        return parse(Source::fromFile(path));
    }
    /// Parse the given file into a configuration document and return a null pointer on error.
    [[nodiscard]] auto parseFile(const std::filesystem::path &path) -> DocumentPtr {
        return parse(Source::fromFile(path));
    }
    /// Parse the given text into a configuration document and throw an exception on error.
    [[nodiscard]] auto parseTextOrThrow(const String &text) -> DocumentPtr {
        return parseOrThrow(Source::fromString(text));
    }
    /// Parse the given text into a configuration document and throw an exception on error.
    template<typename Fwd>
    requires StringConvertible<Fwd>
    [[nodiscard]] auto parseTextOrThrow(Fwd &&text) -> DocumentPtr {
        return parseOrThrow(Source::fromString(String(std::forward<Fwd>(text))));
    }
    /// Parse the given text into a configuration document and return a null pointer on error.
    [[nodiscard]] auto parseText(const String &text) -> DocumentPtr {
        return parse(Source::fromString(text));
    }
    /// Parse the given text into a configuration document and return a null pointer on error.
    template <typename Fwd>
    requires StringConvertible<Fwd>
    [[nodiscard]] auto parseText(Fwd &&text) -> DocumentPtr {
        return parse(Source::fromString(String(std::forward<Fwd>(text))));
    }
    /// @}

private:
    impl::ParserSettings _settings; ///< The parser settings.
    std::optional<Error> _lastError; ///< The last error that occurred.
};


}
