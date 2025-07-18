// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Position.hpp"


#include "impl/utf8/U8Format.hpp"


namespace erbsland::conf {


auto Position::toText() const noexcept -> String {
    if (isUndefined()) {
        return u8"undefined";
    }
    return impl::u8format("{}:{}", _line, _column);
}


#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
auto internalView(const Position &object) -> impl::InternalViewPtr {
    auto result = impl::InternalView::create();
    result->setValue("line", std::to_string(object._line));
    result->setValue("column", std::to_string(object._column));
    return result;
}
#endif


}

