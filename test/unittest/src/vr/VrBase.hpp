// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/conf/Parser.hpp>
#include <erbsland/conf/impl/vr/Rules.hpp>
#include <erbsland/conf/vr/Rules.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;


class VrBase : public el::UnitTest {
public:
    String failedText;
    DocumentPtr vrDocument;
    vr::RulesPtr rules;
    DocumentPtr document;
    String lastError;

    void setUp() override {
        failedText = {};
        vrDocument = nullptr;
        rules = nullptr;
        document = nullptr;
        lastError = {};
    }

    auto additionalErrorMessages() -> std::string override {
        try {
            std::string result;
            if (!failedText.empty()) {
                result += std::format("Failed text:\n{}\n", failedText.toSafeText());
            }
            if (vrDocument == nullptr) {
                result += "VR document: <null>\n";
            } else {
                result += std::format("VR document:\n{}\n", vrDocument->toTestValueTree());
            }
            auto rulesImpl = std::dynamic_pointer_cast<impl::Rules>(rules);
            if (rulesImpl == nullptr) {
                result += "Validated rules: <null>\n";
            } else {
                result += std::format("Validated rules:\n{}\n", internalView(rulesImpl)->toString());
            }
            if (document == nullptr) {
                result += "Validated document: <null>\n";
            } else {
                result += std::format("Validated document:\n{}\n", document->toTestValueTree());
            }
            if (lastError.empty()) {
                result += "Last error: <empty>\n";
            } else {
                result += std::format("Last error: {}\n", lastError.toCharString());
            }
            return result;
        } catch (...) {
            return "Unexpected exception thrown";
        }
    }

    [[nodiscard]] static auto linesToString(const std::vector<std::string_view> &lines) -> String {
        String result;
        for (const auto &line : lines) {
            result += line;
            result += "\n";
        }
        return result;
    }

    void requireRulesPass(const String &text) {
        lastError = {};
        try {
            Parser vrParser;
            vrDocument = vrParser.parseTextOrThrow(text);
        } catch (const Error &e) {
            failedText = text;
            lastError = e.toText();
            REQUIRE(false);
        }
        REQUIRE(vrDocument != nullptr);
        try {
            rules = vr::Rules::createFromDocument(vrDocument);
        } catch (const Error &e) {
            lastError = e.toText();
            REQUIRE(false);
        }
        REQUIRE(rules != nullptr);
    }

    void requireRulesPassLines(const std::vector<std::string_view> &lines) {
        requireRulesPass(linesToString(lines));
    }

    /// Test if compiling *rules* fail. Expects a valid configuration document.
    void requireRulesFail(const String &text) {
        lastError = {};
        try {
            Parser vrParser;
            vrDocument = vrParser.parseTextOrThrow(text);
        } catch (const Error &e) {
            failedText = text;
            lastError = e.toText();
            REQUIRE(false);
        }
        REQUIRE(vrDocument != nullptr);
        try {
            rules = vr::Rules::createFromDocument(vrDocument);
            REQUIRE(false);
        } catch (const Error &e) {
            lastError = e.toText();
        }
    }

    /// Test if compiling *rules* fail. Expects a valid configuration document.
    void requireRulesFailLines(const std::vector<std::string_view> &lines) {
        requireRulesFail(linesToString(lines));
    }

    void requirePass(const String &text, const Integer version = 0) {
        lastError = {};
        Parser docParser;
        try {
            document = docParser.parseTextOrThrow(text);
        } catch (const Error &e) {
            lastError = e.toText();
            REQUIRE(false);
        }
        REQUIRE(document != nullptr);
        REQUIRE(rules != nullptr);
        try {
            rules->validate(document, version);
        } catch (const Error &e) {
            lastError = e.toText();
            REQUIRE(false);
        }
    }

    void requirePassLines(const std::vector<std::string_view> &lines, const Integer version = 0) {
        requirePass(linesToString(lines), version);
    }

    void requireFail(const String &text, const Integer version = 0) {
        lastError = {};
        Parser docParser;
        REQUIRE_NOTHROW(document = docParser.parseTextOrThrow(text));
        REQUIRE(document != nullptr);
        REQUIRE(rules != nullptr);
        try {
            rules->validate(document, version);
            REQUIRE(false);
        } catch (const Error &e) {
            REQUIRE_EQUAL(e.category(), ErrorCategory::Validation);
            lastError = e.toText();
        }
    }

    void requireFailLines(const std::vector<std::string_view> &lines, const Integer version = 0) {
        requireFail(linesToString(lines), version);
    }

    void requireError(const String &partialMatch) {
        REQUIRE(lastError.contains(partialMatch, CaseSensitivity::CaseInsensitive));
    }

    [[nodiscard]] static auto buildOneConstraintDoc(
        const std::string &constraintLine,
        const vr::RuleType ruleType,
        bool caseSensitive = false) -> String {
        auto typeLine = std::format("type: \"{}\"", ruleType.toText());
        std::vector<std::string_view> lines = {
            "[app.x]",
            std::string_view{typeLine},
            std::string_view{constraintLine},
        };
        if (ruleType == vr::RuleType::ValueList || ruleType == vr::RuleType::ValueMatrix) {
            lines.emplace_back("[app.x.vr_entry]");
            lines.emplace_back("type: \"integer\"");
        } else if (ruleType == vr::RuleType::SectionList) {
            lines.emplace_back("[app.x.vr_entry.y]");
            lines.emplace_back("type: \"integer\"");
        } else if (ruleType == vr::RuleType::Section) {
            lines.emplace_back("[app.x.vr_any]");
            lines.emplace_back("type: \"integer\"");
        } else if (ruleType == vr::RuleType::SectionWithTexts) {
            lines.emplace_back("[app.x.vr_any]");
            lines.emplace_back("type: \"integer\"");
        }
        if (caseSensitive) {
            lines.emplace_back("case_sensitive: true");
        }
        return linesToString(lines);
    }

    void requireOneConstraintPass(
        const std::string &constraintLine,
        const vr::RuleType ruleType,
        bool caseSensitive = false) {

        WITH_CONTEXT(requireRulesPass(buildOneConstraintDoc(constraintLine, ruleType, caseSensitive)))
    }

    void requireOneConstraintFail(
        const std::string &constraintLine,
        const vr::RuleType ruleType,
        bool caseSensitive = false) {

        WITH_CONTEXT(requireRulesFail(buildOneConstraintDoc(constraintLine, ruleType, caseSensitive)))
    }

    void requireConstraintValidForRuleTypes(
        const std::string &constraintLine,
        const std::set<vr::RuleType> &validRuleTypes) {

        for (auto testedRuleType : vr::RuleType::all()) {
            if (testedRuleType == vr::RuleType::Alternatives) {
                continue; // alternatives must be defined as a section list and do not allow constraints.
            }
            if (validRuleTypes.contains(testedRuleType)) {
                WITH_CONTEXT(requireOneConstraintPass(constraintLine, testedRuleType));
            } else {
                WITH_CONTEXT(requireOneConstraintFail(constraintLine, testedRuleType));
            }
        }
    }
};
