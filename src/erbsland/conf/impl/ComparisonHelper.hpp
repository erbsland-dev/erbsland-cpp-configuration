// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


namespace erbsland::conf::impl {


// @notest Tested via various container implementations.


#define ERBSLAND_CONF_COMPARE_MEMBER(RIGHT_ARG, LEFT_MEMBER, RIGHT_EXPR) \
auto operator<=>(RIGHT_ARG) const noexcept -> std::strong_ordering { return (LEFT_MEMBER) <=> (RIGHT_EXPR); } \
auto operator==(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) == (RIGHT_EXPR); } \
auto operator!=(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) != (RIGHT_EXPR); } \
auto operator<(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) < (RIGHT_EXPR); } \
auto operator<=(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) <= (RIGHT_EXPR); } \
auto operator>(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) > (RIGHT_EXPR); } \
auto operator>=(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) >= (RIGHT_EXPR); } \

#define ERBSLAND_CONF_CONSTEXPR_COMPARE_MEMBER(RIGHT_ARG, LEFT_MEMBER, RIGHT_EXPR) \
constexpr auto operator<=>(RIGHT_ARG) const noexcept -> std::strong_ordering { return (LEFT_MEMBER) <=> (RIGHT_EXPR); } \
constexpr auto operator==(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) == (RIGHT_EXPR); } \
constexpr auto operator!=(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) != (RIGHT_EXPR); } \
constexpr auto operator<(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) < (RIGHT_EXPR); } \
constexpr auto operator<=(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) <= (RIGHT_EXPR); } \
constexpr auto operator>(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) > (RIGHT_EXPR); } \
constexpr auto operator>=(RIGHT_ARG) const noexcept -> bool { return (LEFT_MEMBER) >= (RIGHT_EXPR); } \

#define ERBSLAND_CONF_COMPARE_FRIEND(LEFT_ARG, RIGHT_ARG, LEFT_EXPR, RIGHT_EXPR) \
friend constexpr auto operator<=>(LEFT_ARG, RIGHT_ARG) noexcept -> std::strong_ordering { return (LEFT_EXPR) <=> (RIGHT_EXPR); } \
friend constexpr auto operator==(LEFT_ARG, RIGHT_ARG) noexcept -> bool { return (LEFT_EXPR) == (RIGHT_EXPR); } \
friend constexpr auto operator!=(LEFT_ARG, RIGHT_ARG) noexcept -> bool { return (LEFT_EXPR) != (RIGHT_EXPR); } \
friend constexpr auto operator<(LEFT_ARG, RIGHT_ARG) noexcept -> bool { return (LEFT_EXPR) < (RIGHT_EXPR); } \
friend constexpr auto operator<=(LEFT_ARG, RIGHT_ARG) noexcept -> bool { return (LEFT_EXPR) <= (RIGHT_EXPR); } \
friend constexpr auto operator>(LEFT_ARG, RIGHT_ARG) noexcept -> bool { return (LEFT_EXPR) > (RIGHT_EXPR); } \
friend constexpr auto operator>=(LEFT_ARG, RIGHT_ARG) noexcept -> bool { return (LEFT_EXPR) >= (RIGHT_EXPR); } \


}
