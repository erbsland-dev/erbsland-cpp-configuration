// Copyright (c) 2025 Erbsland DEV. https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/conf/impl/value/ValueTreeWalker.hpp>
#include <erbsland/conf/Parser.hpp>
#include <erbsland/conf/String.hpp>

using namespace el;
using namespace el::conf;

TESTED_TARGETS(Parser)
class ValueTreeWalkerTest final : public UNITTEST_SUBCLASS(TestHelper) {
private:
    static std::vector<String> collectPreorder(const DocumentPtr &doc, const impl::ValueTreeWalker::Filter &filter = {}) {
        std::vector<String> out;
        impl::ValueTreeWalker walker;
        auto root = std::dynamic_pointer_cast<Value>(doc);
        walker.setRoot(root);
        if (filter) walker.setFilter(filter);
        walker.walk([&out](const ConstValuePtr &node) {
            const auto np = node->namePath().toText();
            out.emplace_back(np.empty() ? String{u8"<root>"} : np);
        });
        return out;
    }

    static std::vector<String> collectPreorder(const ValuePtr &root, const impl::ValueTreeWalker::Filter &filter = {}) {
        std::vector<String> out;
        impl::ValueTreeWalker walker;
        walker.setRoot(root);
        if (filter) walker.setFilter(filter);
        walker.walk([&out](const ConstValuePtr &node) {
            const auto np = node->namePath().toText();
            out.emplace_back(np.empty() ? String{u8"<root>"} : np);
        });
        return out;
    }

public:
    void testPreorderTraversalAndFilter() {
        // Build a simple document with nested sections and values.
        const String text = u8R"(
# Simple tree
[main]
a = 1
[main.sub]
b = 2
[other]
c = 3
)";
        auto source = createTestMemorySource(text);
        Parser parser;
        auto doc = parser.parseOrThrow(source);
        REQUIRE(doc != nullptr);

        // Collect visited name paths in order, pruning "main.sub".
        const auto visited = collectPreorder(doc, [](const ConstValuePtr &node) {
            return node->namePath().toText() != u8"main.sub";
        });

        // Expect preorder in declaration order, with "main.sub" and its child pruned.
        const std::vector<String> expected = {
            u8"<root>",
            u8"main",
            u8"main.a",
            u8"other",
            u8"other.c",
        };
        REQUIRE_EQUAL(visited.size(), expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i) {
            REQUIRE(visited[i] == expected[i]);
        }

        // Also verify that starting from the Value overload yields the same order when not pruning.
        const auto asValue = std::dynamic_pointer_cast<Value>(doc);
        REQUIRE(asValue != nullptr);
        const auto visitedFull = collectPreorder(asValue);

        // Now we expect the full tree (no pruning).
        const std::vector<String> expectedFull = {
            u8"<root>",
            u8"main",
            u8"main.a",
            u8"main.sub",
            u8"main.sub.b",
            u8"other",
            u8"other.c",
        };
        REQUIRE_EQUAL(visitedFull.size(), expectedFull.size());
        for (std::size_t i = 0; i < expectedFull.size(); ++i) {
            REQUIRE(visitedFull[i] == expectedFull[i]);
        }
    }

    void testLargerDocumentTraversal() {
        // Larger tree with multiple siblings and nested subsections.
        const String text = u8R"(
# Larger tree
[app]
name = "demo"
version = 1
[app.ui]
theme = "dark"
[app.ui.colors]
primary = "#123456"
[app.modules]
# sibling subsections A and B
[app.modules.A]
enabled = enabled
[app.modules.B]
level = 3
[db]
host = "localhost"
port = 5432
)";
        Parser parser;
        auto doc = parser.parseOrThrow(createTestMemorySource(text));

        const auto visited = collectPreorder(doc);
        const std::vector<String> requiredNodes = {
            u8"<root>",
            u8"app",
            u8"app.name",
            u8"app.version",
            u8"app.ui",
            u8"app.ui.theme",
            u8"app.ui.colors.primary",
            u8"db.host",
            u8"db.port",
        };
        // Ensure all required nodes are present.
        for (const auto &name : requiredNodes) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                bool found = false;
                for (const auto &v : visited) {
                    if (v == name) { found = true; break; }
                }
                REQUIRE(found);
            }, [&]() -> std::string {
                return std::format("Required node not found in traversal: {}", name.toCharString());
            });
        }
        // Verify preorder constraints: parents before their children.
        auto indexOf = [&](const String &name) -> std::size_t {
            for (std::size_t i = 0; i < visited.size(); ++i) if (visited[i] == name) return i;
            return visited.size();
        };
        REQUIRE(indexOf(u8"<root>") == 0);
        REQUIRE(indexOf(u8"app")    < indexOf(u8"app.name"));
        REQUIRE(indexOf(u8"app")    < indexOf(u8"app.version"));
        REQUIRE(indexOf(u8"app")    < indexOf(u8"app.ui"));
        REQUIRE(indexOf(u8"app.ui") < indexOf(u8"app.ui.theme"));
        // Accept either implicit or explicit colors section node.
        REQUIRE(indexOf(u8"app.ui") < indexOf(u8"app.ui.colors.primary"));
        // Modules ordering
        REQUIRE(indexOf(u8"app") < indexOf(u8"app.modules.A.enabled"));
        REQUIRE(indexOf(u8"app") < indexOf(u8"app.modules.B.level"));
        // DB ordering
        REQUIRE(indexOf(u8"db.host") > indexOf(u8"<root>"));
        REQUIRE(indexOf(u8"db.port") > indexOf(u8"<root>"));
    }

    void testExceptionPropagationFromVisit() {
        const String text = u8R"(
[root]
a = 1
)";
        Parser parser;
        auto doc = parser.parseOrThrow(createTestMemorySource(text));

        struct Boom { };
        bool thrown = false;
        try {
            impl::ValueTreeWalker walker;
            auto root = std::dynamic_pointer_cast<Value>(doc);
            walker.setRoot(root);
            walker.walk([](const ConstValuePtr &node) {
                if (node->namePath().toText() == u8"root.a") {
                    throw Boom{};
                }
            });
        } catch (const Boom &) {
            thrown = true;
        }
        REQUIRE(thrown);
    }

    void testExceptionPropagationFromFilter() {
        const String text = u8R"(
[root]
a = 1
)";
        Parser parser;
        auto doc = parser.parseOrThrow(createTestMemorySource(text));

        struct MyError { };
        bool thrown = false;
        try {
            impl::ValueTreeWalker walker;
            auto root = std::dynamic_pointer_cast<Value>(doc);
            walker.setRoot(root);
            walker.setFilter([](const ConstValuePtr &node) -> bool {
                if (node->namePath().toText() == u8"root") {
                    throw MyError{};
                }
                return true;
            });
            walker.walk(impl::ValueTreeWalker::Visit{});
        } catch (const MyError &) {
            thrown = true;
        }
        REQUIRE(thrown);
    }
};
