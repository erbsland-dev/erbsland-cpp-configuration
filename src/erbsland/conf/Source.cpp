// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Source.hpp"


#include "impl/source/FileSource.hpp"
#include "impl/source/StringSource.hpp"


namespace erbsland::conf {


auto Source::fromFile(const String &path) noexcept -> SourcePtr {
    return std::make_shared<impl::FileSource>(std::filesystem::path{path.toCharString()});
}


auto Source::fromFile(const std::filesystem::path &path) noexcept -> SourcePtr {
    return std::make_shared<impl::FileSource>(path);
}


auto Source::fromString(const String &text) noexcept -> SourcePtr {
    return std::make_shared<impl::StringSource>(text);
}


auto Source::fromString(std::string &&text) noexcept -> SourcePtr {
    return std::make_shared<impl::StringSource>(std::move(text));
}


auto Source::fromString(const std::string &text) noexcept -> SourcePtr {
    return std::make_shared<impl::StringSource>(text);
}


#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
auto internalView(const Source &object) -> impl::InternalViewPtr {
    auto view = impl::InternalView::create();
    if (object.identifier()) {
        view->setValue(u8"identifier", internalView(*object.identifier()));
    } else {
        view->setValue(u8"identifier", u8"<none>");
    }
    return view;
}
#endif


}

