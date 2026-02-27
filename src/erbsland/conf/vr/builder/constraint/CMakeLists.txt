# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(_erbsland-configuration-vr PRIVATE
        Chars.hpp
        ConstraintAttribute.hpp
        ConstraintOptions.hpp
        Contains.hpp
        Ends.hpp
        Equals.hpp
        In.hpp
        Key.hpp
        Matches.hpp
        Maximum.hpp
        Minimum.hpp
        Multiple.hpp
        Starts.hpp
        StringPartConstraint.hpp
)
