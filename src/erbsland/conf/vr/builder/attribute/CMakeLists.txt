# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(_erbsland-configuration-vr PRIVATE
        Attribute.hpp
        CaseSensitive.hpp
        CustomError.hpp
        Default.hpp
        Dependency.hpp
        Description.hpp
        IsOptional.hpp
        IsSecret.hpp
        KeyIndex.hpp
        MaximumVersion.hpp
        MinimumVersion.hpp
        Title.hpp
        Type.hpp
        Version.hpp
)
