// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Location.hpp"


#include "impl/utf8/U8Format.hpp"


namespace erbsland::conf {


auto Location::toText() const -> String {
    String result = _sourceIdentifier == nullptr ? u8"<unknown>" : _sourceIdentifier->toText();
    if (!_position.isUndefined()) {
        result.append(impl::u8format(":{}", _position.line()));
        if (_position.column() >= 0) {
            result.append(impl::u8format(":{}", _position.column()));
        }
    }
    return result;
}


#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
auto internalView(const Location &object) -> impl::InternalViewPtr {
    auto result = impl::InternalView::create();
    if (object._sourceIdentifier) {
        result->setValue(u8"sourceIdentifier", *object._sourceIdentifier);
    } else {
        result->setValue(u8"sourceIdentifier", u8"<none>");
    }
    result->setValue(u8"position", object._position);
    return result;
}
#endif


}

