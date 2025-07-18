// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Document.hpp"


#include "Section.hpp"

#include <stack>


namespace erbsland::conf::impl {


auto Document::toFlatValueMap() const noexcept -> FlatValueMap {
    std::stack<conf::ConstValuePtr> stack;
    auto thisDocument = shared_from_this();
    stack.push(thisDocument);
    FlatValueMap result;
    while (!stack.empty()) {
        auto value = stack.top();
        stack.pop();
        if (!value->empty()) {
            for (const auto &childValue : *value) {
                stack.push(childValue);
            }
        }
        if (value != thisDocument) { // do not add the document root.
            result.emplace(value->namePath(), value);
        }
    }
    return result;
}


}


