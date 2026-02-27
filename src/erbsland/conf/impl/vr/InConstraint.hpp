// Copyright (c) 2025-2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Constraint.hpp"
#include "ConstraintHandlerContext.hpp"
#include "ValidationContext.hpp"

#include "../../CaseSensitivity.hpp"
#include "../../Error.hpp"

#include <cmath>
#include <limits>
#include <utility>
#include <vector>


namespace erbsland::conf::impl {


template<typename T>
class InConstraint : public Constraint {
public:
    template<typename Fwd>
    requires (std::is_same_v<std::remove_cvref_t<Fwd>, std::vector<T>>)
    explicit InConstraint(Fwd &&values) : _values(std::forward<Fwd>(values)) {
        setType(vr::ConstraintType::In);
    }

public:
    [[nodiscard]] static auto hasDuplicate(const std::vector<T> &values, const CaseSensitivity cs) -> bool {
        for (std::size_t i = 0; i < values.size(); ++i) {
            for (std::size_t j = i + 1; j < values.size(); ++j) {
                if (areEqual(values[i], values[j], cs)) {
                    return true;
                }
            }
        }
        return false;
    }

protected:
    [[nodiscard]] auto isEqual(const T &a, const T &b, const ValidationContext &context) const -> bool {
        return areEqual(a, b, context.rule->caseSensitivity());
    }

    [[nodiscard]] static auto areEqual(const T &a, const T &b, const CaseSensitivity cs) -> bool {
        if constexpr (std::is_same_v<T, String>) {
            return a.characterCompare(b, cs) == std::strong_ordering::equal;
        } else if constexpr (std::is_same_v<T, Bytes>) {
            return a == b;
        } else if constexpr (std::is_floating_point_v<T>) {
            return std::abs(a - b) < std::numeric_limits<T>::epsilon();
        } else {
            return a == b;
        }
    }

    [[nodiscard]] auto contains(const T &value, const ValidationContext &context) const -> bool {
        for (const auto &v : _values) {
            if (isEqual(v, value, context)) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] auto isNotValid(const T &validatedValue, const ValidationContext &context) const -> bool {
        if (isNegated()) {
            // invalid if it is in the list when negated
            return contains(validatedValue, context);
        }
        // invalid if it is not in the list when not negated
        return !contains(validatedValue, context);
    }

    [[nodiscard]] auto comparisonText() const -> const String& {
        static const String inText{u8"must be one of"};
        static const String notInText{u8"must not be one of"};
        return isNegated() ? notInText : inText;
    }

protected:
    std::vector<T> _values;
};


class InIntegerConstraint final : public InConstraint<Integer> {
public:
    explicit InIntegerConstraint(const std::vector<Integer> &values) : InConstraint(values) {}

protected:
    void validateInteger(const ValidationContext &context, Integer value) const override;
};


class InFloatConstraint final : public InConstraint<Float> {
public:
    explicit InFloatConstraint(const std::vector<Float> &values) : InConstraint(values) {}

protected:
    void validateFloat(const ValidationContext &context, Float value) const override;
};


class InTextConstraint final : public InConstraint<String> {
public:
    explicit InTextConstraint(const std::vector<String> &values) : InConstraint(values) {}

protected:
    void validateText(const ValidationContext &context, const String &value) const override;
};


class InBytesConstraint final : public InConstraint<Bytes> {
public:
    explicit InBytesConstraint(const std::vector<Bytes> &values) : InConstraint(values) {}

protected:
    void validateBytes(const ValidationContext &context, const Bytes &value) const override;
};


auto handleInConstraint(const ConstraintHandlerContext &context) -> ConstraintPtr;


}
