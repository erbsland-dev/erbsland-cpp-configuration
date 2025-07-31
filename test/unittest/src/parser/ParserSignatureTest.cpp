// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ParserTestHelper.hpp"

#include <erbsland/conf/impl/Char.hpp>
#include <erbsland/conf/impl/utf8/U8StringView.hpp>
#include <erbsland/conf/SignatureValidator.hpp>
#include <erbsland/conf/SignatureSigner.hpp>
#include <erbsland/conf/Signer.hpp>
#include <erbsland/conf/impl/Lexer.hpp>
#include <erbsland/conf/impl/crypto/ShaHash.hpp>

#include <vector>
#include <ranges>
#include <algorithm>


TESTED_TARGETS(Parser)
class ParserSignatureTest final : public UNITTEST_SUBCLASS(ParserTestHelper) {
public:
    void setUp() override {
        clearMocks();
    }

    void tearDown() override {
        cleanUpTestFileDirectory();
        doc = {};
    }

    // IMPORTANT: This is no valid example how validation should work!
    class MockSignatureValidator final : public SignatureValidator {
    public:
        auto unshiftString(const String &str) -> String {
            return U8StringView{str}.transformed([](impl::Char c) -> impl::Char {
                return impl::Char(c.raw() - static_cast<char32_t>(1));
            });
        }

        [[nodiscard]] auto validate(const SignatureValidatorData &data) -> SignatureValidatorResult override {
            log.push_back("validate");
            if (data.signatureText.empty()) {
                log.push_back("reject");
                return SignatureValidatorResult::Reject;
            }
            auto pos = data.signatureText.find(u8';');
            if (pos == String::npos) {
                log.push_back("reject");
                return SignatureValidatorResult::Reject;
            }
            auto digest = unshiftString(data.signatureText.substr(pos + 1));
            if (digest != data.documentDigest) {
                log.push_back("reject");
                return SignatureValidatorResult::Reject;
            }
            log.push_back("accept");
            return SignatureValidatorResult::Accept;
        }

        auto require(const std::string &logEntry) noexcept -> bool {
            return std::ranges::find(log, logEntry) != log.end();
        }

        std::vector<std::string> log;
    };

    // IMPORTANT: This is no valid example how signing should work!
    class MockSignatureSigner final : public SignatureSigner {
    public:
        auto shiftString(const String &str) -> String {
            return U8StringView{str}.transformed([](impl::Char c) -> impl::Char {
                return impl::Char(c.raw() + static_cast<char32_t>(1));
            });
        }

        [[nodiscard]] auto sign(const SignatureSignerData &data) -> String override {
            log.push_back("sign");
            String signature;
            signature += data.signingPersonText;
            signature += u8";";
            signature += shiftString(data.documentDigest);
            return signature;
        }

        auto require(const std::string &logEntry) noexcept -> bool {
            return std::ranges::find(log, logEntry) != log.end();
        }

        std::vector<std::string> log;
    };

    std::shared_ptr<MockSignatureValidator> validator = std::make_shared<MockSignatureValidator>();
    std::shared_ptr<MockSignatureSigner> signatureSigner = std::make_shared<MockSignatureSigner>();

    void clearMocks() {
        validator->log.clear();
        signatureSigner->log.clear();
    }

    void testSignatureCycle() {
        // Make sure the premises of this test are correct.
        REQUIRE(impl::Lexer::hashAlgorithm() == impl::crypto::ShaHash::Algorithm::Sha3_256);

        // Prepare the data
        auto unsignedPath = createTestFile("config/unsigned.elcl",
            u8"[main]\nvalue: 123\n");
        auto expectedDigest = // SHA3_256
            Bytes::fromHex(u8"d615780d1836a0189dc5c826f4ef6bfbbf9cc33b78d07b7a1f459c627cec1b81");

        // Verify the document digest.
        impl::crypto::ShaHash hash(impl::crypto::ShaHash::Algorithm::Sha3_256);
        std::ifstream file(unsignedPath, std::ios::binary);
        REQUIRE(file.is_open());
        std::string fileContents(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        REQUIRE_EQUAL(fileContents.size(), 18);
        hash.update(std::span(reinterpret_cast<const std::byte*>(fileContents.data()), fileContents.size()));
        auto actualDigest = hash.digest();
        REQUIRE_EQUAL(actualDigest, expectedDigest);

        // Sign the document.
        auto signedPath = unsignedPath.parent_path() / "signed.elcl";
        Signer signer{signatureSigner};
        REQUIRE_NOTHROW(signer.sign(unsignedPath, signedPath, u8"test@example.com"));
        REQUIRE(exists(signedPath));
        REQUIRE(is_regular_file(signedPath));
        REQUIRE(signatureSigner->require("sign"));

        // Parse the documents.
        Parser parser;
        // reading the unsigned file should be fine.
        REQUIRE_NOTHROW(doc = parser.parseOrThrow(Source::fromFile(unsignedPath)));
        REQUIRE(doc != nullptr);
        // reading the signed must fail.
        REQUIRE_THROWS(doc = parser.parseOrThrow(Source::fromFile(signedPath)));
        REQUIRE(doc != nullptr);
        // setting the validator must reject unsigned documents.
        parser.setSignatureValidator(validator);
        REQUIRE_THROWS(doc = parser.parseOrThrow(Source::fromFile(unsignedPath)));
        // now, reading the signed document should work as expected.
        REQUIRE_NOTHROW(doc = parser.parseOrThrow(Source::fromFile(signedPath)));
        REQUIRE(doc != nullptr);
    }
};

