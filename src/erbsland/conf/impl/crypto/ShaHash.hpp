// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Sha3.hpp"

#include "../../Bytes.hpp"


namespace erbsland::conf::impl::crypto {


/// SHA3 hash generator.
///
/// Usage: After creating the instance, call `update()` as many times as needed to add data to the hash.
/// When done, call `digest()` to get the digest for the hash.
/// You can reuse the instance by calling `reset()`.
///
/// @tested `ShaHashTest`
///
class ShaHash final { // test-required
public:
    /// The algorithm for the hash function to use.
    ///
    enum class Algorithm : uint8_t {
        Sha3_256,
        Sha3_384,
        Sha3_512,
    };

    /// The implementation used for the calculation.
    ///
    using Impl = std::variant<Sha3_256, Sha3_384, Sha3_512>;

public:
    /// Create a new instance of the hash generator.
    ///
    /// @param algorithm The algorithm to use for the hash.
    ///
    explicit ShaHash(const Algorithm algorithm) : _algorithm{algorithm} {
        switch (algorithm) {
            case Algorithm::Sha3_256:
                _impl = Sha3_256{};
                break;
            case Algorithm::Sha3_384:
                _impl = Sha3_384{};
                break;
            case Algorithm::Sha3_512:
                _impl = Sha3_512{};
                break;
            default:
                throw std::runtime_error("Invalid algorithm.");
        }
    }

    // defaults
    ~ShaHash() = default;

    // disable copy and assign.
    ShaHash(const ShaHash &) = delete;
    auto operator=(const ShaHash &) -> ShaHash& = delete;

public:
    /// Get the algorithm of this hash function.
    ///
    [[nodiscard]] auto algorithm() const noexcept -> Algorithm {
        return _algorithm;
    }

    /// Reset the hash generator to start a new calculation.
    ///
    void reset() noexcept {
        std::visit([&](auto& impl) {
            impl.reset();
        }, _impl);
    }

    /// Update the current hash with new data.
    ///
    /// @param data The data to add to the hash.
    ///
    void update(const std::span<const std::byte> data) noexcept {
        std::visit([&](auto& impl) {
            impl.update(data);
        }, _impl);
    }

    /// Finalize the hash and return the digest.
    ///
    /// @return The digest for the data.
    ///
    auto digest() noexcept -> Bytes {
        std::vector<std::byte> digest;
        std::visit([&](auto& impl) {
            const auto result = impl.digest();
            digest.reserve(result.size());
            digest.insert(digest.end(), result.begin(), result.end());
        }, _impl);
        return Bytes{digest};
    }

    /// Convert the algorithm name into text.
    ///
    /// @return A text with the algorithm name.
    ///
    static auto algorithmToText(Algorithm algorithm) noexcept -> String {
        switch (algorithm) {
        case Algorithm::Sha3_256:
            return String{u8"sha3-256"};
        case Algorithm::Sha3_384:
            return String{u8"sha3-384"};
        case Algorithm::Sha3_512:
            return String{u8"sha3-512"};
        default:
            return {};
        }
    }

private:
    Algorithm _algorithm; ///< The algorithm to use for the hash.
    Impl _impl; ///< The hash implementation.
};


}


template <>
struct std::formatter<erbsland::conf::impl::crypto::ShaHash::Algorithm> : std::formatter<std::string> {
    using ShaHash = erbsland::conf::impl::crypto::ShaHash;
    auto format(const ShaHash::Algorithm algorithm, format_context& ctx) const {
        return std::formatter<std::string>::format(ShaHash::algorithmToText(algorithm).toCharString(), ctx);
    }
};



