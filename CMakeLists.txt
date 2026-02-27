# Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

project(erbsland-configuration-parser LANGUAGES CXX)

# Option if unit tests should be enabled in this build.
# - Keep 'OFF' if you use this library.
# - Set of 'ON' if you are working on this library.
option(ERBSLAND_CONFIGURATION_ENABLE_TESTS "Enable unit tests" OFF)
set(ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT "none" CACHE STRING
        "Validation-rules static library variant to install (none, re-disabled, re-std, re-erbsland, all)")
set_property(CACHE ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT PROPERTY STRINGS
        none re-disabled re-std re-erbsland all)

include(cmake/debug-warnings.cmake)
include(cmake/compiler-options.cmake)

# The core configuration parser library.
# This library only builds the parser, without validation.
add_library(erbsland-configuration-parser STATIC)
add_library(erbsland-configuration-parser::core ALIAS erbsland-configuration-parser)
erbsland_set_required_compiler_options(erbsland-configuration-parser)
erbsland_enable_debug_warnings(erbsland-configuration-parser)

# A library, just to collect all filenames for the validation rules library variants.
# We use this method, to provide a simplified target for the IDE to handle.
add_library(_erbsland-configuration-vr STATIC EXCLUDE_FROM_ALL)
erbsland_set_required_compiler_options(_erbsland-configuration-vr)
erbsland_enable_debug_warnings(_erbsland-configuration-vr)

# The validation-rules library, with disabled 'match' constraint.
add_library(erbsland-configuration-vr-re-disabled STATIC ${_vr_sources} EXCLUDE_FROM_ALL)
add_library(erbsland-configuration-parser::vr-re-disabled ALIAS erbsland-configuration-vr-re-disabled)
target_compile_definitions(erbsland-configuration-vr-re-disabled PUBLIC
        ERBSLAND_CONF_VR_RE_DISABLED=1)
target_link_libraries(erbsland-configuration-vr-re-disabled PUBLIC erbsland-configuration-parser)
erbsland_set_required_compiler_options(erbsland-configuration-vr-re-disabled)
erbsland_enable_debug_warnings(erbsland-configuration-vr-re-disabled)

# The validation-rules library, with 'match' using 'std::regex'
add_library(erbsland-configuration-vr-re-std STATIC ${_vr_sources} EXCLUDE_FROM_ALL)
add_library(erbsland-configuration-parser::vr-re-std ALIAS erbsland-configuration-vr-re-std)
target_compile_definitions(erbsland-configuration-vr-re-std PUBLIC
        ERBSLAND_CONF_VR_RE_STD=1)
target_link_libraries(erbsland-configuration-vr-re-std PUBLIC erbsland-configuration-parser)
erbsland_set_required_compiler_options(erbsland-configuration-vr-re-std)
erbsland_enable_debug_warnings(erbsland-configuration-vr-re-std)

# The validation-rules library, with 'match' using 'erbsland-re'
add_library(erbsland-configuration-vr-re-erbsland STATIC ${_vr_sources} EXCLUDE_FROM_ALL)
add_library(erbsland-configuration-parser::vr-re-erbsland ALIAS erbsland-configuration-vr-re-erbsland)
target_compile_definitions(erbsland-configuration-vr-re-erbsland PUBLIC
        ERBSLAND_CONF_VR_RE_ERBSLAND=1)
target_link_libraries(erbsland-configuration-vr-re-erbsland PUBLIC erbsland-configuration-parser)
erbsland_set_required_compiler_options(erbsland-configuration-vr-re-erbsland)
erbsland_enable_debug_warnings(erbsland-configuration-vr-re-erbsland)

# Add all sources to it.
add_subdirectory(src/erbsland/conf)

# Get all sources for the validation-rules.
get_target_property(_vr_sources _erbsland-configuration-vr SOURCES)
# Add the same sources to all validation-rules variants.
target_sources(erbsland-configuration-vr-re-disabled PRIVATE ${_vr_sources})
target_sources(erbsland-configuration-vr-re-std PRIVATE ${_vr_sources})
target_sources(erbsland-configuration-vr-re-erbsland PRIVATE ${_vr_sources})

# Add the include path for integrations and installations
target_include_directories(erbsland-configuration-parser
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(cmake/install.cmake)
include(cmake/tests.cmake)

