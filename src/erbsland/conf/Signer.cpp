// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Signer.hpp"


#include "impl/Signer.hpp"


namespace erbsland::conf {


Signer::Signer(const SignatureSignerPtr &signatureSigner) : _signatureSigner{signatureSigner} {
    if (_signatureSigner == nullptr) {
        throw std::invalid_argument("Signature signer must not be null");
    }
}


void Signer::sign(std::filesystem::path sourcePath, std::filesystem::path destinationPath, String signingPersonText) {
    return impl::Signer{_signatureSigner}.sign(sourcePath, destinationPath, signingPersonText);
}


}


