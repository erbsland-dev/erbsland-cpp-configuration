# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

add_subdirectory(attribute)
add_subdirectory(constraint)

target_sources(_erbsland-configuration-vr PRIVATE
        Attributes.hpp
        Constraints.hpp
        DefaultValueFactory.cpp
        DefaultValueFactory.hpp
        NamePathHelper.cpp
        NamePathHelper.hpp
        TypeTraits.hpp
)
