// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Char.hpp"
#include "../InternalView.hpp"
#include "../../Bytes.hpp"
#include "../../Position.hpp"
#include "../../String.hpp"

#include <format>
#include <span>


namespace erbsland::conf::impl {


/// Represents a single decoded character from the line.
///
/// @tested DecodedCharTest
///
class DecodedChar final : public Char {
public:
    /// Create a new decoded character.
    ///
    constexpr DecodedChar(
        const char32_t unicode,
        const std::size_t index,
        const Position position) noexcept
    :
        Char{unicode},
        _index{index},
        _position{position} {
    }

    // defaults
    DecodedChar() = default;
    ~DecodedChar() = default;
    DecodedChar(const DecodedChar &) = default;
    DecodedChar(DecodedChar &&) = default;
    auto operator=(const DecodedChar &) -> DecodedChar& = default;
    auto operator=(DecodedChar &&) -> DecodedChar& = default;

public: // Accessors
    /// The start *byte*-index of this character in the current line.
    ///
    [[nodiscard]] constexpr auto index() const noexcept -> std::size_t { return _index; }

    /// The position of this character in the document.
    ///
    [[nodiscard]] constexpr auto position() const noexcept -> Position { return _position; }

public: // testing
#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
    friend auto internalView(const DecodedChar &object) -> InternalViewPtr {
        auto result = InternalView::create();
        result->setValue("unicode", std::format("0x{:04x}", static_cast<uint32_t>(object._unicode)));
        result->setValue("index", std::format("0x{:04x}", object._index));
        result->setValue("position", object._position);
        return result;
    }
#endif

private:
    std::size_t _index{}; ///< The start index of this character in the current line.
    Position _position; ///< The position of this character in the document.
};


}

