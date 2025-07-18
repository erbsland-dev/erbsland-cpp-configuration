# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(erbsland-configuration-parser PRIVATE
        FileSource.cpp
        FileSource.hpp
        StreamSource.cpp
        StreamSource.hpp
        StreamTestInterface.hpp
        StringSource.cpp
        StringSource.hpp
)

