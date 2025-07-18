// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "BytesValue.hpp"


#include "../utf8/U8Format.hpp"


namespace erbsland::conf::impl {


auto BytesValue::toTextRepresentation() const noexcept -> String {
    static const std::u8string_view hexDigits = u8"0123456789abcdef";
    String result(_value.size() * 2u, u8' ');
    for (size_t i = 0; i < _value.size(); i++) {
        auto b = _value.at(i);
        result[i*2] = hexDigits.at(static_cast<size_t>(b >> 4));
        result[i*2+1] = hexDigits.at(static_cast<size_t>(b & static_cast<std::byte>(0x0f)));
    }
    return result;
}


auto BytesValue::toTestText() const noexcept -> String {
    return u8format(u8"{}({})", type(), _value.toHex());
}


}

