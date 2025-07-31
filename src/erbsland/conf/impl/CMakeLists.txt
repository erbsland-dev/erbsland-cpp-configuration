# Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

add_subdirectory(crypto)
add_subdirectory(decoder)
add_subdirectory(lexer)
add_subdirectory(source)
add_subdirectory(utf8)
add_subdirectory(value)

target_sources(erbsland-configuration-parser PRIVATE
        Assignment.hpp
        AssignmentGenerator.hpp
        AssignmentStream.cpp
        AssignmentStream.hpp
        AssignmentType.hpp
        Char.cpp
        Char.hpp
        CharClass.hpp
        CharStream.cpp
        CharStream.hpp
        ComparisonHelper.hpp
        ContainerWrappingHelper.hpp
        Content.hpp
        Defaults.hpp
        Definitions.hpp
        Generator.hpp
        HashHelper.hpp
        InternalView.hpp
        Lexer.cpp
        Lexer.hpp
        LexerToken.hpp
        Limits.hpp
        NameLexer.hpp
        NumberBase.hpp
        Parser.hpp
        ParserContext.hpp
        ParserSettings.hpp
        PrivateTag.hpp
        SaturationMath.hpp
        Signer.hpp
        TestTextHelper.hpp
        TimeHelper.cpp
        TimeHelper.hpp
        TokenGenerator.hpp
        TokenType.hpp
        ValueTreeHelper.hpp
        YieldMacros.hpp
)
