// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "PrivateTag.hpp"

#include "../utf8/U8Format.hpp"

#include "../../String.hpp"

#include <algorithm>
#include <format>
#include <memory>
#include <ranges>
#include <variant>
#include <vector>


namespace erbsland::conf::impl {


class InternalView;
using InternalViewPtr = std::shared_ptr<InternalView>;


/// Get the internal view of an object.
///
template<typename T>
auto internalView(const T& /*object*/) -> InternalViewPtr {
    // uncomment for active development:
    static_assert(false, "missing specialization");
    throw std::runtime_error("Not implemented");
}

/// The internal view structure for testing and debugging.
///
/// This class helps debugging and testing this parser by providing insights into the internals, without
/// actually changing the functionality of the objects. By having a friend function `internalView`, gathering
/// internals works like `std::to_string` and allows implementing safe access to internals, without the risk
/// after altering its functionality in release builds.
///
/// As an extra safety measure, the macro `ERBSLAND_CONF_INTERNAL_VIEW` is in place to remove these functions
/// entirely from any production build.
///
/// @notest This is only used in development builds.
///
class InternalView {
private:
    using Value = std::variant<String, InternalViewPtr>;

public:
    explicit InternalView(PrivateTag) noexcept {}
    InternalView(const String &name, const Value &value, PrivateTag) noexcept {
        _values.emplace_back(name, value);
    }
    [[nodiscard]] static auto create() noexcept -> InternalViewPtr {
        return std::make_shared<InternalView>(PrivateTag{});
    }
    [[nodiscard]] static auto create(const String &name, const Value &value) noexcept -> InternalViewPtr {
        return std::make_shared<InternalView>(name, value, PrivateTag{});
    }

    /// Create a view to a list of objects.
    ///
    /// @tparam Iter The iterator type
    /// @param maxElements The maximum number of elements or 0 for no limit.
    /// @param begin The begin iterator to the list.
    /// @param end The end iterator to the list.
    /// @return The view instance.
    ///
    template<typename Iter>
    [[nodiscard]] static auto createList(
        const std::size_t maxElements,
        const Iter &begin,
        const Iter &end) noexcept -> InternalViewPtr {

        auto result = std::make_shared<InternalView>(PrivateTag{});
        result->setValue("size", std::distance(begin, end));
        std::size_t index = 0;
        for (auto it = begin; it != end; ++it) {
            result->setValue(String{std::format("{:04}", index++)}, internalView(*it));
            if (maxElements > 0 && index >= maxElements) {
                break;
            }
        }
        return result;
    }
    /// Create a view to a list of objects.
    ///
    /// @tparam Iter The iterator type
    /// @param begin The begin iterator to the list.
    /// @param end The end iterator to the list.
    /// @return The view instance.
    ///
    template<typename Iter>
    [[nodiscard]] static auto createNamedList(
        const Iter &begin,
        const Iter &end,
        const std::function<String(const typename Iter::value_type&)> &nameFunc) noexcept -> InternalViewPtr {

        auto result = std::make_shared<InternalView>(PrivateTag{});
        for (auto it = begin; it != end; ++it) {
            result->setValue(nameFunc(*it), internalView(*it));
        }
        return result;
    }
    void removeValue(const String &name) noexcept {
        _values.erase(std::ranges::remove_if(_values, [&](const auto &pair) -> auto {
            return pair.first == name;
        }).begin(), _values.end());
    }
    void setValue(const String &name, const Value &value) noexcept {
        removeValue(name);
        _values.emplace_back(name, value);
    }
    void setValue(const String &name, const String &value) noexcept {
        removeValue(name);
        _values.emplace_back(name, value);
    }
    void setValue(const String &name, const InternalViewPtr &value) noexcept {
        removeValue(name);
        _values.emplace_back(name, value);
    }
    void setUnsafeText(const String &name, const String &text, const String &textIfEmpty = u8"<empty>") {
        if (text.empty()) {
            setValue(name, textIfEmpty);
        } else {
            setValue(name, u8format("\"{}\"", text.toSafeText()));
        }
    }
    void setValue(const String &name, const std::string_view value) noexcept {
        setValue(name, String{value.data(), value.size()});
    }
    template<std::size_t N>
    void setValue(const String &name, const char8_t (&literal)[N]) noexcept {
        setValue(name, String{literal, N - 1});
    }
    void setValue(const String &name, const std::u8string &value) noexcept {
        setValue(name, String{value});
    }
    void setValue(const String &name, const std::string &value) noexcept {
        setValue(name, String{value});
    }
    void setValue(const String &name, const bool value) noexcept {
        setValue(name, String{value ? std::u8string_view{u8"true"} : std::u8string_view{u8"false"}});
    }
    template<typename T> requires (std::is_integral_v<T>)
    void setValue(const String &name, const T &value) noexcept {
        setValue(name, String{std::to_string(value)});
    }
    template<typename T>
    void setValue(const String &name, const T &value) noexcept {
        setValue(name, internalView(value));
    }
    [[nodiscard]] auto toString(const std::size_t indent = 0) const noexcept -> String {
        String result;
        for (auto const &line : toLines(indent)) {
            result.append(line);
            result.append(String{u8"\n"});
        }
        return result;
    }
private:
    [[nodiscard]] auto toLines(const std::size_t indent = 0) const noexcept -> std::vector<String> {
        std::vector<String> lines;
        lines.reserve(_values.size());
        const String indentString(indent, ' ');
        for (auto const &[name, value] : _values) {
            std::visit(
                [&]<typename VisitedType>(VisitedType const &visitedValue) -> void {
                    using DecayType = std::decay_t<VisitedType>;
                    if constexpr (std::is_same_v<DecayType, InternalViewPtr>) {
                        lines.emplace_back(u8format("{}{}:", indentString, name)); // parent heading
                        auto childLines = visitedValue->toLines(indent + 2); // recurse + flatten
                        lines.insert(lines.end(),
                                     std::make_move_iterator(childLines.begin()),
                                     std::make_move_iterator(childLines.end()));
                    } else if constexpr (std::is_same_v<DecayType, String>) {
                        lines.emplace_back(u8format("{}{}: {}", indentString, name, visitedValue)); // leaf entry
                    }
                },
                value
            );
        }
        return lines;
    }

private:
    std::vector<std::pair<String, Value>> _values;
};


}

