# Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

project(erbsland-configuration-testadapter LANGUAGES CXX)

add_executable(testadapter)
erbsland_set_required_compiler_options(testadapter)
erbsland_enable_debug_warnings(testadapter)

# Add the source directory, to test implementations and not just the interface.
target_include_directories(testadapter PRIVATE ../../src)

# Link to the configuration parser.
target_link_libraries(testadapter PRIVATE erbsland-configuration-parser)

add_subdirectory(src)
