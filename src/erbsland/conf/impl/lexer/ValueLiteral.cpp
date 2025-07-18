// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ValueLiteral.hpp"


#include "LiteralTables.hpp"

#include "../YieldMacros.hpp"


namespace erbsland::conf::impl::lexer {


auto scanLiteral(TokenDecoder &decoder) -> std::optional<LexerToken> {
    if (decoder.character() != CharClass::Letter) {
        return std::nullopt;
    }
    auto transaction = Transaction{decoder};
    while (decoder.character() == CharClass::Letter) {
        decoder.next();
        if (transaction.capturedSize() > 8) {
            decoder.throwSyntaxError(u8"Unknown value literal.");
        }
    }
    const auto identifier = transaction.capturedLowerCaseString();
    if (identifier == u8"t" && decoder.character() == CharClass::DecimalDigit) {
        // This is most likely a time prefix - backtracking.
        return std::nullopt;
    }
    if (decoder.character() != CharClass::ValidAfterValue) {
        decoder.throwSyntaxError(u8"Unexpected character after literal.");
    }
    const auto findIt = LiteralTables::identifierMap.find(identifier);
    if (findIt == LiteralTables::identifierMap.end()) {
        decoder.throwSyntaxError(u8"Unknown value literal.");
    }
    transaction.commit();
    return decoder.createToken(findIt->second.type, findIt->second.value);
}


}


