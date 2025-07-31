// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/unittest/UnitTest.hpp>
#include <erbsland/conf/impl/AssignmentStream.hpp>
#include <erbsland/conf/impl/Lexer.hpp>
#include <erbsland/conf/impl/InternalView.hpp>
#include <erbsland/conf/impl/value/Value.hpp>

#include <filesystem>
#include <format>
#include <sstream>
#include <cmath>


using namespace el::conf;
using impl::AssignmentStream;
using impl::AssignmentStreamPtr;
using impl::AssignmentGenerator;
using impl::Assignment;
using impl::AssignmentType;
using impl::Lexer;
using impl::LexerPtr;
using impl::CharStream;
using impl::LexerPtr;
using impl::InternalView;
using std::filesystem::path;


class AssignmentStreamHelper : public el::UnitTest {
public:
    path testFilePath;
    SourcePtr source;
    LexerPtr lexer;
    AssignmentStreamPtr stream;
    AssignmentGenerator generator;
    AssignmentGenerator::iterator generatorIterator;
    AssignmentGenerator::iterator generatorIteratorEnd;
    Assignment assignment;

    auto additionalErrorMessages() -> std::string override {
        try {
            std::ostringstream oss;
            oss << "path: " << testFilePath << "\n";
            if (lexer != nullptr) {
                oss << "lexer:\n" << internalView(*lexer)->toString(2).toCharString() << "\n";
            }
            return oss.str();
        } catch (...) {
            return "Unexpected exception thrown";
        }
    }

    void setupAssignmentStream(const std::string &fileName) {
        using std::filesystem::path;
        testFilePath = path(unitTestExecutablePath()).parent_path() / "data" / "assignment_stream" / fileName;
        source = Source::fromFile(testFilePath);
        REQUIRE_NOTHROW(source->open());
        lexer = Lexer::create(CharStream::create(source));
        stream = AssignmentStream::create(lexer);
        generator = stream->assignments();
        generatorIterator = generator.begin();
        generatorIteratorEnd = generator.end();
    }

    void requireAssignment() {
        REQUIRE(generatorIterator != generatorIteratorEnd);
        assignment = *generatorIterator;
    }

    void requireValue(
        const String &expectedNamePath,
        const ValueType expectedValueType) {

        requireAssignment();
        REQUIRE_EQUAL(assignment.namePath().toText(), expectedNamePath);
        REQUIRE_EQUAL(assignment.type(), AssignmentType::Value);
        REQUIRE_EQUAL(assignment.value()->type(), expectedValueType);
        ++generatorIterator;
    }

    template<typename T>
    void requireValue(
        const String &expectedNamePath,
        const ValueType expectedValueType,
        const T &expectedValue) {

        requireAssignment();
        REQUIRE_EQUAL(assignment.namePath().toText(), expectedNamePath);
        REQUIRE_EQUAL(assignment.type(), AssignmentType::Value);
        REQUIRE_EQUAL(assignment.value()->type(), expectedValueType);
        REQUIRE_EQUAL(assignment.value()->asType<T>(), expectedValue);
        ++generatorIterator;
    }

    auto compareFloat(double actual, double expected) {
        if (std::isnan(expected)) {
            REQUIRE(std::isnan(actual));
        } else if (std::isinf(expected)) {
            REQUIRE_EQUAL(actual, expected);
        } else {
            const auto delta = std::abs(actual - expected);
            REQUIRE(delta < std::numeric_limits<double>::epsilon());
        }
    }

    void requireFloat(
        const String &expectedNamePath,
        double expectedValue) {

        requireAssignment();
        REQUIRE_EQUAL(assignment.namePath().toText(), expectedNamePath);
        REQUIRE_EQUAL(assignment.type(), AssignmentType::Value);
        REQUIRE_EQUAL(assignment.value()->type(), ValueType::Float);
        auto actualValue = assignment.value()->asFloat();
        runWithContext(SOURCE_LOCATION(), [&]() {
            compareFloat(actualValue, expectedValue);
        }, [&]() -> std::string {
            return std::format("Failed comparing floats: {} == {}", actualValue, expectedValue);
        });
        ++generatorIterator;
    }

    struct ExpectedListEntry {
        ValueType type;
        impl::Content content;
    };

    void requireList(
        const String &expectedNamePath,
        const std::vector<ExpectedListEntry> &expectedList) {

        // sanity checks for the unit test
        REQUIRE(expectedList.size() > 1);

        requireAssignment();
        REQUIRE_EQUAL(assignment.namePath().toText(), expectedNamePath);
        REQUIRE_EQUAL(assignment.type(), AssignmentType::Value);
        REQUIRE_EQUAL(assignment.value()->type(), ValueType::ValueList);
        const auto valueList = assignment.value()->asValueList();
        REQUIRE_EQUAL(valueList.size(), expectedList.size());
        for (std::size_t i = 0; i < valueList.size(); ++i) {
            const auto &value = valueList[i];
            const auto &expectedType = expectedList[i].type;
            const auto &expectedContent = expectedList[i].content;
            runWithContext(SOURCE_LOCATION(), [&]() {
                REQUIRE_EQUAL(value->type(), expectedType);
                std::visit([&]<typename T>(const T &expectedValue) -> void {
                    if constexpr (std::is_same_v<T, impl::NoContent>) {
                        // ignore no content
                    } else {
                        const auto actualValue = value->asType<T>();
                        REQUIRE_EQUAL(actualValue, expectedValue);
                    }
                }, expectedContent);
            }, [&]() -> std::string {
                return std::format("Failed for list index: {}", i);
            });
        }
        ++generatorIterator;
    }

    template<typename T>
    void requireMetaValue(
    const String &expectedNamePath,
    const ValueType expectedValueType,
    const T &expectedValue) {

        requireAssignment();
        REQUIRE_EQUAL(assignment.namePath().toText(), expectedNamePath);
        REQUIRE_EQUAL(assignment.type(), AssignmentType::MetaValue);
        REQUIRE_EQUAL(assignment.value()->type(), expectedValueType);
        REQUIRE_EQUAL(assignment.value()->asType<T>(), expectedValue);
        ++generatorIterator;
    }

    void requireSectionMap(
        const String &expectedNamePath) {

        requireAssignment();
        REQUIRE_EQUAL(assignment.namePath().toText(), expectedNamePath);
        REQUIRE_EQUAL(assignment.type(), AssignmentType::SectionMap);
        REQUIRE(assignment.value() == nullptr);
        ++generatorIterator;
    }

    void requireSectionList(
        const String &expectedNamePath) {

        requireAssignment();
        REQUIRE_EQUAL(assignment.namePath().toText(), expectedNamePath);
        REQUIRE_EQUAL(assignment.type(), AssignmentType::SectionList);
        REQUIRE(assignment.value() == nullptr);
        ++generatorIterator;
    }

    void requireEnd() {
        requireAssignment();
        REQUIRE_EQUAL(assignment.type(), AssignmentType::EndOfDocument);
        ++generatorIterator;
        REQUIRE(generatorIterator == generatorIteratorEnd);
    }
};

