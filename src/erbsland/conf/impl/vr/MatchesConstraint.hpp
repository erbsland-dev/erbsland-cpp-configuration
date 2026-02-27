// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Constraint.hpp"
#include "ConstraintHandlerContext.hpp"


#ifdef ERBSLAND_CONF_VR_RE_STD
#include <regex>
#else
#ifdef ERBSLAND_CONF_VR_RE_ERBSLAND
#include <erbsland/re/RegEx.hpp>
#endif
#endif


namespace erbsland::conf::impl {


class MatchesConstraint : public Constraint {
public:
#ifdef ERBSLAND_CONF_VR_RE_STD
    using RegEx = std::regex;
#else
#ifdef ERBSLAND_CONF_VR_RE_ERBSLAND
    using RegEx = erbsland::re::RegExPtr;
#else
    using RegEx = String;
#endif
#endif

public:
    MatchesConstraint(const String &pattern, bool isVerbose);
    ~MatchesConstraint() override = default;

protected:
    void validateText(const ValidationContext &context, const String &value) const override;

private:
    RegEx _regex; ///< The compiled regular expression
};


auto handleMatchesConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr;


}

