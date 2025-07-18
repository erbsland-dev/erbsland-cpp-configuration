# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(erbsland-configuration-parser PRIVATE
        U8Decoder.hpp
        U8Format.hpp
        U8Iterator.hpp
        U8StringView.cpp
        U8StringView.hpp
)

