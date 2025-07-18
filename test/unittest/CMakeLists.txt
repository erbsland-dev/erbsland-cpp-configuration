# Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

project(unittest)

add_executable(unittest)

# Add the source directory, to test implementations and not just the interface.
target_include_directories(unittest PRIVATE ../../src)
set_property(TARGET unittest PROPERTY CXX_STANDARD 20)
target_compile_features(unittest PRIVATE cxx_std_20)
target_link_libraries(unittest PRIVATE erbsland-configuration-parser-for-test)

add_subdirectory(src)

erbsland_unittest(
        TARGET unittest
        COPY_TEST_DATA data
        ENABLE_DATA_DEPS)
