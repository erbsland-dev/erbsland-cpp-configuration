# Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

add_subdirectory(assignment)
add_subdirectory(char)
add_subdirectory(constants)
add_subdirectory(crypto)
add_subdirectory(decoder)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(sign)
add_subdirectory(source)
add_subdirectory(utf8)
add_subdirectory(utilities)
add_subdirectory(value)
add_subdirectory(vr)

target_sources(erbsland-configuration-parser PRIVATE
        Definitions.hpp
)
