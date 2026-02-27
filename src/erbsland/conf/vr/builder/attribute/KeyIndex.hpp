// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Attribute.hpp"

#include "../NamePathHelper.hpp"

#include "../../../CaseSensitivity.hpp"
#include "../../../impl/vr/KeyDefinition.hpp"
#include "../../../NamePath.hpp"

#include <utility>
#include <vector>


namespace erbsland::conf::vr::builder {


/// Defines an index of key paths that can be referenced by key constraints.
struct KeyIndex : Attribute {
    explicit KeyIndex(
        std::vector<NamePathLike> keyPaths,
        const CaseSensitivity caseSensitivity = CaseSensitivity::CaseInsensitive)
        : _keyPaths{std::move(keyPaths)},
          _caseSensitivity{caseSensitivity} {
    }

    KeyIndex(
        Name name,
        std::vector<NamePathLike> keyPaths,
        const CaseSensitivity caseSensitivity = CaseSensitivity::CaseInsensitive)
        : _name{std::move(name)},
          _keyPaths{std::move(keyPaths)},
          _caseSensitivity{caseSensitivity} {
    }

    KeyIndex(
        const String &name,
        std::vector<NamePathLike> keyPaths,
        const CaseSensitivity caseSensitivity = CaseSensitivity::CaseInsensitive)
        : KeyIndex(Name::createRegular(name), std::move(keyPaths), caseSensitivity) {
    }

    explicit KeyIndex(const NamePathLike &keyPath)
        : KeyIndex(std::vector<NamePathLike>{keyPath}, CaseSensitivity::CaseInsensitive) {
    }

    KeyIndex(
        Name name,
        const NamePathLike &keyPath,
        const CaseSensitivity caseSensitivity = CaseSensitivity::CaseInsensitive)
        : KeyIndex(std::move(name), std::vector<NamePathLike>{keyPath}, caseSensitivity) {
    }

    KeyIndex(
        const String &name,
        const NamePathLike &keyPath,
        const CaseSensitivity caseSensitivity = CaseSensitivity::CaseInsensitive)
        : KeyIndex(name, std::vector<NamePathLike>{keyPath}, caseSensitivity) {
    }

    explicit KeyIndex(
        const std::initializer_list<NamePathLike> keyPaths,
        const CaseSensitivity caseSensitivity = CaseSensitivity::CaseInsensitive)
        : KeyIndex(std::vector<NamePathLike>{keyPaths}, caseSensitivity) {
    }

    KeyIndex(
        Name name,
        const std::initializer_list<NamePathLike> keyPaths,
        const CaseSensitivity caseSensitivity = CaseSensitivity::CaseInsensitive)
        : KeyIndex(std::move(name), std::vector<NamePathLike>{keyPaths}, caseSensitivity) {
    }

    KeyIndex(
        const String &name,
        const std::initializer_list<NamePathLike> keyPaths,
        const CaseSensitivity caseSensitivity = CaseSensitivity::CaseInsensitive)
        : KeyIndex(name, std::vector<NamePathLike>{keyPaths}, caseSensitivity) {
    }

    void operator()(impl::Rule &rule) override {
        auto keys = detail::parseNamePathList(_keyPaths);
        auto definition = impl::KeyDefinition::create(_name, keys, _caseSensitivity, {});
        rule.addKeyDefinition(definition);
    }

    Name _name;
    std::vector<NamePathLike> _keyPaths;
    CaseSensitivity _caseSensitivity{CaseSensitivity::CaseInsensitive};
};


}
