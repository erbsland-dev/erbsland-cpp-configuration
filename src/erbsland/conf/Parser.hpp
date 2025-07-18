// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "AccessCheck.hpp"
#include "Document.hpp"
#include "SignatureValidator.hpp"
#include "Source.hpp"
#include "SourceResolver.hpp"

#include "impl/ParserSettings.hpp"

#include <optional>


namespace erbsland::conf {


/// This parser reads the Erbsland Configuration Language.
///
class Parser final {
public:
    Parser() = default;
    ~Parser() = default;

public:
    /// Set a custom source resolver used to resolve include directives while parsing.
    ///
    /// By default, the `FileSourceResolver` is used which supports `file:` includes in the format
    /// recommended in the documentation.
    ///
    /// @param sourceResolver The custom source resolver, or `nullptr` to disable the `\@include` meta-command.
    ///
    void setSourceResolver(const SourceResolverPtr &sourceResolver) noexcept;

    /// Set a custom access check.
    ///
    /// @param accessCheck An instance of a source access check implementation, or `nullptr` to disable
    ///     the `\@include` meta-command.
    ///
    void setAccessCheck(const AccessCheckPtr &accessCheck) noexcept;

    /// Set a signature validator.
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
    auto lastError() const noexcept -> Error;

private:
    impl::ParserSettings _settings; ///< The parser settings.
    std::optional<Error> _lastError; ///< The last error that occurred.
};


}

