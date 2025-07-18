// Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DateTime.hpp"


namespace erbsland::conf {


auto DateTime::toText() const noexcept -> String {
    if (isUndefined()) {
        return {};
    }
    String result;
    result.reserve(36);
    result.append(_date.toText());
    result.append(u8" ");
    result.append(_time.toText());
    return result;
}


}

