#pragma once

#include <erbsland/conf/AccessCheck.hpp>
#include <erbsland/conf/Error.hpp>

#include <filesystem>
#include <memory>

using el::conf::AccessSources;
using el::conf::AccessCheckResult;
using el::conf::Error;
using el::conf::ErrorCategory;

class MyAccessCheck final : public el::conf::AccessCheck {
public:
    explicit MyAccessCheck(std::filesystem::path sandbox) : _sandbox{std::move(sandbox)} {};
    ~MyAccessCheck() override = default;
    static auto create(std::filesystem::path sandbox) -> std::shared_ptr<MyAccessCheck> {
        return std::make_shared<MyAccessCheck>(std::move(sandbox));
    }

public:
    auto check(const AccessSources &sources) -> AccessCheckResult override {
        try {
            if (sources.source == nullptr || sources.source->name() != u8"text") {}
            auto sourcePath = std::filesystem::path{sources.source->path().toCharString()};
            auto canonicalSource = canonical(sourcePath).string();
            auto canonicalSandbox = canonical(_sandbox).string();
            if (!canonicalSource.starts_with(canonicalSandbox)) {
                return AccessCheckResult::Denied;
            }
            return AccessCheckResult::Granted;
        } catch (const std::system_error &error) {
            throw Error(ErrorCategory::Access, u8"System error", error.code());
        }
    }

private:
    std::filesystem::path _sandbox;
};
