
# Erbsland Configuration Parser for C++

This project provides a full-featured, dependency-free implementation of the [Erbsland Configuration Language](https://erbsland-dev.github.io/erbsland-lang-config-doc/) for modern C++ (C++20 and newer).

## Project Status

*Pre-release notice:*  
We are preparing for the 1.0.0 release, planned for August 2025.

- Fully implements the [Erbsland Configuration Language](https://erbsland-dev.github.io/erbsland-lang-config-doc/).
- Thoroughly tested and stable for use in active development.
- The API is considered stable. No breaking changes are expected before the 1.0.0 release.

## Language Conformance

This parser serves as the reference implementation for the Erbsland Configuration Language. It follows the official specification without deviations.

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

A detailed guide and specification are available here:

👉 [Erbsland Configuration Language Documentation](https://erbsland-dev.github.io/erbsland-lang-config-doc/)

## Requirements

- A C++20-compliant compiler (clang, gcc, or MSVC)
- CMake 3.23 or higher

## Contributing, Bugs & Feedback

We welcome your feedback, ideas, and reports. If you encounter any issues or would like to request a feature:

▶️ [Submit an Issue](https://github.com/erbsland-dev/erbsland-cpp-configuration/issues/)

## License

Copyright © 2025 Tobias Erbsland – https://erbsland.dev/

Licensed under the **Apache License, Version 2.0**.
You may obtain a copy at: http://www.apache.org/licenses/LICENSE-2.0
Distributed on an “AS IS” basis, without warranties or conditions of any kind. See the LICENSE file for full details.

