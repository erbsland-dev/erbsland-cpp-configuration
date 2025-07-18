// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::conf::impl {


/// The type of assignment.
///
/// @notest This is tested via test for the `AssignmentStream` class.
///
enum class AssignmentType : uint8_t {
    EndOfDocument,  ///< No assignment. This is the end of the document.
    SectionMap,     ///< A section map `[...]`
    SectionList,    ///< An entry of a section list. `*[...]`
    Value,          ///< A value. `name: value`
    MetaValue,      ///< A meta-value or meta-command. `@name: "text"`
};


}

