// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "String.hpp"

#include "impl/utf8/U8Format.hpp"


namespace erbsland::conf {


/// A class to mark text are regular expressions.
///
class RegEx {
public:
    /// Create an empty regular expression value.
    ///
    RegEx() = default;

    /// Create a regular expression value from the given string.
    ///
    /// @param text The text.
    ///
    explicit RegEx(String text) : _text(std::move(text)) {}

public: // operators
    /// Compare for equality.
    auto operator==(const RegEx &other) const noexcept -> bool { return _text == other._text; }
    /// Compare for inequality.
    auto operator!=(const RegEx &other) const noexcept -> bool { return !operator==(other); }

public:
    /// Get the text for this regular expression.
    ///
    [[nodiscard]] auto toText() const noexcept -> const String& { return _text; }

private:
    String _text;
};


}

