# Erbsland Configuration Parser for C++

This project provides a full-featured, dependency-free implementation of the [Erbsland Configuration Language](https://erbsland-dev.github.io/erbsland-lang-config-doc/) for modern C++ (C++20 and newer).

## Project Status

- Fully implements the [Erbsland Configuration Language](https://erbsland-dev.github.io/erbsland-lang-config-doc/).
- Fully implements validation-rules, created via an ELCL document and code-based.
- Thoroughly tested and stable for use in productive development.
- The API is stable.
- Complete documentation available.

[![CI](https://github.com/erbsland-dev/erbsland-cpp-configuration/actions/workflows/ci.yml/badge.svg)](https://github.com/erbsland-dev/erbsland-cpp-configuration/actions/workflows/ci.yml) [![Docs](https://github.com/erbsland-dev/erbsland-cpp-configuration/actions/workflows/build-docs.yml/badge.svg)](https://github.com/erbsland-dev/erbsland-cpp-configuration/actions/workflows/build-docs.yml)

## Quick Start

```cpp
#include <erbsland/all_conf.hpp>

#include <filesystem>
#include <iostream>
#include <format>

using namespace el::conf;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <config-file>\n";
        return 1;
    }

    const auto configFile = std::filesystem::path{argv[1]};
    try {
        Parser parser;
        const auto source = Source::fromFile(configFile);
        const auto doc = parser.parseOrThrow(source);

        auto width = doc->getIntegerOrThrow(u8"field.width");
        auto height = doc->getIntegerOrThrow(u8"field.height");

        std::cout << std::format("Field width = {}, height = {}\n", width, height);
        return 0;
    } catch (const Error &error) {
        std::cerr << error.toText().toCharString() << "\n";
        return 1;
    }
}
```

## Documentation

Please read the documentation for more information about the parser and its features:

👉 [Erbsland Configuration Parser Documentation](https://cpp-configuration.erbsland.dev)

## About the Erbsland Configuration Language

The *Erbsland Configuration Language* is a human-friendly format designed for writing clear and structured configuration files. It combines a strict, well-defined syntax with flexibility in formatting, making configurations easier to read and maintain.

Here’s an example of a configuration file in a more descriptive style:

```text
# Comments are allowed almost everywhere.
---[ Main Settings ] -------------------- # A section 
App Name : "ELCL Demo"                    # Name-Value Pair with Text Value
Version  : 1                              # Name-Value Pair with Integer Value
```

And the same data in a minimal style:

```text
[main_settings] 
app_name: "ELCL Demo"
version: 1
```

Supported data types include text, integer, floating-point, boolean, date, time, datetime, time-delta, regular expressions, code, and byte sequences. These can be grouped into sections, nested via name paths, or organized into lists.

A detailed language specification is available here:

👉 [Erbsland Configuration Language Documentation](https://erbsland-dev.github.io/erbsland-lang-config-doc/)

## Requirements

- A C++20-compliant compiler (clang, gcc, or MSVC)
- CMake 3.23 or higher

## License

Copyright © 2026 Tobias Erbsland – https://erbsland.dev/

Licensed under the **Apache License, Version 2.0**.
You may obtain a copy at: http://www.apache.org/licenses/LICENSE-2.0
Distributed on an “AS IS” basis, without warranties or conditions of any kind. See the LICENSE file for full details.

