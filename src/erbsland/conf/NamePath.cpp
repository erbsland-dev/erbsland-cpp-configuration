// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "NamePath.hpp"


#include "impl/NameLexer.hpp"


namespace erbsland::conf {


auto NamePath::toText() const noexcept -> String {
    String result;
    bool first = true;
    for (const auto &name : _names) {
        if (!first && !name.isIndex()) {
            result.append('.');
        }
        result.append(name.toPathText());
        first = false;
    }
    return result;
}


auto NamePath::fromText(const String &text) -> NamePath {
    impl::NameLexer lexer(text);
    lexer.initialize();
    NameList names;
    while (lexer.hasNext()) {
        names.push_back(lexer.next());
    }
    return NamePath{names};
}


#ifdef ERBSLAND_CONF_INTERNAL_VIEWS
auto internalView(const NamePath &object) -> impl::InternalViewPtr {
    auto result = impl::InternalView::create();
    result->setValue("names", impl::InternalView::createList(
        0, object._names.begin(), object._names.end()));
    return result;
}
#endif


}

