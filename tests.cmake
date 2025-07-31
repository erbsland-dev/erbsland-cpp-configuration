# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)


if(ERBSLAND_CONFIGURATION_ENABLE_TESTS)
    # Reuse the files from the regular library to build the test library with some extensions.
    get_target_property(_sources erbsland-configuration-parser SOURCES)
    add_library(erbsland-configuration-parser-for-test STATIC)
    target_sources(erbsland-configuration-parser-for-test PRIVATE ${_sources})
    target_compile_definitions(erbsland-configuration-parser-for-test PUBLIC
            ERBSLAND_UNITTEST_BUILD=1
            ERBSLAND_CONF_INTERNAL_VIEWS=1)

    # Set the linker language
    set_target_properties(erbsland-configuration-parser-for-test PROPERTIES LINKER_LANGUAGE CXX)

    # Enforce a C++20 build.
    target_compile_features(erbsland-configuration-parser-for-test PUBLIC cxx_std_20)

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

