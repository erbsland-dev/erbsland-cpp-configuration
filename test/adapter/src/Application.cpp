// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Application.hpp"


#include <erbsland/conf/Parser.hpp>

#include <complex>
#include <string_view>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <stdexcept>


auto Application::run(const int argc, char *argv[]) -> int {
    if (const auto returnCode = parseArguments(argc, argv); returnCode != 0) {
        return returnCode;
    }
    try {
        el::conf::Parser parser;
        auto source = el::conf::Source::fromFile(_argConfigFile);
        auto doc = parser.parseOrThrow(source);
        auto valueMap = doc->toFlatValueMap();
        for (const auto &[namePath, value] : valueMap) {
            std::cout << namePath.toText().toCharString() << " = " << value->toTestText().toCharString() << "\n";
        }
    } catch (const el::conf::Error &error) {
        std::cout << "FAIL = " << error.category().toText().toCharString()
            << "(" << error.toText().toCharString() << ")\n";
        return 1;
    } catch (const std::exception &error) {
        std::cerr << "Unexpected parser exception: " << error.what() << "\n";
        return 2;
    }
    return 0;
}


auto Application::parseArguments(int argc, char *argv[]) -> int {
    std::vector<std::string_view> args(argv + 1, argv + argc);

    auto is_help_flag = [](std::string_view arg) {
        return arg == "--help" || arg == "-h";
    };

    if (std::ranges::any_of(args, is_help_flag)) {
        std::cout << "Test adapter for validating this implementation of the Erbsland Configuration Language parser.\n\n"
            << "Usage: " << argv[0] << " [--help] [--version <lang version>] <configuration file>\n\n"
            << "Options:\n"
            << "  --help                     Displays this help text\n"
            << "  --version <lang version>   Sets the language version to use for parsing\n"
            << "  <configuration file>       An absolute or relative path to configuration file to parse\n";
        exit(0);
    }

    for (std::size_t i = 0; i < args.size(); ++i) {
        auto &arg = args[i];
        if (arg == "--version" || arg.starts_with("--version=")) {
            if (!_argVersion.empty()) {
                std::cerr << "Error: Multiple --version arguments specified.\n";
                return 2;
            }
            if (arg.starts_with("--version=")) {
                _argVersion = arg.substr(10);
            } else {
                if (args.size() <= i + 1) {
                    std::cerr << "Error: Missing argument for --version.\n";
                    return 2;
                }
                _argVersion = args[++i];
            }
            if (_argVersion != "1.0") {
                std::cerr << "Error: Unsupported language version: " << _argVersion << "\n";
                return 2;
            }
        } else if (arg.starts_with("-")) {
            std::cerr << "Error: Unknown argument: " << arg << "\n";
            return 2;
        } else {
            if (!_argConfigFile.empty()) {
                std::cerr << "Error: Multiple configuration files specified.\n";
                return 2;
            }
            _argConfigFile = arg;
            if (!std::filesystem::is_regular_file(_argConfigFile)) {
                std::cerr << "Error: Configuration file does not exist: " << _argConfigFile << "\n";
                return 2;
            }
        }
    }

    if (_argVersion.empty()) {
        _argVersion = "1.0";
    }
    if (_argConfigFile.empty()) {
        std::cerr << "Error: Missing configuration file.\n";
        return 2;
    }

    return 0;
}

