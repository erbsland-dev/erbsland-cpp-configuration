// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../String.hpp"
#include "../TestFormat.hpp"
#include "../Value.hpp"

#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>


namespace erbsland::conf::impl {


/// A helper class to create value trees.
///
class ValueTreeHelper {
    struct Frame {
        conf::ConstValuePtr frameValue;
        String indent;
        bool isLast;
    };

public:
    ValueTreeHelper(
        conf::ConstValuePtr rootValue,
        const TestFormat format) noexcept
    :
        _rootValue(std::move(rootValue)),
        _format(format) {
    }

    ~ValueTreeHelper() = default;

    // prevent copy and move.
    ValueTreeHelper(const ValueTreeHelper &) = delete;
    ValueTreeHelper(ValueTreeHelper &&) = delete;
    auto operator=(const ValueTreeHelper &) -> ValueTreeHelper& = delete;
    auto operator=(ValueTreeHelper &&) -> ValueTreeHelper& = delete;

public:
    /// Create a visual value tree.
    ///
    /// @return A list of lines for the output.
    ///
    auto createLines() -> std::vector<String> {
        if (!_lines.empty()) {
            throw std::logic_error("ValueTreeHelper::createLines() called twice.");
        }
        initStack();
        while (!_stack.empty()) {
            const auto [value, indent, isLast] = popFrame();
            const auto name = computeName(value);
            const auto posText = computePosition(value);
            emitLine(value, name, posText, indent, isLast);
            pushChildren(value, indent, isLast);
        }
        if (_format.isSet(TestFormat::ShowSourceIdentifier)) {
            appendSourceLabels();
        }
        return _lines;
    }

private:
    void initStack() noexcept {
        _stack.clear();
        _stack.reserve(64);
        _stack.emplace_back(_rootValue, String{}, true);
    }

    auto popFrame() noexcept -> Frame {
        const auto frame = _stack.back();
        _stack.pop_back();
        return frame;
    }

    [[nodiscard]] static auto computeName(const conf::ConstValuePtr &value) noexcept -> String {
        if (value->isDocument()) {
            return u8"<Document>";
        }
        if (value->namePath().empty()) {
            return u8"<Empty>";
        }
        return value->namePath().back().toPathText();
    }

    [[nodiscard]] auto computePosition(const conf::ConstValuePtr &value) -> String {
        String positionStr;
        if (_format.isSet(TestFormat::ShowPosition) || _format.isSet(TestFormat::ShowSourceIdentifier)) {
            positionStr += u8"[";
            if (_format.isSet(TestFormat::ShowSourceIdentifier)) {
                appendSourceIdentifier(positionStr, value);
            }
            if (_format.isSet(TestFormat::ShowPosition)) {
                positionStr += value->location().position().toText();
            }
            positionStr += u8"]";
        }
        return positionStr;
    }

    auto appendSourceIdentifier(String &positionStr, const conf::ConstValuePtr &value) -> void {
        if (auto sid = value->location().sourceIdentifier(); sid == nullptr) {
            positionStr += u8"no source";
        } else if (const auto labelIt = _labelMap.find(sid); labelIt != _labelMap.end()) {
            positionStr += labelIt->second;
        } else {
            static auto labels = String{u8"ABCDEFGHIJKLMNPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz+"};
            const auto label = String{1, labels[_labelIndex]};
            if (_labelIndex < labels.size() - 2) {
                _labelIndex += 1;
            }
            _labelMap.emplace(sid, label);
            _labelList.emplace_back(label, sid);
            positionStr += label;
        }
        positionStr += u8":";
    }

    void emitLine(
        const conf::ConstValuePtr &value,
        const String &name,
        String pos,
        const String &indent,
        const bool isLast) {

        if (value == _rootValue) {
            _lines.emplace_back(u8format(u8"{} => {}{}", name, value->toTestText(_format), pos));
        } else {
            auto line = indent + (isLast ? u8"└───" : u8"├───");
            line += u8format(u8"{} => {}{}", name, value->toTestText(_format), pos);
            _lines.emplace_back(std::move(line));
        }
    }

    void pushChildren(
        const conf::ConstValuePtr &value,
        const String &indent,
        const bool isLast) {

        const auto count = value->size();
        for (std::size_t i = count; i > 0;) {
            i -= 1;
            const bool last = (i == count - 1);
            const auto child = value->value(i);
            auto childIndent = indent;
            if (value != _rootValue) {
                childIndent += isLast ? u8"    " : u8"│   ";
            }
            _stack.emplace_back(child, std::move(childIndent), last);
        }
    }

    void appendSourceLabels() {
        for (const auto &[label, sourceIdentifier] : _labelList) {
            _lines.emplace_back(u8format(u8"{}: {}", label, sourceIdentifier->toText()));
        }
    }

private:
    // setup
    conf::ConstValuePtr _rootValue;
    TestFormat _format;

    // working variables
    std::vector<String> _lines;
    std::size_t _labelIndex = 0;
    std::unordered_map<SourceIdentifierPtr, String> _labelMap;
    std::vector<std::pair<String, SourceIdentifierPtr>> _labelList;
    std::vector<Frame> _stack;
};


}

