# Copyright (c) 2024 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

add_subdirectory(impl)

target_sources(erbsland-configuration-parser PRIVATE
        AccessCheck.hpp
        AccessCheckResult.hpp
        AccessSources.hpp
        Bytes.cpp
        Bytes.hpp
        FileAccessCheck.cpp
        FileAccessCheck.hpp
        Date.cpp
        Date.hpp
        DateTime.cpp
        DateTime.hpp
        Document.hpp
        DocumentBuilder.hpp
        Error.cpp
        Error.hpp
        ErrorCategory.cpp
        ErrorCategory.hpp
        EscapeMode.hpp
        FileSourceResolver.cpp
        FileSourceResolver.hpp
        Float.hpp
        Integer.hpp
        Location.cpp
        Location.hpp
        Name.cpp
        Name.hpp
        NamePath.cpp
        NamePath.hpp
        NameType.hpp
        Parser.cpp
        Parser.hpp
        Position.cpp
        Position.hpp
        SignatureSigner.hpp
        SignatureSignerData.hpp
        SignatureValidator.hpp
        SignatureValidatorData.hpp
        SignatureValidatorResult.hpp
        Signer.cpp
        Signer.hpp
        Source.cpp
        Source.hpp
        SourceIdentifier.hpp
        SourceResolverContext.hpp
        String.cpp
        String.hpp
        Time.cpp
        Time.hpp
        TimeDelta.cpp
        TimeDelta.hpp
        TimeOffset.cpp
        TimeOffset.hpp
        TimeUnit.cpp
        TimeUnit.hpp
        Value.cpp
        Value.hpp
        ValueIterator.cpp
        ValueIterator.hpp
        ValueType.cpp
        ValueType.hpp
)
