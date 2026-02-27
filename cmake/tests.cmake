# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

include(${CMAKE_CURRENT_LIST_DIR}/debug-warnings.cmake)

if(ERBSLAND_CONFIGURATION_ENABLE_TESTS)
    # Reuse the files from the regular library to build the test library with some extensions.
    add_library(erbsland-configuration-parser-for-test STATIC)
    get_target_property(_sources erbsland-configuration-parser SOURCES)
    target_sources(erbsland-configuration-parser-for-test PRIVATE ${_sources})
    get_target_property(_sources erbsland-configuration-vr-re-std SOURCES)
    target_sources(erbsland-configuration-parser-for-test PRIVATE ${_sources})
    target_compile_definitions(erbsland-configuration-parser-for-test PUBLIC
            ERBSLAND_UNITTEST_BUILD=1
            ERBSLAND_CONF_INTERNAL_VIEWS=1
            ERBSLAND_CONF_VR_RE_STD=1)
    erbsland_set_required_compiler_options(erbsland-configuration-parser-for-test)
    erbsland_enable_debug_warnings(erbsland-configuration-parser-for-test)

    # Add the test subdirectory.
    add_subdirectory(test)

    # Enable unit tests
    enable_testing()
    add_test(
            NAME unittest
            COMMAND $<TARGET_FILE:unittest> --no-color
    )
    # Provide the directory of the test suite for compliance tests.
    set_tests_properties(
            unittest
            PROPERTIES ENVIRONMENT "ERBSLAND_CONF_TEST_SUITE=${CMAKE_CURRENT_SOURCE_DIR}/test/erbsland-lang-config-tests"
    )
    # Integrate the full conformance test-suite
    add_test(
            NAME conformance_test
            COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test/erbsland-lang-config-tests/conformance_test.py $<TARGET_FILE:testadapter>
    )
endif()
