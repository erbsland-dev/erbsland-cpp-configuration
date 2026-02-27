// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../char/CharStream.hpp"
#include "../utf8/U8Format.hpp"

#include "../../SignatureSigner.hpp"

#include <filesystem>
#include <fstream>


namespace erbsland::conf::impl {


/// The implementation of the signer tool.
///
class Signer final {
public:
    explicit Signer(SignatureSignerPtr signatureSigner) : _signatureSigner{std::move(signatureSigner)} {
        if (_signatureSigner == nullptr) {
            throw std::invalid_argument("Signature signer must not be null");
        }
    }

    // defaults
    ~Signer() = default;

public:
    void sign(
        std::filesystem::path sourcePath,
        std::filesystem::path destinationPath,
        String signingPersonText) {

        auto source = validateAndCreateSource(sourcePath);
        auto [digestText, digest, hasWindowsLineBreaks] = buildDigest(source);
        const SignatureSignerData data{
            .sourceIdentifier = source->identifier(),
            .signingPersonText = std::move(signingPersonText),
            .documentDigest = std::move(digestText)
        };
        source = {};
        auto signatureText = _signatureSigner->sign(data);
        validateAndEscapeSignatureText(signatureText);
        writeSignedFile(sourcePath, destinationPath, signatureText, digest, hasWindowsLineBreaks);
    }

private:
    struct DigestResult final {
        String digestText;
        Bytes digest;
        bool hasWindowsLineEndings;
    };

    auto validateAndCreateSource(std::filesystem::path sourcePath) -> SourcePtr {
        try {
            sourcePath = canonical(sourcePath);
            if (!is_regular_file(sourcePath)) {
                throw Error{ErrorCategory::IO, u8"The source path is no existing regular file.", sourcePath};
            }
            if (file_size(sourcePath) > limits::maxDocumentSize) {
                throw Error{ErrorCategory::LimitExceeded, u8"The source file is too large.", sourcePath};
            }
        } catch (const std::system_error &error) {
            throw Error{
                ErrorCategory::IO,
                u8"Could not validate the source file location or size.",
                sourcePath,
                error.code()};
        }
        return Source::fromFile(sourcePath);
    }

    auto buildDigest(const SourcePtr &source) -> DigestResult {
        source->open();
        CharStream charStream{source};
        charStream.enableHash();
        // read the whole file to verify it's encoding, line lengths and to calculate its hash value.
        auto c = charStream.next();
        bool hasWindowsLineEndings = false;
        while (c != Char::EndOfData) {
            c = charStream.next();
            if (!hasWindowsLineEndings && c == Char::CarriageReturn) {
                hasWindowsLineEndings = true;
            }
        }
        auto digest = charStream.digest();
        String digestText;
        digestText += crypto::ShaHash::algorithmToText(defaults::documentHashAlgorithm);
        digestText += u8" ";
        digestText += digest.toHex();
        source->close();
        return {
            .digestText = std::move(digestText),
            .digest = std::move(digest),
            .hasWindowsLineEndings = hasWindowsLineEndings};
    }

    void validateAndEscapeSignatureText(String &signatureText) {
        if (signatureText.empty()) {
            throw Error{ErrorCategory::Signature, u8"The signature text is empty."};
        }
        if (!signatureText.isValidUtf8()) {
            throw Error{ErrorCategory::Signature, u8"The signature text is not correctly UTF-8 encoded."};
        }
        signatureText = U8StringView{signatureText}.toEscaped(EscapeMode::Text);
        if (signatureText.size() > limits::maxLineLength - 20) {
            throw Error{ErrorCategory::LimitExceeded, u8"The signature text is too long."};
        }
    }

    void writeSignedFile(
        const std::filesystem::path &sourcePath,
        const std::filesystem::path &destinationPath,
        const String &digestText,
        const Bytes &digest,
        bool hasWindowsLineBreaks) {

        try {
            // Open the file
            std::ofstream file{destinationPath, std::ios::binary};
            auto digestTextCharString = digestText.toCharString();
            // Create a line buffer
            std::string buffer;
            buffer.reserve(limits::maxLineLength + 50);

            writePlaceholderSignature(file, buffer, digestTextCharString, hasWindowsLineBreaks);
            auto digestAfterWrite = writeConfiguration(file, buffer, sourcePath);
            if (digest != digestAfterWrite) {
                throw Error{
                    ErrorCategory::Signature,
                    u8"The source file has been modified while writing the signed version."};
            }
            file.seekp(0);
            writeRealSignature(file, buffer, digestTextCharString, hasWindowsLineBreaks);
            file.close();
        } catch (const std::system_error &error) {
            throw Error{
                ErrorCategory::IO,
                u8"Could not write the signed file.",
                destinationPath,
                error.code()};
        }
    }

    void writePlaceholderSignature(
        std::ofstream &file,
        std::string &buffer,
        const std::string &digestTextCharString,
        const bool hasWindowsLineBreaks) {

        buffer.clear();
        buffer += "@signature: \"";
        buffer += std::string(digestTextCharString.size(), '?');
        if (hasWindowsLineBreaks) {
            buffer += "\"\r\n";
        } else {
            buffer += "\"\n";
        }
        file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    }

    auto writeConfiguration(
        std::ofstream &file,
        std::string &buffer,
        const std::filesystem::path &sourcePath) -> Bytes {

        // Reopen the source.
        auto source = Source::fromFile(sourcePath);
        source->open();
        CharStream charStream{source};
        charStream.enableHash();
        auto c = charStream.next();
        // Check if the first line is a signature line and skip it.
        if (charStream.isSignatureLine()) {
            while (c != Char::NewLine && c != Char::EndOfData) {
                c = charStream.next();
            }
            if (c != Char::EndOfData) {
                c = charStream.next();
            }
        }
        buffer.clear();
        while (c != Char::EndOfData) {
            c.appendTo(buffer);
            if (c == Char::NewLine) {
                file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
                buffer.clear();
            }
            c = charStream.next();
        }
        if (!buffer.empty()) {
            file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        }
        return charStream.digest();
    }

    void writeRealSignature(
        std::ofstream &file,
        std::string &buffer,
        const std::string &digestTextCharString,
        const bool hasWindowsLineBreaks) {

        buffer.clear();
        buffer += "@signature: \"";
        buffer += digestTextCharString;
        if (hasWindowsLineBreaks) {
            buffer += "\"\r\n";
        } else {
            buffer += "\"\n";
        }
        file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    }

private:
    SignatureSignerPtr _signatureSigner;
};


}


