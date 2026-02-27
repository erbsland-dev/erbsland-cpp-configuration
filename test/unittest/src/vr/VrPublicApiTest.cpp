// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "VrBase.hpp"

#include <erbsland/conf/Parser.hpp>
#include <erbsland/conf/vr/Rules.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;

// A basic test to verify if the public API to use validation rules is working as expected.
TESTED_TARGETS(Rules) TAGS(ValidationRules)
class VrPublicApiTest final : public UNITTEST_SUBCLASS(VrBase) {
public:
    void testBasicUsage() {
        Parser vrParser;
        REQUIRE_NOTHROW(vrDocument = vrParser.parseTextOrThrow(
            "[server.port]\n"
            "type: \"Integer\"\n"
            "minimum: 1024\n"
            "maximum: 0xffff\n"))
        REQUIRE(vrDocument != nullptr);

        REQUIRE_NOTHROW(rules = vr::Rules::createFromDocument(vrDocument));

        WITH_CONTEXT(requirePass(
            "[server]\n"
            "port: 8080\n"))
        WITH_CONTEXT(requirePass(
            "[server]\n"
            "port: 0x89ab\n"))
        WITH_CONTEXT(requireFail(
            "# empty")); // missing section "server"
        WITH_CONTEXT(requireError(u8"expected a section"));
        WITH_CONTEXT(requireFail(
            "[server]\n")); // missing value "port"
        WITH_CONTEXT(requireError(u8"expected an integer value"));
        WITH_CONTEXT(requireFail(
            "[server]\n" // missing value "port" + unknown value "the_port"
            "the_port: 8080\n"));
        WITH_CONTEXT(requireError(u8"unexpected integer value"));
        WITH_CONTEXT(requireFail(
            "[client]\n" // missing section "server" + unknown section "client"
            "port: 8080\n"));
        WITH_CONTEXT(requireError(u8"unexpected section"));
        WITH_CONTEXT(requireFail(
            "[server]\n" // unknown value "ip"
            "port: 8080\n"
            "ip: \"127.0.0.1\"\n"))
        WITH_CONTEXT(requireError(u8"unexpected text value"));
        WITH_CONTEXT(requireFail(
            "[server]\n" // unknown section "client"
            "port: 8080\n"
            "[client]\n"))
        WITH_CONTEXT(requireError(u8"unexpected section"));
        WITH_CONTEXT(requireFail(
            "*[server]\n" // wrong section type.
            "port: 8080\n"
            "*[server]\n"
            "port: 8080\n"))
        WITH_CONTEXT(requireError(u8"Expected a section but got a section list"));
        WITH_CONTEXT(requireFail(
            "[server]\n" // wrong type for "port"
            "port: \"8080\"\n"))
        WITH_CONTEXT(requireError(u8"Expected an integer value but got a text value"));
        WITH_CONTEXT(requireFail(
            "[server]\n" // wrong type for "port"
            "port: Yes\n"))
        WITH_CONTEXT(requireError(u8"Expected an integer value but got a boolean value"));
        WITH_CONTEXT(requireFail(
            "[server]\n" // wrong type for "port"
            "port: 80, 8080\n"))
        WITH_CONTEXT(requireError(u8"Expected an integer value but got a value list"));
    }

    void testSecretFlagIsExposedThroughValidatedValueApi() {
        WITH_CONTEXT(requireRulesPassLines({
            "[credentials.password]",
            "type: \"text\"",
            "is_secret: yes",
            "[credentials.username]",
            "type: \"text\"",
            "is_secret: no",
        }));
        WITH_CONTEXT(requirePassLines({
            "[credentials]",
            "password: \"correct horse battery staple\"",
            "username: \"alice\"",
        }));

        auto password = document->valueOrThrow("credentials.password");
        REQUIRE(password->validationRule() != nullptr);
        REQUIRE(password->validationRule()->isSecret());
        REQUIRE(password->isSecret());

        auto username = document->valueOrThrow("credentials.username");
        REQUIRE(username->validationRule() != nullptr);
        REQUIRE_FALSE(username->validationRule()->isSecret());
        REQUIRE_FALSE(username->isSecret());
    }
};
