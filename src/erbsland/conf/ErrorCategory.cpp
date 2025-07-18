// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ErrorCategory.hpp"


#include <cassert>


namespace erbsland::conf {


ErrorCategory::ValueMap ErrorCategory::_valueMap = {
    std::make_tuple(IO,                 1, String{u8"IO"}),
    std::make_tuple(Encoding,           2, String{u8"Encoding"}),
    std::make_tuple(UnexpectedEnd,      3, String{u8"UnexpectedEnd"}),
    std::make_tuple(Character,          4, String{u8"Character"}),
    std::make_tuple(Syntax,             5, String{u8"Syntax"}),
    std::make_tuple(LimitExceeded,      6, String{u8"LimitExceeded"}),
    std::make_tuple(NameConflict,       7, String{u8"NameConflict"}),
    std::make_tuple(Indentation,        8, String{u8"Indentation"}),
    std::make_tuple(Unsupported,        9, String{u8"Unsupported"}),
    std::make_tuple(Signature,         10, String{u8"Signature"}),
    std::make_tuple(Access,            11, String{u8"Access"}),
    std::make_tuple(Validation,        12, String{u8"Validation"}),
    std::make_tuple(Internal,          99, String{u8"Internal"}),
    // Additional categories for the API of this parser.
    std::make_tuple(ValueNotFound,    101, String{u8"ValueNotFound"}),
    std::make_tuple(WrongType,        102, String{u8"WrongType"}),
};


auto ErrorCategory::toText() const noexcept -> const String& {
    auto it = std::ranges::find_if(_valueMap, [this](const ValueEntry &entry) -> bool {
        return std::get<0>(entry) == _value;
    });
    assert(it != _valueMap.end());
    return std::get<2>(*it);
}


auto ErrorCategory::toCode() const noexcept -> int {
    auto it = std::ranges::find_if(_valueMap, [this](const ValueEntry &entry) -> bool {
        return std::get<0>(entry) == _value;
    });
    assert(it != _valueMap.end());
    return std::get<1>(*it);
}


}

