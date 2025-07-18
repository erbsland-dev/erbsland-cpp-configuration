// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/conf/NamePath.hpp>
#include <erbsland/unittest/UnitTest.hpp>


using namespace el::conf;


TESTED_TARGETS(NamePath)
class NamePathTest final : public el::UnitTest {
public:
    NamePath namePath;

    auto additionalErrorMessages() -> std::string override {
        try {
            return std::format("namePath:\n{}", internalView(namePath)->toString(2));
        } catch (...) {
            return "Unexpected exception thrown";
        }
    }

    void testEmpty() {
        namePath = {};
        REQUIRE(namePath.empty());
        REQUIRE_EQUAL(namePath.size(), 0);
    }

    void verifyConstruction(const String &name) {
        REQUIRE_FALSE(namePath.empty());
        REQUIRE_EQUAL(namePath.size(), 1);
        REQUIRE_EQUAL(namePath.at(0), Name::createRegular(name));
        REQUIRE_EQUAL(namePath.toText(), name);
    }

    void testConstruction() {
        const auto name = Name::createRegular(u8"server");
        namePath = name; // implicit, copy
        WITH_CONTEXT(verifyConstruction(u8"server"));

        namePath = Name::createRegular(u8"value"); // implicit, move
        WITH_CONTEXT(verifyConstruction(u8"value"));

        namePath = NamePath{Name::createRegular(u8"tree")}; // explicit, move
        WITH_CONTEXT(verifyConstruction(u8"tree"));

        const auto nameList = NameList{Name::createRegular(u8"worker")};
        namePath = NamePath{nameList}; // explicit, list
        WITH_CONTEXT(verifyConstruction(u8"worker"));

        namePath = NamePath{std::span{nameList}}; // explicit, span
        WITH_CONTEXT(verifyConstruction(u8"worker"));

        namePath = NamePath{nameList.begin(), nameList.end()}; // explicit, iterator
        WITH_CONTEXT(verifyConstruction(u8"worker"));
    }

    void testNamesAccess() {
        namePath = NamePath{{Name::createRegular(u8"server"), Name::createRegular(u8"worker")}};
        const auto expectedNameList = NameList{{Name::createRegular(u8"server"), Name::createRegular(u8"worker")}};
        REQUIRE_EQUAL(namePath.size(), 2);
        REQUIRE_EQUAL(namePath.at(0), Name::createRegular(u8"server"));
        REQUIRE_EQUAL(namePath.at(1), Name::createRegular(u8"worker"));
        REQUIRE_EQUAL(namePath.front(), Name::createRegular(u8"server"));
        REQUIRE_EQUAL(namePath.back(), Name::createRegular(u8"worker"));
        REQUIRE_EQUAL(namePath.view()[0], Name::createRegular(u8"server"));
        REQUIRE_EQUAL(namePath.view()[1], Name::createRegular(u8"worker"));
        REQUIRE_EQUAL(namePath.size(), expectedNameList.size());
        std::size_t index = 0;
        for (const auto &name : namePath) {
            REQUIRE(index < expectedNameList.size())
            REQUIRE_EQUAL(name, expectedNameList.at(index));
            ++index;
        }
    }

    void testParent() {
        namePath = NamePath{
            {
                Name::createRegular(u8"server"),
                Name::createRegular(u8"worker")
            }
        };
        namePath = namePath.parent();
        REQUIRE_EQUAL(namePath.size(), 1);
        REQUIRE_EQUAL(namePath.at(0), Name::createRegular(u8"server"));
        namePath = namePath.parent();
        REQUIRE_EQUAL(namePath.size(), 0);
        namePath = namePath.parent(); // calling on an empty path shouldn't be a problem.
        REQUIRE_EQUAL(namePath.size(), 0);
    }

    void testAppend() {
        // append individual elements.
        namePath = {};
        namePath.append({});
        REQUIRE_EQUAL(namePath.size(), 0);
        namePath.append(Name::createRegular(u8"server"));
        REQUIRE_EQUAL(namePath.size(), 1);
        REQUIRE_EQUAL(namePath.at(0), Name::createRegular(u8"server"));
        namePath.append(Name::createRegular(u8"worker"));
        REQUIRE_EQUAL(namePath.size(), 2);
        REQUIRE_EQUAL(namePath.at(0), Name::createRegular(u8"server"));
        REQUIRE_EQUAL(namePath.at(1), Name::createRegular(u8"worker"));
        namePath.append({});
        REQUIRE_EQUAL(namePath.size(), 2);

        // append another path.
        namePath = NamePath{{Name::createRegular(u8"server"), Name::createRegular(u8"worker")}};
        auto otherPath = NamePath{{Name::createRegular(u8"value"), Name::createRegular(u8"other")}};
        namePath.append(otherPath);
        REQUIRE_EQUAL(namePath.size(), 4);
        REQUIRE_EQUAL(namePath.at(0), Name::createRegular(u8"server"));
        REQUIRE_EQUAL(namePath.at(1), Name::createRegular(u8"worker"));
        REQUIRE_EQUAL(namePath.at(2), Name::createRegular(u8"value"));
        REQUIRE_EQUAL(namePath.at(3), Name::createRegular(u8"other"));
    }

    void testPrepend() {
        // relative paths.
        namePath = NamePath{{Name::createRegular(u8"server"), Name::createRegular(u8"worker")}};
        namePath.prepend({});
        REQUIRE_EQUAL(namePath.size(), 2);
        auto otherPath = NamePath{{Name::createRegular(u8"value"), Name::createRegular(u8"other")}};
        namePath.prepend(otherPath);
        REQUIRE_EQUAL(namePath.size(), 4);
        REQUIRE_EQUAL(namePath.at(0), Name::createRegular(u8"value"));
        REQUIRE_EQUAL(namePath.at(1), Name::createRegular(u8"other"));
        REQUIRE_EQUAL(namePath.at(2), Name::createRegular(u8"server"));
        REQUIRE_EQUAL(namePath.at(3), Name::createRegular(u8"worker"));
    }

    void testHash() {
        namePath = {};
        constexpr auto hasher = std::hash<NamePath>();
        REQUIRE_EQUAL(hasher(namePath), hasher(NamePath{}));
        namePath = NamePath{{Name::createRegular(u8"server"), Name::createRegular(u8"worker")}};
        const auto other = NamePath{{Name::createRegular(u8"server"), Name::createRegular(u8"worker")}};
        REQUIRE_EQUAL(hasher(namePath), hasher(other));
    }

    void testFormat() {
        // As format uses toText(), no in-depth tests are required.
        namePath = NamePath{{Name::createRegular(u8"server"), Name::createRegular(u8"worker")}};
        auto text = std::format("~{}~", namePath);
        REQUIRE_EQUAL(text, "~server.worker~");
    }

    void testInternalView() {
        namePath = NamePath{{Name::createRegular(u8"server"), Name::createText(u8"worker"), Name::createIndex(12)}};
        auto text = internalView(namePath)->toString();
        REQUIRE(text.contains(u8"server"));
        REQUIRE(text.contains(u8"worker"));
        REQUIRE(text.contains(u8"12"));
        REQUIRE(text.contains(u8"Regular"));
        REQUIRE(text.contains(u8"Text"));
        REQUIRE(text.contains(u8"Index"));
    }
};

