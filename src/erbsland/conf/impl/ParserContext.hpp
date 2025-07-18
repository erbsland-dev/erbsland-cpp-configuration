// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "AssignmentStream.hpp"
#include "Lexer.hpp"

#include "decoder/TokenDecoder.hpp"

#include "../Source.hpp"

#include <stdexcept>
#include <vector>


namespace erbsland::conf::impl {


class ParserContext;
using ParserContextPtr = std::shared_ptr<ParserContext>;
using ParserContextStack = std::vector<ParserContextPtr>;


/// Parsing context for a single document source.
///
/// @needtest
///
class ParserContext final {
public:
    /// Construct a new parsing context.
    ///
    /// @param includeLevel The include level for this source.
    /// @param source Source from which tokens are read.
    ///
    explicit ParserContext(const std::size_t includeLevel, SourcePtr source, PrivateTag /*pt*/) noexcept
    :
        _includeLevel{includeLevel},
        _source{std::move(source)},
        _lexer{Lexer::create(CharStream::create(_source))},
        _assignmentStream(AssignmentStream::create(_lexer)) {
    }

    /// Create a new context instance.
    ///
    /// @param includeLevel The include level for this source.
    /// @param source Source from which tokens are read.
    /// @return Shared-pointer to the new context.
    ///
    [[nodiscard]] static auto create(const std::size_t includeLevel, SourcePtr source) -> ParserContextPtr {
        return std::make_shared<ParserContext>(includeLevel, std::move(source), PrivateTag{});
    }

    // defaults
    ~ParserContext() = default;

public:
    /// Test if this context was initialized.
    ///
    [[nodiscard]] auto isInitialized() const noexcept -> bool {
        return _initialized;
    }

    /// Initialize this context.
    ///
    void initialize() {
        if (_initialized) {
            throw std::logic_error("ParserContext::initialize() called twice.");
        }
        if (!_source->isOpen()) {
            _source->open();
        }
        _assignmentGenerator = _assignmentStream->assignments();
        _assignmentIterator = _assignmentGenerator.begin();
        _endIterator = _assignmentGenerator.end();
        _initialized = true;
    }

    /// Check if more assignments are available.
    ///
    [[nodiscard]] auto hasNext() const -> bool {
        return _assignmentIterator != _endIterator;
    }

    /// Retrieve the next assignment.
    ///
    /// @throws Error For any problems while parsing a document.
    ///
    [[nodiscard]] auto nextAssignment() -> Assignment {
        auto assignment =  *_assignmentIterator;
        ++_assignmentIterator;
        return assignment;
    }

    /// Set the signature text for this context.
    ///
    void setSignatureText(String signatureText) {
        _signatureText = std::move(signatureText);
    }

    /// Get the signature text assigned to this context.
    ///
    [[nodiscard]] auto signatureText() const -> String {
        return _signatureText;
    }

    /// Get the document digest produced by the lexer.
    ///
    [[nodiscard]] auto digest() const -> Bytes {
        return _lexer->digest();
    }

    /// Get the include level of this source.
    ///
    [[nodiscard]] auto includeLevel() const noexcept -> uint8_t {
        return _includeLevel;
    }

    /// Identifier of the source currently processed.
    ///
    [[nodiscard]] auto sourceIdentifier() const -> SourceIdentifierPtr {
        return _source->identifier();
    }

    /// Set the include location for this context.
    ///
    void setIncludeLocation(Location includeLocation) {
        _includeLocation = std::move(includeLocation);
    }

    /// Access the include location of this context.
    ///
    [[nodiscard]] auto includeLocation() const -> const Location& {
        return _includeLocation;
    }

    /// Set the parent source identifier.
    ///
    void setParentSourceIdentifier(SourceIdentifierPtr parentSourceIdentifier) {
        _parentSourceIdentifier = std::move(parentSourceIdentifier);
    }

    /// Access the parent source identifier.
    ///
    [[nodiscard]] auto parentSourceIdentifier() const -> const SourceIdentifierPtr& {
        return _parentSourceIdentifier;
    }

    /// Close this context.
    ///
    /// Explicit call to avoid exceptions in destruction.
    ///
    void close() {
        _endIterator = {};
        _assignmentIterator = {};
        _assignmentGenerator = {};
        _lexer = {};
        if (_source->isOpen()) {
            _source->close();
        }
        _source = {};
    }

private:
    bool _initialized = false; ///< Flag indicating if the context has been initialized.
    std::size_t _includeLevel; ///< The include level for this context.
    SourcePtr _source; ///< The source for this context, as reference to detect inclusion loops.
    SourceIdentifierPtr _parentSourceIdentifier; ///< The identifier of the parent source.
    Location _includeLocation; ///< The location of the include directive.
    LexerPtr _lexer; ///< The lexer instance.
    AssignmentStreamPtr _assignmentStream; ///< The assignment stream.
    AssignmentGenerator _assignmentGenerator; ///< The assignment generator.
    AssignmentGenerator::iterator _assignmentIterator; ///< The assignment iterator.
    AssignmentGenerator::iterator _endIterator; ///< The end iterator.
    String _signatureText; ///< The signature text, if any.
};


}
