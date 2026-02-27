// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "VrBase.hpp"

#include <erbsland/conf/Parser.hpp>
#include <erbsland/conf/vr/RulesBuilder.hpp>


using namespace el::conf;
using namespace el::conf::vr::builder;
using vr::RulesBuilder;
using vr::RuleType;


// Testing manual rule construction.
TESTED_TARGETS(RulesBuilder RuleDefinition) TAGS(ValidationRules)
class VrManualConstructionTest final : public UNITTEST_SUBCLASS(VrBase) {
public:
    void testBasicConstruction() {
        RulesBuilder rulesBuilder;
        rulesBuilder.addRule("app",
            RuleType::Section);
        rulesBuilder.addRule("app.x",
            RuleType::Integer,
            Title("X"),
            Description("This is the value x"),
            Minimum(1),
            Maximum(100)
        );
        rules = rulesBuilder.takeRules();
        REQUIRE(rules != nullptr);
        WITH_CONTEXT(requirePassLines({
            "[app]",
            "x = 10",
        }));
        auto xValue = document->value("app.x");
        REQUIRE(xValue != nullptr);
        REQUIRE(xValue->validationRule() != nullptr);
        REQUIRE(xValue->validationRule()->type() == RuleType::Integer);
        REQUIRE_EQUAL(xValue->validationRule()->title(), "X");
        REQUIRE_EQUAL(xValue->validationRule()->description(), "This is the value x");
    }

    void testAdvancedConstruction() {
        RulesBuilder rulesBuilder;
        rulesBuilder.addRule("app",
            RuleType::Section,
            KeyIndex(String{u8"user_id"}, NamePathLike{String{u8"users.vr_entry.id"}}));
        rulesBuilder.addRule("app.server",
            RuleType::Section,
            IsOptional(),
            Dependency(impl::DependencyMode::XOR, {"hostname"}, {"ip_address"}));
        rulesBuilder.addRule("app.server.hostname", RuleType::Text, IsOptional());
        rulesBuilder.addRule("app.server.ip_address", RuleType::Text, IsOptional());

        rulesBuilder.addRule("app.users", RuleType::SectionList);
        rulesBuilder.addRule("app.users.vr_entry", RuleType::Section);
        rulesBuilder.addRule("app.users.vr_entry.id", RuleType::Integer);
        rulesBuilder.addRule("app.users.vr_entry.name",
            RuleType::Text,
            Starts("u"));

        rulesBuilder.addRule("app.start_user_id",
            RuleType::Integer,
            Key("user_id"));

        rulesBuilder.addRule("app.mode",
            RuleType::Text,
            Default("dev"),
            In({"dev", "prod"}),
            Version({1, 2, 3}),
            MinimumVersion(1),
            MaximumVersion(10));

        rules = rulesBuilder.takeRules();
        REQUIRE(rules != nullptr);

        WITH_CONTEXT(requirePassLines({
            "[app]",
            "start_user_id: 1",
            "mode: \"prod\"",
            "[app.server]",
            "hostname: \"example.local\"",
            "*[app.users]*",
            "id: 1",
            "name: \"user-1\"",
        }, 2));

        WITH_CONTEXT(requireFailLines({
            "[app]",
            "start_user_id: 99",
            "mode: \"prod\"",
            "*[app.users]*",
            "id: 1",
            "name: \"user-1\"",
        }, 2));
        WITH_CONTEXT(requireError("must refer to an existing key"));

        WITH_CONTEXT(requireFailLines({
            "[app]",
            "start_user_id: 1",
            "[app.server]",
            "hostname: \"example.local\"",
            "ip_address: \"127.0.0.1\"",
            "*[app.users]*",
            "id: 1",
            "name: \"user-1\"",
        }, 2));
        WITH_CONTEXT(requireError("either configure 'hostname' or configure 'ip_address'"));
    }
};
