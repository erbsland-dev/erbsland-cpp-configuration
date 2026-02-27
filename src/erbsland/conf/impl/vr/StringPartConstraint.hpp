// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Constraint.hpp"
#include "ConstraintHandlerContext.hpp"
#include "ValidationContext.hpp"


namespace erbsland::conf::impl {


class StringPartConstraint : public Constraint {
public:
    template<typename Fwd>
    requires (std::is_same_v<std::remove_cvref_t<Fwd>, std::vector<String>>)
    explicit StringPartConstraint(Fwd &&values) : _expectedValues(std::forward<Fwd>(values)) {
    }

protected: // implement Constraint
    void validateText(const ValidationContext &context, const String &value) const override;

protected: // interface for subclasses
    [[nodiscard]] virtual auto partText() const -> const String& = 0;
    [[nodiscard]] virtual auto doesPartMatch(
        const String &expectedValue,
        const String &testedValue,
        const ValidationContext &context) const -> bool = 0;

private:
    std::vector<String> _expectedValues;
};


class StartsConstraint final : public StringPartConstraint {
public:
    explicit StartsConstraint(const std::vector<String> &values) : StringPartConstraint(values) {
        setType(vr::ConstraintType::Starts);
    }

protected:
    [[nodiscard]] auto partText() const -> const String& override {
        static const String text{u8"start with"};
        return text;
    }
    [[nodiscard]] auto doesPartMatch(
        const String &expectedValue,
        const String &testedValue,
        const ValidationContext &context) const -> bool override {

        return testedValue.startsWith(expectedValue, context.rule->caseSensitivity());
    }
};


class EndsConstraint final : public StringPartConstraint {
public:
    explicit EndsConstraint(const std::vector<String> &values) : StringPartConstraint(values) {
        setType(vr::ConstraintType::Ends);
    }

protected:
    [[nodiscard]] auto partText() const -> const String& override {
        static const String text{u8"end with"};
        return text;
    }
    [[nodiscard]] auto doesPartMatch(
        const String &expectedValue,
        const String &testedValue,
        const ValidationContext &context) const -> bool override {

        return testedValue.endsWith(expectedValue, context.rule->caseSensitivity());
    }
};


class ContainsConstraint final : public StringPartConstraint {
public:
    explicit ContainsConstraint(const std::vector<String> &values) : StringPartConstraint(values) {
        setType(vr::ConstraintType::Contains);
    }

protected:
    [[nodiscard]] auto partText() const -> const String& override {
        static const String text{u8"contain"};
        return text;
    }
    [[nodiscard]] auto doesPartMatch(
        const String &expectedValue,
        const String &testedValue,
        const ValidationContext &context) const -> bool override {

        return testedValue.contains(expectedValue, context.rule->caseSensitivity());
    }
};


auto handleStartsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr;
auto handleEndsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr;
auto handleContainsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr;


}

