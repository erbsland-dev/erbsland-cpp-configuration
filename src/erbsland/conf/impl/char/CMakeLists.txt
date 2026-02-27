# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(erbsland-configuration-parser PRIVATE
        Char.cpp
        Char.hpp
        CharClass.hpp
        CharRanges.hpp
        CharRanges.hpp
        CharStream.cpp
        CharStream.hpp
)

