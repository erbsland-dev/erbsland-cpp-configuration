# Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0


cmake_minimum_required(VERSION 3.23)


project(erbsland-configuration-testadapter)


add_executable(testadapter)

# Add the source directory, to test implementations and not just the interface.
target_include_directories(testadapter PRIVATE ../../src)
set_property(TARGET testadapter PROPERTY CXX_STANDARD 20)
target_compile_features(testadapter PRIVATE cxx_std_20)
target_link_libraries(testadapter PRIVATE erbsland-configuration-parser-for-test)

add_subdirectory(src)
