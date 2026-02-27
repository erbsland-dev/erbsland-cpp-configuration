// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/ErrorCategory.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <format>
#include <stdexcept>
#include <string>


using namespace el::conf;


TESTED_TARGETS(ErrorCategory)
class ErrorCategoryTest final : public UNITTEST_SUBCLASS(TestHelper) {

    inline static const auto cEnumValues = std::array<std::tuple<ErrorCategory::Enum, String>, 11>{
        std::make_tuple(ErrorCategory::IO, String{u8"IO"}),
        std::make_tuple(ErrorCategory::Encoding, String{u8"Encoding"}),
        std::make_tuple(ErrorCategory::UnexpectedEnd, String{u8"UnexpectedEnd"}),
        std::make_tuple(ErrorCategory::Character, String{u8"Character"}),
        std::make_tuple(ErrorCategory::Syntax, String{u8"Syntax"}),
        std::make_tuple(ErrorCategory::LimitExceeded, String{u8"LimitExceeded"}),
        std::make_tuple(ErrorCategory::NameConflict, String{u8"NameConflict"}),
        std::make_tuple(ErrorCategory::Indentation, String{u8"Indentation"}),
        std::make_tuple(ErrorCategory::Unsupported, String{u8"Unsupported"}),
        std::make_tuple(ErrorCategory::Signature, String{u8"Signature"}),
        std::make_tuple(ErrorCategory::Internal, String{u8"Internal"})
    };

    ErrorCategory ec{};

public:
    auto additionalErrorMessages() -> std::string override {
        try {
            return std::format("ErrorCategory: enum={}, text={}, code={}",
                static_cast<int>(static_cast<ErrorCategory::Enum>(ec)), ec.toText().toCharString(), ec.toCode());
        } catch (...) {
            return {"ErrorCategory: unknown error"};
        }
    }

    void setUp() override {
        ec = {};
    }

    void testDefaultConstruction() {
        ErrorCategory const ec;
        REQUIRE(ec == ErrorCategory::Internal);
    }

    void testConstructor() {
        for (const auto &[value, text] : cEnumValues) {
            ErrorCategory const ec{value};
            REQUIRE(ec == value);
            REQUIRE(ec == ErrorCategory{value});
        }
    }

    void testEnumAssignment() {
        ec = ErrorCategory::IO;
        REQUIRE(ec == ErrorCategory::IO);

        ec = ErrorCategory::Encoding;
        REQUIRE(ec == ErrorCategory::Encoding);
    }

    void testToText() {
        for (const auto &[value, text] : cEnumValues) {
            ec = value;
            REQUIRE(ec.toText() == text);
        }
    }

    void testToCode() {
        ec = ErrorCategory::IO;
        REQUIRE(ec.toCode() == 1);

        ec = ErrorCategory::Signature;
        REQUIRE(ec.toCode() == 10);
    }

    void testCopyAndAssignment() {
        const ErrorCategory ec1 = ErrorCategory::UnexpectedEnd;
        const ErrorCategory ec2 = ec1; // Copy constructor
        REQUIRE(ec1 == ec2);

        ErrorCategory ec3;
        ec3 = ec1; // Assignment operator
        REQUIRE(ec3 == ec1);
    }

    void testCast() {
        ec = ErrorCategory::UnexpectedEnd;
        REQUIRE(static_cast<ErrorCategory::Enum>(ec) == ErrorCategory::UnexpectedEnd);
        REQUIRE(static_cast<int>(ec) == 3);
    }

    void testOperators() {
        WITH_CONTEXT(
            requireAllOperators<ErrorCategory, ErrorCategory>(
                ErrorCategory{ErrorCategory::IO},
                ErrorCategory{ErrorCategory::Encoding},
                ErrorCategory{ErrorCategory::Internal},
                ErrorCategory{ErrorCategory::IO},
                ErrorCategory{ErrorCategory::Encoding},
                ErrorCategory{ErrorCategory::Internal}
            )
        );
        WITH_CONTEXT(
            requireAllOperators<ErrorCategory, ErrorCategory::Enum>(
                ErrorCategory{ErrorCategory::IO},
                ErrorCategory{ErrorCategory::Encoding},
                ErrorCategory{ErrorCategory::Internal},
                ErrorCategory::IO,
                ErrorCategory::Encoding,
                ErrorCategory::Internal
            )
        );
        WITH_CONTEXT(
            requireAllOperators<ErrorCategory::Enum, ErrorCategory>(
                ErrorCategory::IO,
                ErrorCategory::Encoding,
                ErrorCategory::Internal,
                ErrorCategory{ErrorCategory::IO},
                ErrorCategory{ErrorCategory::Encoding},
                ErrorCategory{ErrorCategory::Internal}
            )
        );
    }
};

