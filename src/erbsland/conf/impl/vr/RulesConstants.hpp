// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../Name.hpp"


namespace erbsland::conf::impl::vrc {

const inline static auto cEmptyName = Name{};
const inline static auto cReservedAny = Name{NameType::Regular, String{u8"vr_any"}, PrivateTag{}};
const inline static auto cReservedTemplate = Name{NameType::Regular, String{u8"vr_template"}, PrivateTag{}};
const inline static auto cReservedName = Name{NameType::Regular, String{u8"vr_name"}, PrivateTag{}};
const inline static auto cReservedEntry = Name{NameType::Regular, String{u8"vr_entry"}, PrivateTag{}};
const inline static auto cReservedKey = Name{NameType::Regular, String{u8"vr_key"}, PrivateTag{}};
const inline static auto cReservedDependency = Name{NameType::Regular, String{u8"vr_dependency"}, PrivateTag{}};
const inline static auto cReservedEscape = String{u8"vr_vr_"};
const inline static auto cReservedPrefix = String{u8"vr_"};

const inline static auto cUseTemplate = Name{NameType::Regular, String{u8"use_template"}, PrivateTag{}};
const inline static auto cType = Name{NameType::Regular, String{u8"type"}, PrivateTag{}};
const inline static auto cCaseSensitive = Name{NameType::Regular, String{u8"case_sensitive"}, PrivateTag{}};

const inline static auto ctChars = String{u8"chars"};
const inline static auto ctContains = String{u8"contains"};
const inline static auto ctDefault = String{u8"default"};
const inline static auto ctDescription = String{u8"description"};
const inline static auto ctEnds = String{u8"ends"};
const inline static auto ctEquals = String{u8"equals"};
const inline static auto ctError = String{u8"error"};
const inline static auto ctIn = String{u8"in"};
const inline static auto ctIsOptional = String{u8"is_optional"};
const inline static auto ctIsSecret = String{u8"is_secret"};
const inline static auto ctKey = String{u8"key"};
const inline static auto ctMatches = String{u8"matches"};
const inline static auto ctMaximum = String{u8"maximum"};
const inline static auto ctMaximumVersion = String{u8"maximum_version"};
const inline static auto ctMinimum = String{u8"minimum"};
const inline static auto ctMinimumVersion = String{u8"minimum_version"};
const inline static auto ctMultiple = String{u8"multiple"};
const inline static auto ctStarts = String{u8"starts"};
const inline static auto ctTitle = String{u8"title"};
const inline static auto ctVersion = String{u8"version"};

const inline static auto ctPrefixNot = String{u8"not_"};
const inline static auto ctSuffixError = String{u8"_error"};

const inline static auto keyName = Name{NameType::Regular, String{u8"name"}, PrivateTag{}};
const inline static auto keyKey = Name{NameType::Regular, String{u8"key"}, PrivateTag{}};

const inline static auto depMode = Name{NameType::Regular, String{u8"mode"}, PrivateTag{}};
const inline static auto depSource = Name{NameType::Regular, String{u8"source"}, PrivateTag{}};
const inline static auto depTarget = Name{NameType::Regular, String{u8"target"}, PrivateTag{}};
const inline static auto depError = Name{NameType::Regular, String{u8"error"}, PrivateTag{}};


}

