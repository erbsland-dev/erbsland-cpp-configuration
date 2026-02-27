// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "AssignmentStreamHelper.hpp"


TESTED_TARGETS(AssignmentStream)
class AssignmentStreamSectionListTest final : public UNITTEST_SUBCLASS(AssignmentStreamHelper) {
public:
    void testSectionLists() {
        WITH_CONTEXT(setupAssignmentStream("section_lists.elcl"));
        WITH_CONTEXT(requireSectionList(u8"server"));
        WITH_CONTEXT(requireValue(u8"server.value", ValueType::Integer, 1));
        WITH_CONTEXT(requireSectionList(u8"server"));
        WITH_CONTEXT(requireValue(u8"server.value", ValueType::Integer, 2));
        WITH_CONTEXT(requireSectionList(u8"server"));
        WITH_CONTEXT(requireValue(u8"server.value", ValueType::Integer, 3));
        WITH_CONTEXT(requireSectionList(u8"client.config"));
        WITH_CONTEXT(requireValue(u8"client.config.value", ValueType::Integer, 1));
        WITH_CONTEXT(requireSectionList(u8"client.config"));
        WITH_CONTEXT(requireValue(u8"client.config.value", ValueType::Integer, 2));
        WITH_CONTEXT(requireSectionList(u8"client.config"));
        WITH_CONTEXT(requireValue(u8"client.config.value", ValueType::Integer, 3));
    }
};

