// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <iostream>


namespace erbsland::conf::impl {


/// An internal interface to simulate errors.
///
/// @notest This interface is only used in development builds.
///
class StreamTestInterface {
public:
    using Stream = std::basic_istream<char>;
    virtual ~StreamTestInterface() = default;

public:
    virtual void afterOpen(Stream &stream) const {}
    virtual void beforeRead(Stream &stream) const {}
    virtual void afterClose(Stream &stream) const {}
};


#ifdef ERBSLAND_UNITTEST_BUILD
#define ERBSLAND_CONF_STREAM_TEST(FN_NAME) if (_testInterface) { _testInterface->FN_NAME(stream()); }
#else
#define ERBSLAND_CONF_STREAM_TEST(FN_NAME)
#endif


}

