# Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(erbsland-configuration-parser PRIVATE
        Core.cpp
        Core.hpp
        Name.cpp
        Name.hpp
        Number.cpp
        Number.hpp
        Text.cpp
        Text.hpp
        Section.cpp
        Section.hpp
        Value.cpp
        Value.hpp
        ValueDateTime.cpp
        ValueDateTime.hpp
        ValueFloat.cpp
        ValueFloat.hpp
        ValueLiteral.cpp
        ValueLiteral.hpp
        ValueInteger.cpp
        ValueInteger.hpp
        ValueText.cpp
        ValueText.hpp
        ValueBytes.cpp
        ValueBytes.hpp
        ValueMultiLine.cpp
        ValueMultiLine.hpp
        LiteralTables.hpp
)
