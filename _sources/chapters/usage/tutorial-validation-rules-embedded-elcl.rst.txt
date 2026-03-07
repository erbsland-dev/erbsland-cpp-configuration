..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: tutorial
    single: validation rules
    single: ELCL
    single: usage

**********************************************
Use Validation-Rules as Embedded ELCL Document
**********************************************

This tutorial shows how you can define validation rules as an embedded
ELCL document and apply them to a parsed configuration.

This approach keeps the rules readable and close to the configuration
syntax itself, while still allowing you to embed everything directly
into your C++ application.

.. important::

    Before using validation rules, link exactly one validation-rules
    library variant as described in
    :doc:`integration-submodule-validation-rules`.

When to Use This Approach
=========================

Embedded ELCL validation documents are ideal for quick starts and
moderate rule sets.

.. list-table::
    :widths: 1 99

    *   -   :fas:`plus;sd-text-success`
        -   Small embedded rule sets are easy to read and maintain.
    *   -   :fas:`plus;sd-text-success`
        -   Rules stay close to the configuration format they validate.
    *   -   :fas:`minus;sd-text-warning`
        -   Less suitable for very large or highly dynamic rule sets.
    *   -   :fas:`minus;sd-text-warning`
        -   Errors in the rules document are detected at runtime, not at compile time.

If your rules are mostly static and maintained by humans, this approach
is often clearer than constructing rules in C++ code.

Step 1: Add the Required Headers
================================

Include both the configuration and validation-rules headers:

.. code-block:: cpp

    #include <erbsland/all_conf.hpp>
    #include <erbsland/all_conf_vr.hpp>

    #include <format>
    #include <iostream>

Step 2: Embed the Rules Document
================================

The rules document is standard ELCL text.
Embed it as a C++ raw string literal and parse it using
:cpp:any:`Parser::parseTextOrThrow() <erbsland::conf::Parser::parseTextOrThrow()>`.

.. code-block:: cpp

    const auto rulesDocument = parser.parseTextOrThrow(R"(
    [server]
    type: "section"

    [server.host]
    type: "text"
    default: "127.0.0.1"

    [server.port]
    type: "integer"
    minimum: 1024
    maximum: 65535

    [server.mode]
    type: "text"
    in: "dev", "prod"
    default: "dev"
    )");

.. important::

    Do not indent the ELCL content inside the raw string literal.
    Leading whitespace becomes part of the text and may change parsing
    behavior.

Step 3: Build and Apply the Rules
=================================

Convert the parsed rules document into a
:cpp:class:`vr::Rules <erbsland::conf::vr::Rules>` instance and validate your configuration document:

.. code-block:: cpp

    using el::conf::vr::Rules;
    const auto rules = Rules::createFromDocument(rulesDocument);

    // Validate configuration for schema version 1
    rules->validate(configDocument, 1);

During validation, the library:

* Verifies structure (sections and keys)
* Checks value types
* Applies constraints (e.g., ``minimum``, ``maximum``, ``in``)
* Inserts default values where defined

If validation fails, an :cpp:class:`Error <erbsland::conf::Error>` with category
:cpp:any:`Validation <erbsland::conf::ErrorCategory::Enum::Validation>` is thrown.

Complete Example
================

.. code-block:: cpp

    #include <erbsland/all_conf.hpp>
    #include <erbsland/all_conf_vr.hpp>

    #include <format>
    #include <iostream>

    namespace el = erbsland;

    int main() {
        try {
            el::conf::Parser parser;

            const auto configDocument = parser.parseTextOrThrow(R"(
    [server]
    port: 8443
            )");

            const auto rulesDocument = parser.parseTextOrThrow(R"(
    [server]
    type: "section"

    [server.host]
    type: "text"
    default: "127.0.0.1"

    [server.port]
    type: "integer"
    minimum: 1024
    maximum: 65535

    [server.mode]
    type: "text"
    in: "dev", "prod"
    default: "dev"
            )");

            const auto rules =
                el::conf::vr::Rules::createFromDocument(rulesDocument);

            rules->validate(configDocument, 1);

            const auto host =
                configDocument->getOrThrow<std::string>("server.host");
            const auto port =
                configDocument->getOrThrow<int>("server.port");
            const auto mode =
                configDocument->getOrThrow<std::string>("server.mode");

            std::cout << std::format(
                "host={} port={} mode={}\n",
                host, port, mode);

            return 0;
        } catch (const el::conf::Error &error) {
            std::cerr << error.toText().toCharString() << "\n";
            return 1;
        }
    }

Try a Failing Input
===================

Change the port in the configuration to ``80`` and run validation again.

Because the rule defines ``minimum: 1024``, validation will fail and
throw an error describing the violated constraint.

This is a good way to verify that your rules behave exactly as intended.

