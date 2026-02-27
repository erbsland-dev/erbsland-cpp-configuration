// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "NamePathHelper.hpp"


#include "../../Error.hpp"


namespace erbsland::conf::vr::builder::detail {


auto parseNamePathLike(const NamePathLike &namePathLike) -> NamePath {
    if (std::holds_alternative<std::size_t>(namePathLike)) {
        throw conf::Error{ErrorCategory::Validation, u8"Index values are not allowed in this name-path"};
    }
    if (std::holds_alternative<Name>(namePathLike)) {
        return NamePath{std::get<Name>(namePathLike)};
    }
    if (std::holds_alternative<NamePath>(namePathLike)) {
        return std::get<NamePath>(namePathLike);
    }
    if (std::holds_alternative<String>(namePathLike)) {
        return NamePath::fromText(std::get<String>(namePathLike));
    }
    throw conf::Error{ErrorCategory::Validation, u8"Invalid name-path type"};
}


auto parseNamePathList(const std::vector<NamePathLike> &paths) -> NamePathList {
    NamePathList result;
    result.reserve(paths.size());
    for (const auto &path : paths) {
        result.emplace_back(parseNamePathLike(path));
    }
    return result;
}


}
