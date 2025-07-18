// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "String.hpp"


#include "Bytes.hpp"

#include "impl/utf8/U8StringView.hpp"


namespace erbsland::conf {


auto String::escapedSize(EscapeMode mode) const noexcept -> std::size_t {
    return impl::U8StringView{*this}.escapedSize(mode);
}


auto String::toEscaped(const EscapeMode mode) const noexcept -> String {
    return impl::U8StringView{*this}.toEscaped(mode);
}


}

