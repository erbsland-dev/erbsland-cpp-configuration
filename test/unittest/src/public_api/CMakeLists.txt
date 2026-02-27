# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(document_api)
add_subdirectory(string)
add_subdirectory(value_types)

target_sources(unittest PRIVATE
        DocumentBuilderTest.cpp
        ErrorCategoryTest.cpp
        ErrorTest.cpp
        LocationTest.cpp
        PositionTest.cpp
        SourceIdentifierTest.cpp
        TestFormatTest.cpp
)
