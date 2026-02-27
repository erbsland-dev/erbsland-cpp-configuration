// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Signer.hpp"


#include "impl/sign/Signer.hpp"

#include <utility>


namespace erbsland::conf {


Signer::Signer(SignatureSignerPtr signatureSigner) : _signatureSigner{std::move(signatureSigner)} {
    if (_signatureSigner == nullptr) {
        throw std::invalid_argument("Signature signer must not be null");
    }
}


void Signer::sign(
    std::filesystem::path sourcePath,
    std::filesystem::path destinationPath,
    String signingPersonText) {

    impl::Signer{_signatureSigner}.sign(
        std::move(sourcePath),
        std::move(destinationPath),
        std::move(signingPersonText));
}


}


