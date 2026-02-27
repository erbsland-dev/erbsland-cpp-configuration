# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(assignment)
add_subdirectory(char)
add_subdirectory(constants)
add_subdirectory(crypto)
add_subdirectory(decoder)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(public_api)
add_subdirectory(source)
add_subdirectory(utf8)
add_subdirectory(utilities)
add_subdirectory(value)
add_subdirectory(vr)

target_sources(unittest PRIVATE
        TestHelper.hpp
        main.cpp
)

