# Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

target_sources(erbsland-configuration-parser PRIVATE
        DecodedChar.hpp
        Decoder.hpp
        FastNameDecoder.hpp
        TokenDecoder.cpp
        TokenDecoder.hpp
        TokenTransactionBuffer.hpp
        Transaction.hpp
        TransactionHandler.hpp
        TransactionStack.hpp
)

