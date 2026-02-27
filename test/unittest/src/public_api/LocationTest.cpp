// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/Location.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>
#include <utility>


using namespace el::conf;


TESTED_TARGETS(Location)
class LocationTest final : public el::UnitTest {
public:
    void testDefaultConstructor() {
        Location loc;
        REQUIRE(loc.isUndefined());
        REQUIRE(loc.sourceIdentifier() == nullptr);
        REQUIRE(loc.position().isUndefined());
    }

    void testParameterizedConstructor() {
        const auto sourceIdentifier = SourceIdentifier::createForFile(u8"file.elcl");
        Location loc(sourceIdentifier, Position{42, 10});
        REQUIRE_FALSE(loc.isUndefined());
        REQUIRE(SourceIdentifier::areEqual(loc.sourceIdentifier(), sourceIdentifier));
        REQUIRE(loc.position().line() == 42);
        REQUIRE(loc.position().column() == 10);
    }

    void testEqualityOperators() {
        Location loc1; // Undefined
        Location loc2; // Undefined

        // Two undefined locations should be equal
        REQUIRE(loc1 == loc2);

        // Two undefined locations should not be unequal
        REQUIRE_FALSE(loc1 != loc2);

        const auto sourceIdentifier = SourceIdentifier::createForFile(u8"file.elcl");
        Location loc3(sourceIdentifier, Position{10, 20});
        Location loc4(sourceIdentifier, Position{10, 20});

        // Locations with the same data should be equal
        REQUIRE(loc3 == loc4);

        // Locations with the same data should not be unequal
        REQUIRE_FALSE(loc3 != loc4);

        Location loc5(sourceIdentifier, Position{10, 21});

        // Locations with different columns should be unequal
        REQUIRE(loc3 != loc5);

        Location loc6(sourceIdentifier, Position{11, 20});

        // Locations with different lines should be unequal
        REQUIRE(loc3 != loc6);

        const auto sourceIdentifier2 = SourceIdentifier::createForFile(u8"another_file.elcl");
        Location loc7(sourceIdentifier2, Position{10, 20});

        // Locations with different source identifiers should be unequal
        REQUIRE(loc3 != loc7);
    }

    void testCopyConstructor() {
        const auto sourceIdentifier = SourceIdentifier::createForFile(u8"file.elcl");
        Location original(sourceIdentifier, Position{30, 40});
        Location copy = original;

        // The copied location should be equal to the original
        REQUIRE(copy == original);
    }

    void testMoveConstructor() {
        const auto sourceIdentifier = SourceIdentifier::createForFile(u8"file.elcl");
        Location original(sourceIdentifier, Position{50, 60});
        Location moved = std::move(original);

        // Moved location should retain the source identifier
        REQUIRE(moved.sourceIdentifier()->name() == u8"file");
        REQUIRE(moved.sourceIdentifier()->path() == u8"file.elcl");

        // Moved location should retain the line number
        REQUIRE(moved.position().line() == 50);

        // Moved location should retain the column number
        REQUIRE(moved.position().column() == 60);

        // Note: The state of 'original' after move is unspecified
    }

    void testCopyAssignment() {
        const auto sourceIdentifier = SourceIdentifier::createForFile(u8"file.elcl");
        Location loc1(sourceIdentifier, Position{70, 80});
        Location loc2;
        loc2 = loc1;

        // After copy assignment, loc2 should be equal to loc1
        REQUIRE(loc2 == loc1);
    }

    void testMoveAssignment() {
        const auto sourceIdentifier = SourceIdentifier::createForFile(u8"file2.elcl");
        Location loc1(sourceIdentifier, Position{90, 100});
        Location loc2;
        loc2 = std::move(loc1);

        // Move-assigned location should have the correct source identifier
        REQUIRE(loc2.sourceIdentifier()->path() == u8"file2.elcl");

        // Move-assigned location should have the correct line number
        REQUIRE(loc2.position().line() == 90);

        // Move-assigned location should have the correct column number
        REQUIRE(loc2.position().column() == 100);
    }

    void testAccessors() {
        const auto sourceIdentifier = SourceIdentifier::createForFile(u8"source.elcl");
        Location loc(sourceIdentifier, Position{15, 25});

        // Check sourceIdentifier accessor
        REQUIRE(loc.sourceIdentifier()->path() == "source.elcl");

        // Check line accessor
        REQUIRE(loc.position().line() == 15);

        // Check column accessor
        REQUIRE(loc.position().column() == 25);
    }
    
    void testToText() {
        const auto sourceIdentifier = SourceIdentifier::createForFile(u8"config.elcl");
        Location loc(sourceIdentifier, Position{5, 10});
        String expected = u8"file:config.elcl:5:10";

        // Check if toText returns the correct formatted string
        REQUIRE(loc.toText() == expected);

        Location undefinedLoc;

        // Check if toText correctly represents an undefined location
        REQUIRE(undefinedLoc.toText() == u8"<unknown>");
    }
};
