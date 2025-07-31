# Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

project(erbsland-configuration-parser CXX)
option(ERBSLAND_CONFIGURATION_ENABLE_TESTS "Enable unit tests" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(DEFINED _DEBUG OR DEFINED DEBUG)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "/W4 /WX")
    else()
        set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
    endif()
endif()

# Create the regular library first.
add_library(erbsland-configuration-parser STATIC)

# Add all sources to it.
add_subdirectory(src/erbsland/conf)

# Set the linker language
set_target_properties(erbsland-configuration-parser PROPERTIES LINKER_LANGUAGE CXX)

# Enforce a C++20 build.
target_compile_features(erbsland-configuration-parser PUBLIC cxx_std_20)

# Add the include path for integrations and installations
target_include_directories(erbsland-configuration-parser
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(install.cmake)
include(tests.cmake)


