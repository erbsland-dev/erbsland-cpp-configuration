// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


namespace erbsland::conf::impl {


// @notest Tested via various container implementations.


/// Re-define the types of the wrapped container.
///
/// @param WRAPPED_TYPE The type to wrap.
///
#define ERBSLAND_CONF_CONTAINER_DEFINITIONS(WRAPPED_TYPE) \
    using value_type = WRAPPED_TYPE::value_type; \
    using size_type = WRAPPED_TYPE::size_type; \
    using difference_type = WRAPPED_TYPE::difference_type; \
    using reference = WRAPPED_TYPE::reference; \
    using const_reference = WRAPPED_TYPE::const_reference; \
    using pointer = WRAPPED_TYPE::pointer; \
    using const_pointer = WRAPPED_TYPE::const_pointer; \
    using iterator = WRAPPED_TYPE::iterator; \
    using const_iterator = WRAPPED_TYPE::const_iterator; \
    using reverse_iterator = WRAPPED_TYPE::reverse_iterator; \
    using const_reverse_iterator = WRAPPED_TYPE::const_reverse_iterator;

/// Implement common access method of a container.
///
/// Expects that the container definitions, via `ERBSLAND_CONF_CONTAINER_DEFINITIONS` are already defined.
///
/// @see ERBSLAND_CONF_CONTAINER_DEFINITIONS
///
#define ERBSLAND_CONF_CONTAINER_ACCESS_METHODS(WRAPPED_TYPE, MEMBER_VARIABLE) \
    auto operator[](size_type pos) noexcept -> value_type& { return (MEMBER_VARIABLE)[pos]; } \
    auto operator[](size_type pos) const noexcept -> value_type { return (MEMBER_VARIABLE)[pos]; } \
    [[nodiscard]] constexpr auto at(size_type pos) -> value_type& { return (MEMBER_VARIABLE).at(pos); } \
    [[nodiscard]] constexpr auto at(size_type pos) const -> value_type { return (MEMBER_VARIABLE).at(pos); } \
    [[nodiscard]] constexpr auto front() const noexcept -> value_type { return (MEMBER_VARIABLE).front(); } \
    [[nodiscard]] constexpr auto back() const noexcept -> value_type { return (MEMBER_VARIABLE).back(); } \
    [[nodiscard]] constexpr auto front() noexcept -> reference { return (MEMBER_VARIABLE).front(); } \
    [[nodiscard]] constexpr auto back() noexcept -> reference { return (MEMBER_VARIABLE).back(); } \
    [[nodiscard]] constexpr auto begin() noexcept -> iterator { return (MEMBER_VARIABLE).begin(); } \
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator { return (MEMBER_VARIABLE).begin(); } \
    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator { return (MEMBER_VARIABLE).cbegin(); } \
    [[nodiscard]] constexpr auto rbegin() noexcept -> reverse_iterator { return (MEMBER_VARIABLE).rbegin(); } \
    [[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator { return (MEMBER_VARIABLE).rbegin(); } \
    [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator { return (MEMBER_VARIABLE).crbegin(); } \
    [[nodiscard]] constexpr auto end() noexcept -> iterator { return (MEMBER_VARIABLE).end(); } \
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator { return (MEMBER_VARIABLE).end(); } \
    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator { return (MEMBER_VARIABLE).cend(); } \
    [[nodiscard]] constexpr auto rend() noexcept -> reverse_iterator { return (MEMBER_VARIABLE).rend(); } \
    [[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator { return (MEMBER_VARIABLE).rend(); } \
    [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator { return (MEMBER_VARIABLE).crend(); } \
    [[nodiscard]] constexpr auto size() const noexcept -> size_type { return (MEMBER_VARIABLE).size(); } \
    [[nodiscard]] constexpr auto empty() const noexcept -> bool { return (MEMBER_VARIABLE).empty(); } \
    [[nodiscard]] constexpr auto data() const noexcept -> const_pointer { return (MEMBER_VARIABLE).data(); } \
    [[nodiscard]] constexpr auto data() noexcept -> pointer { return (MEMBER_VARIABLE).data(); } \
    [[nodiscard]] auto raw() const noexcept -> const WRAPPED_TYPE& { return (MEMBER_VARIABLE); } \
    [[nodiscard]] auto raw() noexcept -> WRAPPED_TYPE& { return (MEMBER_VARIABLE); }

#define ERBSLAND_CONF_CONTAINER_PUSH_BACK(MEMBER_VARIABLE) \
    template <typename Fwd> \
    void push_back(Fwd&& value) noexcept { (MEMBER_VARIABLE).push_back(std::forward<Fwd>(value)); }

}

