// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../impl/value/Value.hpp"
#include "../../RegEx.hpp"
#include "../../Time.hpp"
#include "../../TimeDelta.hpp"

#include <string>
#include <vector>


namespace erbsland::conf::vr::builder::detail {


[[nodiscard]] auto createDefaultValue(Integer value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(bool value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(Float value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const String &value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const char *value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const std::string &value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const Date &value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const Time &value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const DateTime &value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const Bytes &value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const TimeDelta &value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const RegEx &value) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const std::vector<Integer> &values) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const std::vector<bool> &values) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const std::vector<Float> &values) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const std::vector<String> &values) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const std::vector<Bytes> &values) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const std::vector<std::vector<Integer>> &values) -> impl::ValuePtr;
[[nodiscard]] auto createDefaultValue(const std::vector<std::vector<Float>> &values) -> impl::ValuePtr;


}
