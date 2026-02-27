# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

add_subdirectory(builder)

target_sources(_erbsland-configuration-vr PRIVATE
        Constraint.hpp
        ConstraintType.cpp
        ConstraintType.hpp
        Rule.hpp
        Rules.cpp
        Rules.hpp
        RulesBuilder.cpp
        RulesBuilder.hpp
        RuleType.cpp
        RuleType.hpp
)
