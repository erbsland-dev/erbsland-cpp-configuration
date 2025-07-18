// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/Error.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>


using namespace el::conf;


TESTED_TARGETS(Error Location ErrorCategory)
class ErrorTest final : public el::UnitTest {
public:
    void testConstructorWithMessageOnly() {
        Error const error(ErrorCategory::Syntax, u8"Syntax error occurred");
        REQUIRE(error.category() == ErrorCategory::Syntax);
        REQUIRE(error.message() == u8"Syntax error occurred");
        REQUIRE(error.toText().find(u8"Syntax error occurred") != String::npos);
    }

    void testConstructorWithMessageAndLocation() {
        const auto sourceIdentifier = SourceIdentifier::createForFile(u8"example.elcl");
        Location location{sourceIdentifier, Position{42, 10}};
        Error const error(ErrorCategory::IO, u8"File read error", location);

        REQUIRE(error.category() == ErrorCategory::IO);
        REQUIRE(error.message() == u8"File read error");

        String const text = error.toText();
        REQUIRE(text.find(u8"File read error") != String::npos);
        REQUIRE(text.find(u8":42:10") != String::npos);
    }

    void testDefaultConstructedLocation() {
        Error const error(ErrorCategory::Encoding, u8"Encoding issue");
        Location defaultLocation;

        // Check that the default location is empty or has default values
        REQUIRE(error.message() == u8"Encoding issue");
        REQUIRE(error.category() == ErrorCategory::Encoding);

        // `toText` should not contain any specific location information
        REQUIRE(error.toText().find(u8"Encoding issue") != String::npos);
        REQUIRE(error.toText().find(u8"Line") == String::npos);
    }

    void testWhat() {
        Error error(ErrorCategory::LimitExceeded, u8"Limit exceeded on request");

        // `what()` should return a const char* with the error message
        const char* whatStr = error.what();
        REQUIRE(std::string(whatStr).find("Limit exceeded on request") != std::string::npos);
    }

    void testCopyAndAssignment() {
        Error originalError(ErrorCategory::Unsupported, u8"Unsupported operation");

        // Copy constructor
        Error copyError = originalError;
        REQUIRE(copyError.category() == originalError.category());
        REQUIRE(copyError.message() == originalError.message());
        REQUIRE(copyError.toText() == originalError.toText());

        // Move constructor
        Error movedError = std::move(copyError);
        REQUIRE(movedError.category() == originalError.category());
        REQUIRE(movedError.message() == originalError.message());
        REQUIRE(movedError.toText() == originalError.toText());

        // Assignment operator
        Error assignedError(ErrorCategory::Character, u8"Temporary message");
        assignedError = originalError;
        REQUIRE(assignedError.category() == originalError.category());
        REQUIRE(assignedError.message() == originalError.message());
        REQUIRE(assignedError.toText() == originalError.toText());

        // Move assignment
        Error movedAssignedError(ErrorCategory::Signature, u8"Temporary message");
        movedAssignedError = std::move(assignedError);
        REQUIRE(movedAssignedError.category() == originalError.category());
        REQUIRE(movedAssignedError.message() == originalError.message());
        REQUIRE(movedAssignedError.toText() == originalError.toText());
    }
};

