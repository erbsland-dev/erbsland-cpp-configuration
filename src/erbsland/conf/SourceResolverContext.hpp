// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "SourceIdentifier.hpp"


namespace erbsland::conf {


/// The context for resolving sources.
///
/// @tested Tested via `Parser` class.
///
struct SourceResolverContext {
    /// The raw and unprocessed text from the `include`-command.
    ///
    String includeText;

    /// The source identifier of the document with the `include`-command.
    ///
    SourceIdentifierPtr sourceIdentifier;
};


}


