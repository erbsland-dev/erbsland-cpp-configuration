#pragma once

#include <erbsland/conf/SourceResolver.hpp>
#include <erbsland/conf/FileSourceResolver.hpp>

using el::conf::SourceResolver;
using el::conf::SourceResolverPtr;
using el::conf::SourceResolverContext;
using el::conf::SourceList;
using el::conf::SourceListPtr;
using el::conf::FileSourceResolver;

class MySourceResolver final : public SourceResolver {
public:
    MySourceResolver() : _fallbackResolver(FileSourceResolver::create()) {}
    [[nodiscard]] static auto create() -> SourceResolverPtr {
        return std::make_shared<MySourceResolver>();
    }

public:
    auto resolve(const SourceResolverContext &context) -> SourceListPtr override {
        if (!context.includeText.starts_with(u8"my:")) {
            return _fallbackResolver->resolve(context);
        }
        auto result = std::make_shared<SourceList>();
        // ...add my sources...
        return result;
    }

private:
    SourceResolverPtr _fallbackResolver;
};



