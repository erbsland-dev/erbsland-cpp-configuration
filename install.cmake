# Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

# Set up the installation.
install(TARGETS erbsland-configuration-parser
        EXPORT erbsland-config-parser-targets
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
)
install(DIRECTORY include/
        DESTINATION include
)
install(DIRECTORY src/erbsland/conf/
        DESTINATION src/erbsland/conf
        FILES_MATCHING PATTERN "*.hpp"
)
install(EXPORT erbsland-config-parser-targets
        FILE erbsland-config-parser-targets.cmake
        NAMESPACE ErbslandDEV::
        DESTINATION lib/cmake/erbsland-configuration-parser
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-configuration-parserConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/erbsland-configuration-parserConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-configuration-parserConfig.cmake"
        INSTALL_DESTINATION lib/cmake/erbsland-configuration-parser
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-configuration-parserConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-configuration-parserConfigVersion.cmake"
        DESTINATION lib/cmake/erbsland-configuration-parser
)

