# Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(erbsland-configuration-parser PRIVATE
        BytesValue.cpp
        BytesValue.hpp
        Container.hpp
        Document.cpp
        Document.hpp
        DocumentBuilder.cpp
        DocumentBuilder.hpp
        DocumentBuilderStorage.cpp
        DocumentBuilderStorage.hpp
        Section.hpp
        SectionList.hpp
        ValueList.hpp
        Value.cpp
        Value.hpp
        ValueMap.cpp
        ValueMap.hpp
        ValueWithChildren.cpp
        ValueWithChildren.hpp
        ValueWithConvertibleType.hpp
        ValueWithNativeType.hpp
)
