// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "crypto/ShaHash.hpp"


namespace erbsland::conf::impl::defaults {


// @tested `DefaultsTest`


/// The hash algorithm to use for the document hash.
///
constexpr auto documentHashAlgorithm = crypto::ShaHash::Algorithm::Sha3_256;

/// The identifier for text sources.
///
constexpr auto textSourceIdentifier = String{u8"text"};

/// The identifier for file sources.
///
constexpr auto fileSourceIdentifier = String{u8"file"};

/// The identifier for name paths.
///
constexpr auto namePathIdentifier = String{u8"name-path"};

/// The configuration language version.
///
constexpr auto languageVersion = String{u8"1.0"};

/// The default file suffix.
///
constexpr auto fileSuffix = String{u8".elcl"};


}

