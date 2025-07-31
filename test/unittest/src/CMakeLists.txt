# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(assignment_stream)
add_subdirectory(decoder)
add_subdirectory(document_api)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(string)
add_subdirectory(utf8)
add_subdirectory(value_types)

target_sources(unittest PRIVATE
        DefaultsTest.cpp
        DocumentBuilderImplTest.cpp
        DocumentBuilderStorageTest.cpp
        DocumentBuilderTest.cpp
        ErrorCategoryTest.cpp
        ErrorTest.cpp
        FileSourceResolverTest.cpp
        FileSourceTest.cpp
        GeneratorTest.cpp
        HashHelperTest.cpp
        LocationTest.cpp
        NumberBaseTest.cpp
        PositionTest.cpp
        SaturationMathTest.cpp
        ShaHashTest.cpp
        SourceCreateTest.cpp
        SourceIdentifierTest.cpp
        StringSourceTest.cpp
        TestFormatTest.cpp
        TestHelper.hpp
        TokenTypeTest.cpp
        main.cpp
)

