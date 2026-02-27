// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "AssignmentStreamHelper.hpp"


TESTED_TARGETS(AssignmentStream)
class AssignmentStreamTextNameTest final : public UNITTEST_SUBCLASS(AssignmentStreamHelper) {
public:
    void testTextNames() {
        WITH_CONTEXT(setupAssignmentStream("text_name_values.elcl"));
        WITH_CONTEXT(requireSectionMap(u8"text_names"));
        WITH_CONTEXT(requireValue(u8"text_names.\"One\"", ValueType::Integer, 1));
        WITH_CONTEXT(requireValue(u8"text_names.\"  Two  \"", ValueType::Integer, 2));
        WITH_CONTEXT(requireValue(u8"text_names.\"Good Morning!\"", ValueType::Text, String{u8"おはようございます！"}));
        WITH_CONTEXT(requireValue(u8"text_names.\"\\u{1f606}\"", ValueType::Text, String{u8"😆"}));
        WITH_CONTEXT(requireValue(u8"text_names.\"->\\u{1f606}\"", ValueType::Text, String{u8"😆"}));
    }
};

