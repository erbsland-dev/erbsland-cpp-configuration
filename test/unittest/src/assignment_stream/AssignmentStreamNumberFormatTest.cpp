// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "AssignmentStreamHelper.hpp"


TESTED_TARGETS(AssignmentStream)
class AssignmentStreamNumberFormatTest final : public UNITTEST_SUBCLASS(AssignmentStreamHelper) {
public:
    void testIntegerFormats() {
        WITH_CONTEXT(setupAssignmentStream("integer_formats.elcl"));
        WITH_CONTEXT(requireSectionMap(u8"decimal"));
        WITH_CONTEXT(requireValue(u8"decimal.value_1", ValueType::Integer, 0));
        WITH_CONTEXT(requireValue(u8"decimal.value_2", ValueType::Integer, 1));
        WITH_CONTEXT(requireValue(u8"decimal.value_3", ValueType::Integer, 1));
        WITH_CONTEXT(requireValue(u8"decimal.value_4", ValueType::Integer, -1));
        WITH_CONTEXT(requireValue(u8"decimal.value_5", ValueType::Integer, 100'000));
        WITH_CONTEXT(requireValue(u8"decimal.value_6", ValueType::Integer, 1'000'000));
        WITH_CONTEXT(requireValue(u8"decimal.value_7", ValueType::Integer, std::numeric_limits<int64_t>::min()));
        WITH_CONTEXT(requireValue(u8"decimal.value_8", ValueType::Integer, std::numeric_limits<int64_t>::max()));

        WITH_CONTEXT(requireSectionMap(u8"hexadecimal"));
        WITH_CONTEXT(requireValue(u8"hexadecimal.value_1", ValueType::Integer, 0));
        WITH_CONTEXT(requireValue(u8"hexadecimal.value_2", ValueType::Integer, 1));
        WITH_CONTEXT(requireValue(u8"hexadecimal.value_3", ValueType::Integer, 1));
        WITH_CONTEXT(requireValue(u8"hexadecimal.value_4", ValueType::Integer, -1));
        WITH_CONTEXT(requireValue(u8"hexadecimal.value_5", ValueType::Integer, 0x1234ABCD));
        WITH_CONTEXT(requireValue(u8"hexadecimal.value_6", ValueType::Integer, std::numeric_limits<int64_t>::min()));
        WITH_CONTEXT(requireValue(u8"hexadecimal.value_7", ValueType::Integer, std::numeric_limits<int64_t>::max()));

        WITH_CONTEXT(requireSectionMap(u8"binary"));
        WITH_CONTEXT(requireValue(u8"binary.value_1", ValueType::Integer, 0));
        WITH_CONTEXT(requireValue(u8"binary.value_2", ValueType::Integer, 1));
        WITH_CONTEXT(requireValue(u8"binary.value_3", ValueType::Integer, 1));
        WITH_CONTEXT(requireValue(u8"binary.value_4", ValueType::Integer, -1));
        WITH_CONTEXT(requireValue(u8"binary.value_5", ValueType::Integer, 0b1000000111111010));
        WITH_CONTEXT(requireValue(u8"binary.value_6", ValueType::Integer, std::numeric_limits<int64_t>::min()));
        WITH_CONTEXT(requireValue(u8"binary.value_7", ValueType::Integer, std::numeric_limits<int64_t>::max()));

        WITH_CONTEXT(requireSectionMap(u8"byte_counts"));
        WITH_CONTEXT(requireValue(u8"byte_counts.value_1", ValueType::Integer, 5'000));
        WITH_CONTEXT(requireValue(u8"byte_counts.value_2", ValueType::Integer, 5'120));
        WITH_CONTEXT(requireValue(u8"byte_counts.value_3", ValueType::Integer, 10'000'000));
        WITH_CONTEXT(requireValue(u8"byte_counts.value_4", ValueType::Integer, 1'000'000'000));
        WITH_CONTEXT(requireValue(u8"byte_counts.value_5", ValueType::Integer, 1'000'000'000'000));
        WITH_CONTEXT(requireValue(u8"byte_counts.value_6", ValueType::Integer, 1'000'000'000'000'000));
    }

    void testFloatFormats() {
        WITH_CONTEXT(setupAssignmentStream("float_formats.elcl"));
        WITH_CONTEXT(requireSectionMap(u8"float"));
        WITH_CONTEXT(requireFloat(u8"float.value_1",  0.0));
        WITH_CONTEXT(requireFloat(u8"float.value_2",  0.0));
        WITH_CONTEXT(requireFloat(u8"float.value_3",  1.0));
        WITH_CONTEXT(requireFloat(u8"float.value_4",  -1.0));
        WITH_CONTEXT(requireFloat(u8"float.value_5",  std::numeric_limits<double>::quiet_NaN()));
        WITH_CONTEXT(requireFloat(u8"float.value_6",  std::numeric_limits<double>::infinity()));
        WITH_CONTEXT(requireFloat(u8"float.value_7",  -std::numeric_limits<double>::infinity()));
        WITH_CONTEXT(requireFloat(u8"float.value_8",  std::numeric_limits<double>::infinity()));
        WITH_CONTEXT(requireFloat(u8"float.value_9",  2937.28301));
        WITH_CONTEXT(requireFloat(u8"float.value_10", 5e-12));
        WITH_CONTEXT(requireFloat(u8"float.value_11", 0.02e+8));
        WITH_CONTEXT(requireFloat(u8"float.value_12", 12e+10));
        WITH_CONTEXT(requireFloat(u8"float.value_13", -12.9));
        WITH_CONTEXT(requireFloat(u8"float.value_14", -8283.9e-5));
        WITH_CONTEXT(requireFloat(u8"float.value_15", 1000000.000001));
    }

};

