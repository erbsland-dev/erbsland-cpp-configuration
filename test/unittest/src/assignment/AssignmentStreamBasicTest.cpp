// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "AssignmentStreamHelper.hpp"


TESTED_TARGETS(AssignmentStream)
class AssignmentStreamBasicTest final : public UNITTEST_SUBCLASS(AssignmentStreamHelper) {
public:
    // Basic test to make sure the unittest works as expected.
    void testBasicFunctionality() {
        WITH_CONTEXT(setupAssignmentStream("basic.elcl"));
        WITH_CONTEXT(requireSectionMap("main"));
        WITH_CONTEXT(requireValue("main.server", ValueType::Text));
        WITH_CONTEXT(requireValue("main.port", ValueType::Integer));
        WITH_CONTEXT(requireEnd());
    }

    void testSingleValues() {
        WITH_CONTEXT(setupAssignmentStream("single_values.elcl"));
        WITH_CONTEXT(requireSectionMap("main"));
        WITH_CONTEXT(requireValue("main.value_1", ValueType::Integer, Integer{12345}));
        WITH_CONTEXT(requireValue("main.value_2", ValueType::Boolean, true));
        WITH_CONTEXT(requireValue("main.value_3", ValueType::Float, 12.345));
        WITH_CONTEXT(requireValue("main.value_4", ValueType::Text, String{u8"This is Text"}));
        WITH_CONTEXT(requireValue("main.value_5", ValueType::Text, String{u8"This is Code"}));
        WITH_CONTEXT(requireValue("main.value_6", ValueType::Date, Date{2026, 8, 10}));
        WITH_CONTEXT(requireValue("main.value_7", ValueType::Time, Time{17, 54, 12, 0, TimeOffset::utc()}));
        WITH_CONTEXT(requireValue("main.value_8", ValueType::DateTime, DateTime{Date{2026, 8, 10}, Time{17, 54, 12, 0, TimeOffset::utc()}}));
        WITH_CONTEXT(requireValue("main.value_9", ValueType::Bytes, Bytes::fromHex("010203fdfeff")));
        WITH_CONTEXT(requireValue("main.value_10", ValueType::TimeDelta, TimeDelta{TimeUnit::Years, 10}));
        WITH_CONTEXT(requireValue("main.value_11", ValueType::RegEx, RegEx{u8"regex", false}));

        WITH_CONTEXT(requireValue("main.value_12", ValueType::Integer, Integer{12345}));
        WITH_CONTEXT(requireValue("main.value_13", ValueType::Text, String{u8"This is Text"}));
        WITH_CONTEXT(requireValue("main.value_14", ValueType::Date, Date{2026, 8, 10}));
    }

    void testMultiLineValues() {
        WITH_CONTEXT(setupAssignmentStream("multiline_values.elcl"));
        WITH_CONTEXT(requireSectionMap("text"));
        WITH_CONTEXT(requireValue("text.value_1", ValueType::Text, String{u8"Hello World!"}));
        WITH_CONTEXT(requireValue("text.value_2", ValueType::Text, String{u8"\nHello World!\n"}));
        WITH_CONTEXT(requireValue("text.value_3", ValueType::Text, String{u8"Hello World!"}));
        WITH_CONTEXT(requireValue("text.value_4", ValueType::Text, String{u8"    Hello World!"}));
        WITH_CONTEXT(requireValue("text.value_5", ValueType::Text, String{u8"The first line.\nA second line.\nThird line of text."}));
        WITH_CONTEXT(requireSectionMap("code"));
        WITH_CONTEXT(requireValue("code.value_1", ValueType::Text, String{u8"Code\\n"}));
        WITH_CONTEXT(requireValue("code.value_2", ValueType::Text, String{u8"\nCode\\n\n"}));
        WITH_CONTEXT(requireValue("code.value_3", ValueType::Text, String{u8"Code\\n"}));
        WITH_CONTEXT(requireValue("code.value_4", ValueType::Text, String{u8"    Code\\n"}));
        WITH_CONTEXT(requireValue("code.value_5", ValueType::Text, String{u8"if len(lines) == 3:\n    print(f\"{lines}\\n\")\nexit(0)"}));
        WITH_CONTEXT(requireSectionMap("regex"));
        WITH_CONTEXT(requireValue("regex.value_1", ValueType::RegEx, RegEx{u8"^\\w+\\.[Ee][Ll][Cc][Ll]$", true}));
        WITH_CONTEXT(requireValue("regex.value_2", ValueType::RegEx, RegEx{u8"\n^\\w+\\.[Ee][Ll][Cc][Ll]$\n", true}));
        WITH_CONTEXT(requireValue("regex.value_3", ValueType::RegEx, RegEx{u8"^\\w+\\.[Ee][Ll][Cc][Ll]$", true}));
        WITH_CONTEXT(requireValue("regex.value_4", ValueType::RegEx, RegEx{u8"    ^\\w+\\.[Ee][Ll][Cc][Ll]$", true}));
        WITH_CONTEXT(requireValue("regex.value_5", ValueType::RegEx, RegEx{u8"^\\w+\n    \\.[Ee][Ll][Cc][Ll]\n$", true}));
        WITH_CONTEXT(requireSectionMap("bytes"));
        WITH_CONTEXT(requireValue("bytes.value_1", ValueType::Bytes, Bytes::fromHex("01020304e1e2e3e4")));
        WITH_CONTEXT(requireValue("bytes.value_2", ValueType::Bytes, Bytes::fromHex("01020304e1e2e3e4")));
        WITH_CONTEXT(requireValue("bytes.value_3", ValueType::Bytes, Bytes::fromHex("01020304e1e2e3e4")));
        WITH_CONTEXT(requireValue("bytes.value_4", ValueType::Bytes, Bytes::fromHex("01020304e1e2e3e4")));
        WITH_CONTEXT(requireEnd());
    }

    void testSections() {
        WITH_CONTEXT(setupAssignmentStream("sections.elcl"));
        WITH_CONTEXT(requireSectionMap("main"));
        WITH_CONTEXT(requireSectionMap("main.server.filter"));
        WITH_CONTEXT(requireValue("main.server.filter.value", ValueType::Text, String{u8"text"}));
        WITH_CONTEXT(requireSectionMap("main.client.filter"));
        WITH_CONTEXT(requireValue("main.client.filter.value", ValueType::Text, String{u8"text"}));
        WITH_CONTEXT(requireSectionMap("text.\"First Text\""));
        WITH_CONTEXT(requireValue("text.\"First Text\".value", ValueType::Integer, 1));
        WITH_CONTEXT(requireSectionMap("text.\"Second Text\""));
        WITH_CONTEXT(requireValue("text.\"Second Text\".value", ValueType::Integer, 2));
        WITH_CONTEXT(requireEnd());
    }

    void testMeta() {
        WITH_CONTEXT(setupAssignmentStream("meta.elcl"));
        WITH_CONTEXT(requireMetaValue("@signature", ValueType::Text, String{u8"data"}));
        WITH_CONTEXT(requireMetaValue("@version", ValueType::Text, String{u8"1.0"}));
        WITH_CONTEXT(requireMetaValue("@features", ValueType::Text, String{u8"core multi-line time-delta"}));
        WITH_CONTEXT(requireSectionMap("main"));
        WITH_CONTEXT(requireMetaValue("@include", ValueType::Text, String{u8"path1"}));
        WITH_CONTEXT(requireMetaValue("@include", ValueType::Text, String{u8"path2"}));
        WITH_CONTEXT(requireSectionMap("second"));
        WITH_CONTEXT(requireEnd());
    }
};

