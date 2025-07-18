// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "impl/value/Value.hpp"


namespace erbsland::conf {


class Document;
using DocumentPtr = std::shared_ptr<Document>;


/// A configuration document.
///
class Document : public Value {
public:
    using FlatValueMap = std::map<NamePath, ConstValuePtr>;

public:
    // defaults
    ~Document() override = default;

public:
    /// Convert the value structure of this document into a flat map of values.
    ///
    /// @return A flat map with all sections and values of this document.
    ///
    [[nodiscard]] virtual auto toFlatValueMap() const noexcept -> FlatValueMap = 0;
};


}

