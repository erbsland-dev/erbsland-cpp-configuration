// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


template <>
struct std::formatter<erbsland::conf::Value> : std::formatter<std::string> {
    auto format(const erbsland::conf::Value &value, format_context &ctx) const {
        return std::formatter<std::string>::format(value.toTextRepresentation().toCharString(), ctx);
    }
};