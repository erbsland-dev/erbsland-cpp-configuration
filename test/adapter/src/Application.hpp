// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <string>
#include <filesystem>


class Application final {
public:
    Application() = default;
    ~Application() = default;

public:
    auto run(int argc, char *argv[]) -> int;

private:
    auto parseArguments(int argc, char *argv[]) -> int;

private:
    std::string _argVersion;
    std::filesystem::path _argConfigFile;
};