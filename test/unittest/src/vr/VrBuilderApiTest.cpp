// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "VrBase.hpp"

#include <erbsland/conf/impl/vr/DependencyMode.hpp>
#include <erbsland/conf/impl/vr/EqualsConstraint.hpp>
#include <erbsland/conf/impl/vr/InConstraint.hpp>
#include <erbsland/conf/impl/vr/KeyConstraint.hpp>
#include <erbsland/conf/impl/vr/MinMaxConstraint.hpp>
#include <erbsland/conf/impl/vr/MultipleConstraint.hpp>
#include <erbsland/conf/impl/vr/Rule.hpp>
#include <erbsland/conf/impl/vr/Rules.hpp>
#include <erbsland/conf/vr/RuleType.hpp>
#include <erbsland/conf/vr/RulesBuilder.hpp>


using namespace el::conf;


TESTED_TARGETS(RulesBuilder RuleDefinition) TAGS(ValidationRules)
class VrBuilderApiTest final : public UNITTEST_SUBCLASS(VrBase) {
public:
    [[nodiscard]] static auto makeRule(const vr::RuleType type = vr::RuleType::Text) -> impl::Rule {
        auto rule = impl::Rule{};
        rule.setRuleNamePath(NamePath::fromText(u8"app.value"));
        rule.setTargetNamePath(NamePath::fromText(u8"app.value"));
        rule.setType(type);
        return rule;
    }

    [[nodiscard]] auto takeRulesImpl(vr::RulesBuilder &builder) -> std::shared_ptr<impl::Rules> {
        auto rules = builder.takeRules();
        REQUIRE(rules != nullptr);
        auto rulesImpl = std::dynamic_pointer_cast<impl::Rules>(rules);
        REQUIRE(rulesImpl != nullptr);
        return rulesImpl;
    }

    void testAddRuleSupportsAllNamePathLikeFormsAndErrors() {
        vr::RulesBuilder builder;

        REQUIRE_NOTHROW(builder.addRule(NamePathLike{String{u8"app"}}, vr::RuleType::Section));
        REQUIRE_NOTHROW(builder.addRule(
            NamePathLike{NamePath::fromText(u8"app.int_value")},
            vr::RuleType::Integer));
        REQUIRE_NOTHROW(builder.addRule(NamePathLike{Name::createRegular(u8"root_text")}, vr::RuleType::Text));
        REQUIRE_NOTHROW(builder.takeRules());

        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addRule(NamePathLike{std::size_t{2}}, vr::RuleType::Integer));
        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addRule(NamePathLike{NamePath{}}, vr::RuleType::Integer));
        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addRule(NamePathLike{String{u8"app[0]"}}, vr::RuleType::Integer));
        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addRule(NamePathLike{String{u8"app.\"key\""}}, vr::RuleType::Integer));
        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addRule(NamePathLike{String{u8"unknown.parent.child"}}, vr::RuleType::Integer));
    }

    void testAddAlternativeBranches() {
        vr::RulesBuilder builder;

        REQUIRE_NOTHROW(builder.addRule(u8"app", vr::RuleType::Section));
        REQUIRE_NOTHROW(builder.addAlternative(u8"app.variant", vr::RuleType::Integer));
        REQUIRE_NOTHROW(builder.addAlternative(u8"app.variant", vr::RuleType::Text));

        auto rulesImpl = takeRulesImpl(builder);
        const auto alternativeRule = rulesImpl->ruleForNamePath(NamePath::fromText(u8"app.variant"));
        REQUIRE(alternativeRule != nullptr);
        REQUIRE(alternativeRule->type() == vr::RuleType::Alternatives);
        REQUIRE_EQUAL(alternativeRule->childrenImpl().size(), 2U);

        const auto firstAlternative = alternativeRule->childrenImpl().rule(Name::createIndex(0));
        const auto secondAlternative = alternativeRule->childrenImpl().rule(Name::createIndex(1));
        REQUIRE(firstAlternative != nullptr);
        REQUIRE(secondAlternative != nullptr);
        REQUIRE(firstAlternative->type() == vr::RuleType::Integer);
        REQUIRE(secondAlternative->type() == vr::RuleType::Text);

        builder.reset();
        REQUIRE_NOTHROW(builder.addRule(u8"app", vr::RuleType::Section));
        REQUIRE_NOTHROW(builder.addRule(u8"app.variant", vr::RuleType::Integer));
        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addAlternative(u8"app.variant", vr::RuleType::Text));
        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addAlternative(u8"missing.parent.variant", vr::RuleType::Text));
    }

    void testAddMethodsValidateRuleTypeAndConstraintAlignment() {
        vr::RulesBuilder builder;

        REQUIRE_THROWS_AS(el::conf::Error, builder.addRule(u8"app", vr::RuleType::Undefined));
        REQUIRE_NOTHROW(builder.addRule(u8"app", vr::RuleType::Section));

        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addRule(u8"app.port", vr::RuleType::Integer, vr::builder::Default("text")));
        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addRule(u8"app.port", vr::RuleType::Integer, vr::builder::Minimum(Date{2026, 1, 1})));
        REQUIRE_THROWS_AS(
            el::conf::Error,
            builder.addAlternative(u8"app.kind", vr::RuleType::Text, vr::builder::In(std::vector<Integer>{1, 2})));

        REQUIRE_NOTHROW(builder.addRule(u8"app.port", vr::RuleType::Integer, vr::builder::Default(Integer{443})));
        REQUIRE_NOTHROW(builder.addAlternative(u8"app.kind", vr::RuleType::Text, vr::builder::In({"dev", "prod"})));
    }

    void testSimpleAttributes() {
        auto rule = makeRule();
        vr::builder::Title(u8"My Title")(rule);
        REQUIRE_EQUAL(rule.title(), u8"My Title");

        vr::builder::Description(u8"Description")(rule);
        REQUIRE_EQUAL(rule.description(), u8"Description");

        vr::builder::CustomError(u8"Rule error")(rule);
        REQUIRE(rule.hasCustomError());
        REQUIRE_EQUAL(rule.customError(), u8"Rule error");

        vr::builder::IsOptional()(rule);
        REQUIRE(rule.isOptional());
        vr::builder::IsOptional(false)(rule);
        REQUIRE_FALSE(rule.isOptional());

        vr::builder::IsSecret()(rule);
        REQUIRE(rule.isSecret());
        vr::builder::IsSecret(false)(rule);
        REQUIRE_FALSE(rule.isSecret());

        auto defaultCaseSensitive = vr::builder::CaseSensitive{};
        defaultCaseSensitive(rule);
        REQUIRE(rule.caseSensitivity() == CaseSensitivity::CaseSensitive);
        auto caseInsensitive = vr::builder::CaseSensitive{CaseSensitivity::CaseInsensitive};
        caseInsensitive(rule);
        REQUIRE(rule.caseSensitivity() == CaseSensitivity::CaseInsensitive);
    }

    void testDefaultAttributeConstructors() {
        auto defaultFromValue = vr::builder::Default(impl::Value::createInteger(7));
        REQUIRE(defaultFromValue._value != nullptr);
        REQUIRE(defaultFromValue._value->type() == ValueType::Integer);

        REQUIRE(vr::builder::Default(Integer{7})._value->type() == ValueType::Integer);
        REQUIRE(vr::builder::Default(true)._value->type() == ValueType::Boolean);
        REQUIRE(vr::builder::Default(Float{1.5})._value->type() == ValueType::Float);
        REQUIRE(vr::builder::Default(String{u8"text"})._value->type() == ValueType::Text);
        REQUIRE(vr::builder::Default("text")._value->type() == ValueType::Text);
        REQUIRE(vr::builder::Default(std::string{"text"})._value->type() == ValueType::Text);
        REQUIRE(vr::builder::Default(Date{2026, 1, 1})._value->type() == ValueType::Date);
        REQUIRE(vr::builder::Default(Time{12, 0, 0, 0, TimeOffset{}})._value->type() == ValueType::Time);
        REQUIRE(vr::builder::Default(DateTime{Date{2026, 1, 1}, Time{12, 0, 0, 0, TimeOffset{}}})._value->type() == ValueType::DateTime);
        REQUIRE(vr::builder::Default(Bytes::fromHex("DE AD"))._value->type() == ValueType::Bytes);
        REQUIRE(vr::builder::Default(TimeDelta{TimeUnit::Hours, 2})._value->type() == ValueType::TimeDelta);
        REQUIRE(vr::builder::Default(RegEx{u8"a.*", false})._value->type() == ValueType::RegEx);

        REQUIRE(vr::builder::Default(std::vector<Integer>{1, 2})._value->type() == ValueType::ValueList);
        REQUIRE(vr::builder::Default(std::vector<bool>{true, false})._value->type() == ValueType::ValueList);
        REQUIRE(vr::builder::Default(std::vector<Float>{1.0, 2.0})._value->type() == ValueType::ValueList);
        REQUIRE(vr::builder::Default(std::vector<String>{u8"a", u8"b"})._value->type() == ValueType::ValueList);
        REQUIRE(vr::builder::Default(
            std::vector<Bytes>{Bytes::fromHex("AA"), Bytes::fromHex("BB")})._value->type() == ValueType::ValueList);
        REQUIRE(vr::builder::Default(std::vector<std::vector<Integer>>{{1, 2}, {3, 4}})._value->type() == ValueType::ValueList);
        REQUIRE(vr::builder::Default(std::vector<std::vector<Float>>{{1.0, 2.0}})._value->type() == ValueType::ValueList);

        auto rule = makeRule(vr::RuleType::Integer);
        vr::builder::Default(Integer{42})(rule);
        REQUIRE(rule.hasDefault());
        REQUIRE(rule.defaultValue() != nullptr);
        REQUIRE(rule.defaultValue()->type() == ValueType::Integer);
    }

    void testKeyIndexConstructors() {
        auto rule = makeRule(vr::RuleType::Section);

        vr::builder::KeyIndex(std::vector<NamePathLike>{String{u8"users.vr_entry.id"}})(rule);
        vr::builder::KeyIndex(Name::createRegular(u8"ids"), std::vector<NamePathLike>{String{u8"users.vr_entry.id"}})(rule);
        vr::builder::KeyIndex(String{u8"names"}, std::vector<NamePathLike>{String{u8"users.vr_entry.name"}})(rule);
        vr::builder::KeyIndex(NamePathLike{String{u8"users.vr_entry.id"}})(rule);
        vr::builder::KeyIndex(Name::createRegular(u8"one"), NamePathLike{String{u8"users.vr_entry.id"}}, CaseSensitivity::CaseSensitive)(rule);
        vr::builder::KeyIndex(String{u8"two"}, NamePathLike{String{u8"users.vr_entry.id"}}, CaseSensitivity::CaseInsensitive)(rule);
        vr::builder::KeyIndex({String{u8"users.vr_entry.id"}, String{u8"users.vr_entry.name"}})(rule);
        vr::builder::KeyIndex(Name::createRegular(u8"three"), {String{u8"users.vr_entry.id"}})(rule);
        vr::builder::KeyIndex(String{u8"four"}, {String{u8"users.vr_entry.id"}}, CaseSensitivity::CaseSensitive)(rule);

        REQUIRE(rule.hasKeyDefinitions());
        REQUIRE_EQUAL(rule.keyDefinitions().size(), 9U);
        REQUIRE(rule.keyDefinitions().back()->caseSensitivity() == CaseSensitivity::CaseSensitive);
    }

    void testDependencyConstructors() {
        auto rule = makeRule(vr::RuleType::Section);

        vr::builder::Dependency(
            impl::DependencyMode::If,
            std::vector<NamePathLike>{String{u8"a"}},
            std::vector<NamePathLike>{String{u8"b"}},
            u8"dep")(rule);
        vr::builder::Dependency(
            impl::DependencyMode::XOR,
            {String{u8"x"}},
            {String{u8"y"}})(rule);

        REQUIRE(rule.hasDependencyDefinitions());
        REQUIRE_EQUAL(rule.dependencyDefinitions().size(), 2U);
        REQUIRE(rule.dependencyDefinitions().front()->mode() == impl::DependencyMode::If);
        REQUIRE(rule.dependencyDefinitions().front()->hasErrorMessage());
        REQUIRE(rule.dependencyDefinitions().back()->mode() == impl::DependencyMode::XOR);
    }

    void testVersionAttributesAndBranches() {
        auto rule = makeRule(vr::RuleType::Integer);

        vr::builder::Version(std::vector<Integer>{1, 1, 3})(rule);
        REQUIRE(rule.versionMask().matches(1));
        REQUIRE(rule.versionMask().matches(3));

        vr::builder::Version(Integer{9}, true)(rule);
        REQUIRE_FALSE(rule.versionMask().matches(9));

        auto initializerRule = makeRule(vr::RuleType::Integer);
        vr::builder::Version({2, 4})(initializerRule);
        REQUIRE_FALSE(initializerRule.versionMask().matches(1));
        REQUIRE(initializerRule.versionMask().matches(2));
        REQUIRE(initializerRule.versionMask().matches(4));

        vr::builder::MinimumVersion(2)(rule);
        REQUIRE(rule.versionMask().matches(3));
        REQUIRE_FALSE(rule.versionMask().matches(1));

        vr::builder::MaximumVersion(10)(rule);
        REQUIRE(rule.versionMask().matches(3));
        REQUIRE_FALSE(rule.versionMask().matches(50));

        auto negatedRule = makeRule(vr::RuleType::Integer);
        vr::builder::MinimumVersion(5, true)(negatedRule);
        REQUIRE(negatedRule.versionMask().matches(2));
        REQUIRE_FALSE(negatedRule.versionMask().matches(5));

        auto negatedMaxRule = makeRule(vr::RuleType::Integer);
        vr::builder::MaximumVersion(7, true)(negatedMaxRule);
        REQUIRE_FALSE(negatedMaxRule.versionMask().matches(6));
        REQUIRE(negatedMaxRule.versionMask().matches(9));

        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Version(std::vector<Integer>{})(rule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Version(std::vector<Integer>{1, -1})(rule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::MinimumVersion(-1)(rule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::MaximumVersion(-1)(rule));
    }

    void testCharsConstraintConstructorsAndOptions() {
        auto rule = makeRule(vr::RuleType::Text);

        vr::builder::Chars(std::vector<String>{u8"[ab]"})(rule);
        vr::builder::Chars(String{u8"[cd]"})(rule);
        vr::builder::Chars("[ef]")(rule);
        vr::builder::Chars(std::string{"[gh]"})(rule);
        vr::builder::Chars({String{u8"[ij]"}, String{u8"[kl]"}})(rule);
        vr::builder::Chars({"[mn]", "[op]"}, {.isNegated = true, .errorMessage = u8"chars error"})(rule);

        const auto constraint = rule.constraint(u8"not_chars");
        REQUIRE(constraint != nullptr);
        REQUIRE(constraint->type() == vr::ConstraintType::Chars);
        REQUIRE(constraint->isNegated());
        REQUIRE(constraint->hasCustomError());

        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Chars(std::vector<String>{})(rule));

        auto integerRule = makeRule(vr::RuleType::Integer);
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Chars("[ab]")(integerRule));
    }

    void testStringPartConstraintConstructorsAndOptions() {
        auto rule = makeRule(vr::RuleType::Text);

        vr::builder::Starts(std::vector<String>{u8"a"})(rule);
        vr::builder::Starts(String{u8"b"})(rule);
        vr::builder::Starts("c")(rule);
        vr::builder::Starts(std::string{"d"})(rule);
        vr::builder::Starts({String{u8"e"}})(rule);
        vr::builder::Starts({"f", "g"}, {.isNegated = true, .errorMessage = u8"starts error"})(rule);

        vr::builder::Ends(std::vector<String>{u8"a"})(rule);
        vr::builder::Ends(String{u8"b"})(rule);
        vr::builder::Ends("c")(rule);
        vr::builder::Ends(std::string{"d"})(rule);
        vr::builder::Ends({String{u8"e"}})(rule);
        vr::builder::Ends({"f", "g"}, {.isNegated = true, .errorMessage = u8"ends error"})(rule);

        vr::builder::Contains(std::vector<String>{u8"a"})(rule);
        vr::builder::Contains(String{u8"b"})(rule);
        vr::builder::Contains("c")(rule);
        vr::builder::Contains(std::string{"d"})(rule);
        vr::builder::Contains({String{u8"e"}})(rule);
        vr::builder::Contains({"f", "g"}, {.isNegated = true, .errorMessage = u8"contains error"})(rule);

        REQUIRE(rule.constraint(u8"not_starts") != nullptr);
        REQUIRE(rule.constraint(u8"not_ends") != nullptr);
        REQUIRE(rule.constraint(u8"not_contains") != nullptr);

        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Starts(std::vector<String>{})(rule));

        auto integerRule = makeRule(vr::RuleType::Integer);
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Ends("x")(integerRule));
    }

    void testEqualsConstraintConstructorsAndBranches() {
        auto textRule = makeRule(vr::RuleType::Text);
        vr::builder::Equals(Integer{5})(textRule);
        REQUIRE(std::dynamic_pointer_cast<impl::EqualsIntegerConstraint>(textRule.constraint(u8"equals")) != nullptr);
        vr::builder::Equals(String{u8"abc"})(textRule);
        REQUIRE(std::dynamic_pointer_cast<impl::EqualsTextConstraint>(textRule.constraint(u8"equals")) != nullptr);
        vr::builder::Equals("def")(textRule);
        REQUIRE(std::dynamic_pointer_cast<impl::EqualsTextConstraint>(textRule.constraint(u8"equals")) != nullptr);
        vr::builder::Equals(std::string{"ghi"})(textRule);
        REQUIRE(std::dynamic_pointer_cast<impl::EqualsTextConstraint>(textRule.constraint(u8"equals")) != nullptr);

        auto booleanRule = makeRule(vr::RuleType::Boolean);
        vr::builder::Equals(true)(booleanRule);
        REQUIRE(std::dynamic_pointer_cast<impl::EqualsBooleanConstraint>(booleanRule.constraint(u8"equals")) != nullptr);

        auto floatRule = makeRule(vr::RuleType::Float);
        vr::builder::Equals(Float{1.25})(floatRule);
        REQUIRE(std::dynamic_pointer_cast<impl::EqualsFloatConstraint>(floatRule.constraint(u8"equals")) != nullptr);

        auto bytesRule = makeRule(vr::RuleType::Bytes);
        vr::builder::Equals(Bytes::fromHex("AA"))(bytesRule);
        REQUIRE(std::dynamic_pointer_cast<impl::EqualsBytesConstraint>(bytesRule.constraint(u8"equals")) != nullptr);
        vr::builder::Equals(Integer{7})(bytesRule);
        REQUIRE(std::dynamic_pointer_cast<impl::EqualsIntegerConstraint>(bytesRule.constraint(u8"equals")) != nullptr);

        auto matrixRule = makeRule(vr::RuleType::ValueMatrix);
        vr::builder::Equals(std::pair<Integer, Integer>{2, 3})(matrixRule);
        REQUIRE(std::dynamic_pointer_cast<impl::EqualsMatrixConstraint>(matrixRule.constraint(u8"equals")) != nullptr);

        vr::builder::Equals(Integer{4}, Integer{5}, {.isNegated = true, .errorMessage = u8"eq error"})(matrixRule);
        const auto negated = matrixRule.constraint(u8"not_equals");
        REQUIRE(negated != nullptr);
        REQUIRE(negated->isNegated());
        REQUIRE(negated->hasCustomError());

        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Equals(true)(textRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Equals(Float{1.0})(textRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Equals(Bytes::fromHex("AA"))(textRule));
    }

    void testInConstraintConstructorsAndBranches() {
        auto integerRule = makeRule(vr::RuleType::Integer);
        vr::builder::In(std::vector<Integer>{1, 2})(integerRule);
        REQUIRE(std::dynamic_pointer_cast<impl::InIntegerConstraint>(integerRule.constraint(u8"in")) != nullptr);
        vr::builder::In(std::initializer_list<Integer>{1, 2})(integerRule);
        vr::builder::In(Integer{5})(integerRule);

        auto floatRule = makeRule(vr::RuleType::Float);
        vr::builder::In(std::vector<Float>{1.0, 2.0})(floatRule);
        REQUIRE(std::dynamic_pointer_cast<impl::InFloatConstraint>(floatRule.constraint(u8"in")) != nullptr);
        vr::builder::In({1.0, 2.0})(floatRule);
        vr::builder::In(Float{9.0})(floatRule);

        auto textRule = makeRule(vr::RuleType::Text);
        vr::builder::In(std::vector<String>{u8"a", u8"b"})(textRule);
        REQUIRE(std::dynamic_pointer_cast<impl::InTextConstraint>(textRule.constraint(u8"in")) != nullptr);
        vr::builder::In({String{u8"x"}, String{u8"y"}})(textRule);
        vr::builder::In({"a", "b"})(textRule);
        vr::builder::In(String{u8"x"})(textRule);
        vr::builder::In("x")(textRule);

        auto bytesRule = makeRule(vr::RuleType::Bytes);
        vr::builder::In(std::vector<Bytes>{Bytes::fromHex("AA")})(bytesRule);
        REQUIRE(std::dynamic_pointer_cast<impl::InBytesConstraint>(bytesRule.constraint(u8"in")) != nullptr);
        vr::builder::In({Bytes::fromHex("01"), Bytes::fromHex("02")})(bytesRule);
        vr::builder::In(Bytes::fromHex("FF"), {.isNegated = true, .errorMessage = u8"in error"})(bytesRule);

        const auto negated = bytesRule.constraint(u8"not_in");
        REQUIRE(negated != nullptr);
        REQUIRE(negated->isNegated());
        REQUIRE(negated->hasCustomError());

        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::In(std::vector<Integer>{})(integerRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::In(std::vector<Integer>{1, 1})(integerRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::In(std::vector<Integer>{1, 2})(textRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::In(std::vector<String>{u8"A", u8"a"})(textRule));
    }

    void testKeyConstraintConstructorsAndOptions() {
        auto rule = makeRule(vr::RuleType::Integer);

        vr::builder::Key(NamePathLike{String{u8"ids"}})(rule);
        REQUIRE(std::dynamic_pointer_cast<impl::KeyConstraint>(rule.constraint(u8"key")) != nullptr);

        vr::builder::Key(std::vector<NamePathLike>{String{u8"ids"}, String{u8"other"}})(rule);
        REQUIRE(std::dynamic_pointer_cast<impl::KeyConstraint>(rule.constraint(u8"key")) != nullptr);

        vr::builder::Key({String{u8"ids"}}, {.isNegated = true, .errorMessage = u8"key error"})(rule);
        const auto negated = rule.constraint(u8"not_key");
        REQUIRE(negated != nullptr);
        REQUIRE(negated->isNegated());
        REQUIRE(negated->hasCustomError());

        auto floatRule = makeRule(vr::RuleType::Float);
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Key(NamePathLike{String{u8"ids"}})(floatRule));
    }

    void testMatchesConstraintConstructorsAndOptions() {
        auto rule = makeRule(vr::RuleType::Text);

        vr::builder::Matches(String{u8"^[a-z]+$"})(rule);
        REQUIRE(rule.constraint(u8"matches") != nullptr);

        vr::builder::Matches("^[0-9]+$", true)(rule);
        REQUIRE(rule.constraint(u8"matches") != nullptr);

        vr::builder::Matches(std::string{"^x+$"})(rule);
        REQUIRE(rule.constraint(u8"matches") != nullptr);

        vr::builder::Matches(RegEx{u8"^a+$", true}, {.isNegated = true, .errorMessage = u8"match error"})(rule);
        const auto negated = rule.constraint(u8"not_matches");
        REQUIRE(negated != nullptr);
        REQUIRE(negated->isNegated());
        REQUIRE(negated->hasCustomError());

        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Matches(String{})(rule));

        auto integerRule = makeRule(vr::RuleType::Integer);
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Matches("^[0-9]+$")(integerRule));
    }

    void testMinimumAndMaximumConstraintConstructorsAndBranches() {
        auto integerRule = makeRule(vr::RuleType::Integer);
        vr::builder::Minimum(Integer{1})(integerRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxIntegerConstraint>(integerRule.constraint(u8"minimum")) != nullptr);
        vr::builder::Maximum(Integer{1})(integerRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxIntegerConstraint>(integerRule.constraint(u8"maximum")) != nullptr);

        auto floatRule = makeRule(vr::RuleType::Float);
        vr::builder::Minimum(Float{1.5})(floatRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxFloatConstraint>(floatRule.constraint(u8"minimum")) != nullptr);
        vr::builder::Maximum(Float{1.5})(floatRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxFloatConstraint>(floatRule.constraint(u8"maximum")) != nullptr);

        auto dateRule = makeRule(vr::RuleType::Date);
        vr::builder::Minimum(Date{2026, 1, 1})(dateRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxDateConstraint>(dateRule.constraint(u8"minimum")) != nullptr);
        vr::builder::Maximum(Date{2026, 1, 1})(dateRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxDateConstraint>(dateRule.constraint(u8"maximum")) != nullptr);

        auto dateTimeRule = makeRule(vr::RuleType::DateTime);
        vr::builder::Minimum(DateTime{Date{2026, 1, 1}, Time{12, 0, 0, 0, TimeOffset{}}})(dateTimeRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxDateTimeConstraint>(dateTimeRule.constraint(u8"minimum")) != nullptr);
        vr::builder::Maximum(DateTime{Date{2026, 1, 1}, Time{12, 0, 0, 0, TimeOffset{}}})(dateTimeRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxDateTimeConstraint>(dateTimeRule.constraint(u8"maximum")) != nullptr);

        auto matrixRule = makeRule(vr::RuleType::ValueMatrix);
        vr::builder::Minimum(std::pair<Integer, Integer>{2, 3})(matrixRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxMatrixConstraint>(matrixRule.constraint(u8"minimum")) != nullptr);
        vr::builder::Maximum(std::pair<Integer, Integer>{2, 3})(matrixRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MinMaxMatrixConstraint>(matrixRule.constraint(u8"maximum")) != nullptr);

        vr::builder::Minimum(Integer{2}, Integer{4}, {.isNegated = true, .errorMessage = u8"min error"})(matrixRule);
        const auto negMin = matrixRule.constraint(u8"not_minimum");
        REQUIRE(negMin != nullptr);
        REQUIRE(negMin->isNegated());
        REQUIRE(negMin->hasCustomError());

        vr::builder::Maximum(Integer{3}, Integer{5}, {.isNegated = true, .errorMessage = u8"max error"})(matrixRule);
        const auto negMax = matrixRule.constraint(u8"not_maximum");
        REQUIRE(negMax != nullptr);
        REQUIRE(negMax->isNegated());
        REQUIRE(negMax->hasCustomError());

        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Minimum(Float{1.5})(integerRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Minimum(Date{2026, 1, 1})(integerRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Maximum(Integer{1})(matrixRule));
    }

    void testMultipleConstraintConstructorsAndBranches() {
        auto integerRule = makeRule(vr::RuleType::Integer);
        vr::builder::Multiple(Integer{2})(integerRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MultipleIntegerConstraint>(integerRule.constraint(u8"multiple")) != nullptr);

        auto floatRule = makeRule(vr::RuleType::Float);
        vr::builder::Multiple(Float{0.5})(floatRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MultipleFloatConstraint>(floatRule.constraint(u8"multiple")) != nullptr);

        auto matrixRule = makeRule(vr::RuleType::ValueMatrix);
        vr::builder::Multiple(std::pair<Integer, Integer>{2, 3})(matrixRule);
        REQUIRE(std::dynamic_pointer_cast<impl::MultipleMatrixConstraint>(matrixRule.constraint(u8"multiple")) != nullptr);

        vr::builder::Multiple(Integer{4}, Integer{5}, {.isNegated = true, .errorMessage = u8"mul error"})(matrixRule);
        const auto negated = matrixRule.constraint(u8"not_multiple");
        REQUIRE(negated != nullptr);
        REQUIRE(negated->isNegated());
        REQUIRE(negated->hasCustomError());

        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Multiple(Integer{0})(integerRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Multiple(Float{0.0})(floatRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Multiple(Integer{1}, Integer{0})(matrixRule));
        REQUIRE_THROWS_AS(el::conf::Error, vr::builder::Multiple(Float{1.0})(integerRule));
    }
};
