# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(erbsland-configuration-parser PRIVATE
        Assignment.hpp
        AssignmentGenerator.hpp
        AssignmentStream.cpp
        AssignmentStream.hpp
        AssignmentType.hpp
)

