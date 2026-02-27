# Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

# Set up the installation.
set(_erbsland_configuration_install_targets erbsland-configuration-parser)
if(ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT STREQUAL "none")
    # Parser-only installation (default)
elseif(ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT STREQUAL "re-disabled")
    list(APPEND _erbsland_configuration_install_targets erbsland-configuration-vr-re-disabled)
elseif(ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT STREQUAL "re-std")
    list(APPEND _erbsland_configuration_install_targets erbsland-configuration-vr-re-std)
elseif(ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT STREQUAL "re-erbsland")
    list(APPEND _erbsland_configuration_install_targets erbsland-configuration-vr-re-erbsland)
elseif(ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT STREQUAL "all")
    list(APPEND _erbsland_configuration_install_targets
            erbsland-configuration-vr-re-disabled
            erbsland-configuration-vr-re-std
            erbsland-configuration-vr-re-erbsland)
else()
    message(FATAL_ERROR "Invalid value for ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT: "
            "${ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT}. Valid values are: none, re-disabled, re-std, re-erbsland, all.")
endif()

install(TARGETS ${_erbsland_configuration_install_targets}
        EXPORT erbsland-config-parser-targets
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
)
install(DIRECTORY include/
        DESTINATION include
)
install(DIRECTORY src/erbsland/conf/
        DESTINATION src/erbsland/conf
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.tpp"
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
        "${CMAKE_CURRENT_LIST_DIR}/erbsland-configuration-parserConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-configuration-parserConfig.cmake"
        INSTALL_DESTINATION lib/cmake/erbsland-configuration-parser
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-configuration-parserConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-configuration-parserConfigVersion.cmake"
        DESTINATION lib/cmake/erbsland-configuration-parser
)
