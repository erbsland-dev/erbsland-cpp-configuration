// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Parser.hpp"


#include "impl/Parser.hpp"


namespace erbsland::conf {


void Parser::setSourceResolver(const SourceResolverPtr &sourceResolver) noexcept {
    _settings.sourceResolver = sourceResolver;
}


void Parser::setAccessCheck(const AccessCheckPtr &accessCheck) noexcept {
    _settings.accessCheck = accessCheck;
}


void Parser::setSignatureValidator(const SignatureValidatorPtr &signatureValidator) noexcept {
    _settings.signatureValidator = signatureValidator;
}


auto Parser::parseOrThrow(const SourcePtr &source) -> DocumentPtr  {
    _lastError = std::nullopt;
    impl::Parser parserImplementation(source, _settings);
    return parserImplementation.parse();
}


auto Parser::parse(const SourcePtr &source) -> DocumentPtr {
    try {
        _lastError = std::nullopt;
        impl::Parser parserImplementation(source, _settings);
        return parserImplementation.parse();
    } catch (const Error &error) {
        _lastError = error;
        return {};
    }
}


auto Parser::lastError() const noexcept -> Error {
    return _lastError.value_or(Error{ErrorCategory::Internal, u8"No error occurred."});
}


}

