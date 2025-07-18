// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


// Define the `erbsland::conf` namespace.
namespace erbsland::conf {}


// Allow disabling the short namespace if it causes conflicts.
#ifndef ERBSLAND_NO_SHORT_NAMESPACE
#ifdef ERBSLAND_SHORT_NAMESPACE
namespace ERBSLAND_SHORT_NAMESPACE = erbsland;
#else
namespace el = erbsland;
#endif
#endif


#ifdef ERBSLAND_UNITTEST_BUILD
#define ERBSLAND_CONF_ONLY_FOR_UNITTEST(...) __VA_ARGS__
#else
#define ERBSLAND_CONF_ONLY_FOR_UNITTEST(...)
#endif