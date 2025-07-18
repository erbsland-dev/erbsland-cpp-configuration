// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/impl/crypto/ShaHash.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <utility>
#include <format>
#include <fstream>


using namespace el::conf;
using impl::crypto::ShaHash;

TESTED_TARGETS(ShaHash)
class ShaHashTest final : public el::UnitTest {
public:
    struct TestFile {
        std::filesystem::path path;
        ShaHash::Algorithm algorithm;
    };
    inline static auto nistFiles = std::array<TestFile, 6>{
        TestFile{"data/sha3/SHA3_256LongMsg.rsp", ShaHash::Algorithm::Sha3_256},
        TestFile{"data/sha3/SHA3_256ShortMsg.rsp", ShaHash::Algorithm::Sha3_256},
        TestFile{"data/sha3/SHA3_384LongMsg.rsp", ShaHash::Algorithm::Sha3_384},
        TestFile{"data/sha3/SHA3_384ShortMsg.rsp", ShaHash::Algorithm::Sha3_384},
        TestFile{"data/sha3/SHA3_512LongMsg.rsp", ShaHash::Algorithm::Sha3_512},
        TestFile{"data/sha3/SHA3_512ShortMsg.rsp", ShaHash::Algorithm::Sha3_512},
    };

    void verifyHash(ShaHash::Algorithm algorithm, const Bytes &message, const Bytes &expectedDigest) {
        // Verify in one go.
        ShaHash hash{algorithm};
        const auto messageSpan = std::span(message.raw().begin(), message.raw().size());
        hash.update(messageSpan);
        auto actualDigest = hash.digest();
        REQUIRE_EQUAL(actualDigest, expectedDigest);
        // Verify in chunks.
        hash.reset();
        constexpr std::size_t chunkSize = 10;
        for (std::size_t i = 0; i < message.size(); i += chunkSize) {
            auto chunk = messageSpan.subspan(i, std::min(chunkSize, message.size() - i));
            hash.update(std::span(chunk.begin(), chunk.size()));
        }
        actualDigest = hash.digest();
        REQUIRE_EQUAL(actualDigest, expectedDigest);
    }

    void testZeroByte() {
        ShaHash hash(ShaHash::Algorithm::Sha3_256);
        hash.update(Bytes::fromHex("00"));
        auto digest = hash.digest();
        auto expectedDigest = Bytes::fromHex("5d53469f20fef4f8eab52b88044ede69c77a6a68a60728609fc4a65ff531e7d0");
        REQUIRE_EQUAL(digest, expectedDigest);
    }

    void testFourByteMessage() {
        ShaHash hash(ShaHash::Algorithm::Sha3_256);
        hash.update(Bytes::fromHex("74657374"));
        auto digest = hash.digest();
        auto expectedDigest = Bytes::fromHex("36f028580bb02cc8272a9a020f4200e346e276ae664e45ee80745574e2f5ab80");
        REQUIRE_EQUAL(digest, expectedDigest);
    }

    void verifyFile(const TestFile& file) {
        auto testFilePath = unitTestExecutablePath().parent_path() / file.path;
        auto stream = std::ifstream(testFilePath);
        REQUIRE(stream.is_open());
        std::string line;
        Bytes message;
        std::size_t expectedMessageSize = 0;
        int testCount = 0;
        while (std::getline(stream, line)) {
            if (line.starts_with("Len = ")) {
                auto messageSizeInBits = std::stoi(line.substr(6));
                REQUIRE_EQUAL(messageSizeInBits % 8, 0);
                expectedMessageSize = messageSizeInBits / 8;
            } else if (line.starts_with("Msg = ")) {
                std::string_view msgView(line.begin() + 6, line.end());
                if (expectedMessageSize == 0) {
                    // Some test files encode zero length messages as "00".
                    message = {};
                } else {
                    message = Bytes::fromHex(msgView);
                    REQUIRE_EQUAL(expectedMessageSize, message.size());
                }
            } else if (line.starts_with("MD = ")) {
                auto digest = Bytes::fromHex(std::string_view(line.begin() + 5, line.end()));
                WITH_CONTEXT(verifyHash(file.algorithm, message, digest));
                testCount += 1;
            }
        }
        REQUIRE_GREATER(testCount, 50); // Sanity test. Fail if the test files aren't properly read.
    }

    void testSha3NIST() {
        for (const auto &nistFile : nistFiles) {
            runWithContext(SOURCE_LOCATION(), [&, this]() {
                verifyFile(nistFile);
            }, [&, this]() -> std::string {
                return std::format("Failed to verify file {} with algorithm {}", nistFile.path.string(), nistFile.algorithm);
            });
        }
    }
};
