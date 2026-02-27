# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(erbsland-configuration-parser PRIVATE
        ComparisonHelper.hpp
        ContainerWrappingHelper.hpp
        Generator.hpp
        HashHelper.hpp
        InternalError.hpp
        InternalView.hpp
        NumberBase.hpp
        PrivateTag.hpp
        SaturationMath.hpp
        TestTextHelper.hpp
        TimeHelper.cpp
        TimeHelper.hpp
        TypeTraits.hpp
        YieldMacros.hpp
)

