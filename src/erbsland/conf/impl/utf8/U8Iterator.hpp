// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "U8Decoder.hpp"

#include "../char/Char.hpp"

#include "../../String.hpp"

#include <iterator>


namespace erbsland::conf::impl {


/// An internal UTF-8 iterator for make code for low-level string processing more readable.
///
/// @warning This is no user code, because iterators aren't safe.
/// An instance of this class must be used in the lifetime-scope of the processed string.
/// The string must not change while using this iterator.
/// This is designed like any std-library iterator, with the same flaws.
///
/// @tested `U8IteratorTest`
///
class U8Iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Char;
    using difference_type = std::size_t;
    using pointer = const Char*;
    using reference = const Char&;

public:
    explicit U8Iterator(const std::u8string_view view, const std::size_t startPosition, PrivateTag /*pt*/)
    :
        _stringView{view},
        _currentPosition(startPosition),
        _nextPosition(startPosition)
    {
        readCurrentChar();
    }
    ~U8Iterator() = default;
    U8Iterator(const U8Iterator&) = default;
    U8Iterator(U8Iterator&&) = default;
    auto operator=(const U8Iterator&) -> U8Iterator& = default;
    auto operator=(U8Iterator&&) -> U8Iterator& = default;

public: // operators
    auto operator*() const noexcept -> reference {
        return _currentChar;
    }
    auto operator->() const noexcept -> pointer {
        return &_currentChar;
    }
    auto operator++() noexcept -> U8Iterator& {
        _currentPosition = _nextPosition;
        readCurrentChar();
        return *this;
    }
    auto operator++(int) noexcept -> U8Iterator {
        auto copy = *this;
        _currentPosition = _nextPosition;
        readCurrentChar();
        return copy;
    }
    auto operator+=(difference_type n) noexcept -> U8Iterator& {
        if (n <= 0) {
            return *this;
        }
        for (difference_type i = 0; i < n; ++i) {
            ++(*this);
        }
        return *this;
    }
    auto operator==(const U8Iterator &other) const noexcept -> bool {
        return _currentPosition == other._currentPosition;
    }
    auto operator!=(const U8Iterator &other) const noexcept -> bool {
        return !(*this == other);
    }

public:
    static auto begin(const String &str) -> U8Iterator {
        return U8Iterator{str.raw(), 0, PrivateTag{}};
    }
    static auto begin(const std::u8string_view stringView) -> U8Iterator {
        return U8Iterator{stringView, 0, PrivateTag{}};
    }
    static auto end(const String &str) -> U8Iterator {
        return U8Iterator{str.raw(), str.size(), PrivateTag{}};
    }
    static auto end(const std::u8string_view stringView) -> U8Iterator {
        return U8Iterator{stringView, stringView.size(), PrivateTag{}};
    }

private:
    void readCurrentChar() {
        if (_currentPosition >= _stringView.size()) {
            _currentChar = Char::EndOfData;
        } else {
            _currentChar = U8Decoder<const char8_t>::decodeChar(_stringView, _nextPosition);
        }
    }

private:
    std::u8string_view _stringView; ///< A view to the data of the string.
    std::size_t _currentPosition; ///< The current position of the iterator and the decoded character.
    std::size_t _nextPosition; ///< The next position, when the iterator is incremented.
    Char _currentChar = Char::EndOfData; ///< The current character the iterator is pointing to.
};


}

