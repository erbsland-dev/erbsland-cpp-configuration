// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ParserContext.hpp"
#include "ParserSettings.hpp"

#include "../value/DocumentBuilder.hpp"

#include "../../Position.hpp"
#include "../../Source.hpp"


namespace erbsland::conf::impl {


/// The parser implementation to hide details from the API.
///
/// This class is the main abstraction of the parser process with two main functions. First, it hides the
/// implementation from the `Parser` interface. Second, the context creates the token decoder stack used
/// to parse nested documents.
///
/// About the const reference to `ParserSettings`: an instance of this structure is created as a local variable
/// in `conf::Parser::parse()`. Therefore, the reference stored here is always valid for the lifetime of this object.
///
/// @needtest
///
class Parser {
public:
    Parser(
        SourcePtr documentSource,
        const ParserSettings &settings)
    :
        _settings{settings} {

        // Prepare the stack with the root context.
        _contextStack.reserve(limits::maxDocumentNesting + 1);
        _contextStack.emplace_back(ParserContext::create(0, std::move(documentSource)));
    }

    ~Parser() = default;

    // prevent copy and assign.
    Parser(const Parser&) = delete;
    auto operator=(const Parser&) -> Parser& = delete;

public:
    /// Parse the document and return the resulting value tree.
    ///
    /// @return The root value of the parsed document.
    ///
    auto parse() -> DocumentPtr {
        try {
            Location rootLocation;
            if (!_contextStack.empty()) {
                // Create a location for the document root for better error messages.
                rootLocation = Location{_contextStack.front()->sourceIdentifier()};
            }
            while (hasMoreContent()) {
                initializeCurrentContext();
                if (hasNext()) {
                    auto assignment = nextAssignment();
                    processAssignment(assignment);
                } else {
                    preLeaveProcessing();
                    leaveContext();
                }
            }
            auto document = _builder.getDocumentAndReset();
            document->setLocation(rootLocation);
            return document;
        } catch (const Error&) {
            // close all contexts in case of an error.
            for (const auto &context : std::views::reverse(_contextStack)) {
                try {
                    context->close();
                } catch (const Error&) {
                    // ignore any `Error` exceptions while closing the contexts because of an error.
                }
            }
            throw;
        }
    }

private:
    /// Test if there is more context for processing.
    ///
    [[nodiscard]] auto hasMoreContent() const -> bool {
        return !_contextStack.empty();
    }

    /// Access the current context.
    ///
    [[nodiscard]] auto currentContext() -> ParserContext& {
        if (_contextStack.empty()) {
            throw std::logic_error{"Called 'currentContext()` with no context available."};
        }
        return *_contextStack.back();
    }

    /// Access the current context.
    ///
    [[nodiscard]] auto currentContext() const -> const ParserContext& {
        if (_contextStack.empty()) {
            throw std::logic_error{"Called 'currentContext()` with no context available."};
        }
        return *_contextStack.back();
    }

    /// Initialize the current context if required.
    ///
    void initializeCurrentContext() {
        if (!currentContext().isInitialized()) {
            // before initializing, verify if we are allowed to access the source.
            if (_settings.accessCheck != nullptr) {
                const AccessSources sources{
                    .source = currentContext().sourceIdentifier(),
                    .parent = currentContext().parentSourceIdentifier(),
                    .root = !_contextStack.empty() ? _contextStack.front()->sourceIdentifier() : nullptr
                };
                auto location = currentContext().includeLocation();
                if (location.isUndefined()) {
                    location = Location{currentContext().sourceIdentifier()};
                }
                try {
                    if (_settings.accessCheck->check(sources) != AccessCheckResult::Granted) {
                        throw Error{
                            ErrorCategory::Access,
                            u8"Access denied to source.",
                            location
                        };
                    }
                } catch (const Error& error) {
                    throw error.withLocation(location);
                }
            }
            // Now as we got access, initialize this context.
            currentContext().initialize();
        }
    }

    /// Test if there is a next token.
    ///
    [[nodiscard]] auto hasNext() const -> bool {
        return currentContext().hasNext();
    }

    /// Get the next token.
    ///
    [[nodiscard]] auto nextAssignment() -> Assignment {
        return currentContext().nextAssignment();
    }

    /// Process an assignment
    ///
    void processAssignment(const Assignment &assignment) {
        switch (assignment.type()) {
        case AssignmentType::EndOfDocument:
            // ignore
            break;
        case AssignmentType::SectionMap:
            _builder.addSectionMap(assignment.namePath(), assignment.location());
            break;
        case AssignmentType::SectionList:
            _builder.addSectionList(assignment.namePath(), assignment.location());
            break;
        case AssignmentType::Value:
            _builder.addValue(assignment.namePath(), assignment.value(), assignment.location());
            break;
        case AssignmentType::MetaValue:
            processMetaValue(assignment);
            break;
        }
    }

    void processMetaValue(const Assignment &assignment) {
        if (assignment.namePath().back() == Name::metaSignature()) {
            currentContext().setSignatureText(assignment.value()->asText());
        } else if (assignment.namePath().back() == Name::metaInclude()) {
            const auto includeLevel = currentContext().includeLevel() + 1U;
            if (includeLevel >= limits::maxDocumentNesting) {
                throw Error{
                    ErrorCategory::LimitExceeded,
                    u8format(u8"The maximum document nesting level of {} is exceeded.", limits::maxDocumentNesting),
                    assignment.location()};
            }
            SourceResolverContext const resolveContext{
                .includeText = assignment.value()->asText(),
                .sourceIdentifier = sourceIdentifier()
            };
            if (_settings.sourceResolver == nullptr) {
                throw Error{
                    ErrorCategory::Unsupported,
                    u8"The @include meta-command is disabled.",
                    assignment.location()};
            }
            SourceListPtr sourceList;
            try {
                sourceList = _settings.sourceResolver->resolve(resolveContext);
            } catch (const Error& error) {
                throw error.withLocation(assignment.location());
            }
            if (sourceList == nullptr) {
                throw Error{
                    ErrorCategory::Syntax,
                    u8"The @include meta-command could not be resolved.",
                    assignment.location()};
            }
            const auto parentSourceIdentifier = sourceIdentifier();
            for (const auto &source : std::views::reverse(*sourceList)) {
                addSourceContext(
                    includeLevel,
                    source,
                    parentSourceIdentifier,
                    assignment.location());
            }
        }
    }

    void addSourceContext(
        const std::size_t includeLevel,
        const SourcePtr &source,
        const SourceIdentifierPtr &parentSourceIdentifier,
        const Location &location) {

        for (const auto &context : _contextStack) {
            if (*context->sourceIdentifier() == *source->identifier()) {
                throw Error{
                    ErrorCategory::Syntax,
                    u8"An included document is in the list of parent documents (loop detected).",
                    location};
            }
        }
        auto newContext = ParserContext::create(includeLevel, source);
        newContext->setIncludeLocation(location);
        newContext->setParentSourceIdentifier(parentSourceIdentifier);
        _contextStack.emplace_back(std::move(newContext));
    }

    /// The source identifier for the current context.
    ///
    [[nodiscard]] auto sourceIdentifier() const -> SourceIdentifierPtr {
        return currentContext().sourceIdentifier();
    }

    /// The signature text for the current context.
    ///
    [[nodiscard]] auto signatureText() const -> String {
        return currentContext().signatureText();
    }

    /// The digest text for the current context.
    ///
    [[nodiscard]] auto digestText() const -> String {
        String result;
        result += crypto::ShaHash::algorithmToText(defaults::documentHashAlgorithm);
        result += u8" ";
        result += currentContext().digest().toHex();
        return result;
    }

    /// Process the signature before leaving the context.
    ///
    void preLeaveProcessing() {
        // Before leaving the context, verify the signature if one is required.
        if (_settings.signatureValidator != nullptr) {
            const SignatureValidatorData data{
                .sourceIdentifier = sourceIdentifier(),
                .signatureText = signatureText(),
                .documentDigest = digestText()
            };
            const auto signatureVerificationResult = _settings.signatureValidator->validate(data);
            if (signatureVerificationResult != SignatureValidatorResult::Accept) {
                throw Error{
                    ErrorCategory::Signature,
                    u8"Signature verification failed.",
                    Location{sourceIdentifier()}
                };
            }
        } else {
            // The default behavior is to reject a document with a signature if it can't be verified.
            if (!signatureText().empty()) {
                throw Error{
                    ErrorCategory::Signature,
                    u8"Signature cannot be verified.",
                    Location{sourceIdentifier()}
                };
            }
        }
    }

    /// Leave the current context.
    ///
    void leaveContext() {
        if (_contextStack.empty()) {
            throw std::logic_error{"Called 'leaveContext()` with no context available."};
        }
        currentContext().close();
        _contextStack.pop_back();
    }

private:
    DocumentBuilder _builder; ///< The document builder.
    ParserContextStack _contextStack; ///< The context stack.
    const ParserSettings &_settings; ///< The parser settings.
};


}

