// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Constraint.hpp"
#include "ConstraintHandlerContext.hpp"
#include "ValidationContext.hpp"

#include <cmath>
#include <limits>
#include <utility>


namespace erbsland::conf::impl {


template<typename T>
class EqualsConstraint : public Constraint {
public:
    template<typename Fwd>
    requires (std::is_same_v<std::remove_cvref_t<Fwd>, T>)
    explicit EqualsConstraint(Fwd &&value) : _value{std::forward<Fwd>(value)} {
        setType(vr::ConstraintType::Equals);
    }

protected:
    [[nodiscard]] auto isEqual(const T &a, const T &b, const ValidationContext &context) const -> bool {
        if constexpr (std::is_same_v<T, String>) {
            return a.characterCompare(b, context.rule->caseSensitivity()) == std::strong_ordering::equal;
        } else if constexpr (std::is_same_v<T, Bytes>) {
            return a == b;
        } else if constexpr (std::is_floating_point_v<T>) {
            if (std::isnan(a) && std::isnan(b)) {
                return true;
            }
            if (std::isinf(a) || std::isinf(b)) {
                return a == b; // true only for the same sign infinity
            }
            return std::abs(a - b) < std::numeric_limits<T>::epsilon();
        } else {
            return a == b;
        }
    }

    [[nodiscard]] auto isNotValid(const T &validatedValue, const ValidationContext &context) const -> bool {
        if (isNegated()) {
            return isEqual(validatedValue, _value, context);
        }
        return !isEqual(validatedValue, _value, context);
    }

    [[nodiscard]] auto comparisonText() const -> const String& {
        static const String equal{u8"must be equal to"};
        static const String notEqual{u8"must not be equal to"};
        return isNegated() ? notEqual : equal;
    }

protected:
    T _value;
};


class EqualsIntegerConstraint final : public EqualsConstraint<Integer> {
public:
    explicit EqualsIntegerConstraint(Integer value);

protected:
    void validateInteger(const ValidationContext &context, Integer value) const override;
    void validateText(const ValidationContext &context, const String &value) const override;
    void validateBytes(const ValidationContext &context, const Bytes &value) const override;
    void validateValueList(const ValidationContext &context) const override;
    void validateSectionWithNames(const ValidationContext &context) const override;
    void validateSectionWithTexts(const ValidationContext &context) const override;
    void validateSectionList(const ValidationContext &context) const override;
};


class EqualsBooleanConstraint final : public EqualsConstraint<bool> {
public:
    explicit EqualsBooleanConstraint(bool value);

protected:
    void validateBoolean(const ValidationContext &context, bool value) const override;
};


class EqualsFloatConstraint final : public EqualsConstraint<Float> {
public:
    explicit EqualsFloatConstraint(Float value);

protected:
    void validateFloat(const ValidationContext &context, Float value) const override;
};


class EqualsTextConstraint final : public EqualsConstraint<String> {
public:
    template<typename Fwd>
    requires (std::is_same_v<std::remove_cvref_t<Fwd>, String>)
    explicit EqualsTextConstraint(Fwd &&expected) : EqualsConstraint(std::forward<Fwd>(expected)) {}

protected:
    void validateText(const ValidationContext &context, const String &value) const override;
};


class EqualsBytesConstraint final : public EqualsConstraint<Bytes> {
public:
    template<typename Fwd>
    requires (std::is_same_v<std::remove_cvref_t<Fwd>, Bytes>)
    explicit EqualsBytesConstraint(Fwd &&expected) : EqualsConstraint(std::forward<Fwd>(expected)) {}

protected:
    void validateBytes(const ValidationContext &context, const Bytes &value) const override;
};


class EqualsMatrixConstraint final : public EqualsConstraint<Integer> {
public:
    explicit EqualsMatrixConstraint(Integer rows, Integer columns);

protected:
    [[nodiscard]] auto isNotValidColumns(const Integer &validatedValue, const ValidationContext &context) const -> bool;
    void validateValueList(const ValidationContext &context) const override;

private:
    Integer _columns;
};


auto handleEqualsConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr;


}
