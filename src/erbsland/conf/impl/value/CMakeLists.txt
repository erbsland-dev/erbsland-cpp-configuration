# Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(erbsland-configuration-parser PRIVATE
        BytesValue.cpp
        BytesValue.hpp
        Container.hpp
        DirectStorageAccess.hpp
        Document.cpp
        Document.hpp
        DocumentBuilder.cpp
        DocumentBuilder.hpp
        DocumentBuilderStorage.cpp
        DocumentBuilderStorage.hpp
        Section.hpp
        SectionList.hpp
        Value.cpp
        Value.hpp
        ValueHelper.hpp
        ValueList.hpp
        ValueMap.cpp
        ValueMap.hpp
        ValueTreeHelper.hpp
        ValueTreeWalker.hpp
        ValueWithChildren.hpp
        ValueWithConvertibleType.hpp
        ValueWithNativeType.hpp
)
