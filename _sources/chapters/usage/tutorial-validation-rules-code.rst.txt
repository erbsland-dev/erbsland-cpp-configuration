..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: tutorial
    single: validation rules
    single: RulesBuilder
    single: usage

************************************
Use Validation-Rules Built from Code
************************************

This tutorial shows how you can define validation rules directly in C++
using :cpp:class:`vr::RulesBuilder <erbsland::conf::vr::RulesBuilder>`
instead of loading them from a validation-rules ELCL document.

This approach gives you full control at compile time and integrates
naturally into modern C++ workflows.

.. important::

    Before using validation rules, link exactly one validation-rules
    library variant as described in
    :doc:`integration-submodule-validation-rules`.

When to Use This Approach
=========================

Building rules in C++ is a good alternative to embedding an ELCL rules
document into your application.

.. list-table::
    :widths: 1 99

    *   -   :fas:`plus;sd-text-success`
        -   Rules constructed in code are created faster at runtime.
    *   -   :fas:`plus;sd-text-success`
        -   Code can be generated automatically using the
            ``erbsland-conf-vr-to-cpp`` tool.
    *   -   :fas:`plus;sd-text-success`
        -   You can construct rules dynamically based on compile-time or runtime conditions.
    *   -   :fas:`minus;sd-text-warning`
        -   Without an ELCL-based workflow, rules defined purely in C++ can
            become harder to read and maintain as they grow.

As a rule of thumb:
If rules are primarily static and maintained by humans, ELCL is often
clearer. If rules are generated, derived, or tightly integrated into C++,
building them in code can be the better choice.

Step 1: Build the Rule Set
==========================

:cpp:class:`vr::RulesBuilder <erbsland::conf::vr::RulesBuilder>` collects
rule definitions and validates the internal structure when you call
:cpp:any:`takeRules()<erbsland::conf::vr::RulesBuilder::takeRules()>`.
If the rule graph is inconsistent or incomplete, an exception is thrown
at that point.

.. rubric:: Example:

.. code-block:: cpp

    [[nodiscard]] auto createRules() -> el::conf::vr::RulesPtr {
        using namespace el::conf::vr;
        using namespace el::conf::vr::builder;

        RulesBuilder rulesBuilder;

        rulesBuilder.addRule("server", RuleType::Section);

        rulesBuilder.addRule("server.host",
            RuleType::Text,
            Default("127.0.0.1"));

        rulesBuilder.addRule("server.port",
            RuleType::Integer,
            Minimum(1024),
            Maximum(65535));

        rulesBuilder.addRule("server.mode",
            RuleType::Text,
            In({"dev", "prod"}),
            Default("dev"));

        return rulesBuilder.takeRules();
    }

In this example you define:

* A ``server`` section
* A ``host`` text value with a default
* A ``port`` integer constrained to a valid non-privileged range
* A ``mode`` value restricted to ``dev`` or ``prod``

Step 2: Parse and Validate Configuration
========================================

After creating the rules, parse your configuration and validate it
against the rule set.

.. code-block:: cpp

    // Usually read from a file. Embedded here for demonstration.
    const auto configDocument = parser.parseTextOrThrow(R"(
    [server]
    port: 8080
    )");

    const auto rules = createRules();

    // Validate configuration for schema version 1
    rules->validate(configDocument, 1);

Validation performs:

* Structural checks (sections and keys)
* Type validation
* Constraint checks (e.g., ``Minimum``, ``Maximum``, ``In``)
* Default value insertion where applicable

If validation fails, an exception derived from ``Error`` is thrown.

Complete Example
================

.. code-block:: cpp

    #include <erbsland/all_conf.hpp>
    #include <erbsland/all_conf_vr.hpp>

    #include <format>
    #include <iostream>

    using namespace el::conf;

    [[nodiscard]] auto createRules() -> el::conf::vr::RulesPtr {
        using namespace el::conf::vr;
        using namespace el::conf::vr::builder;

        RulesBuilder rulesBuilder;

        rulesBuilder.addRule("server", RuleType::Section);

        rulesBuilder.addRule("server.host",
            RuleType::Text,
            Default("127.0.0.1"));

        rulesBuilder.addRule("server.port",
            RuleType::Integer,
            Minimum(1024),
            Maximum(65535));

        rulesBuilder.addRule("server.mode",
            RuleType::Text,
            In({"dev", "prod"}),
            Default("dev"));

        return rulesBuilder.takeRules();
    }

    int main() {
        try {
            Parser parser;

            const auto configDocument = parser.parseTextOrThrow(R"(
    [server]
    port: 8080
            )");

            const auto rules = createRules();
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
        } catch (const Error &error) {
            std::cerr << error.toText().toCharString() << "\n";
            return 1;
        }
    }

.. note::

    For simplicity, this example uses
    ``using namespace el::conf;``.

    Avoid importing namespaces into the global namespace in production
    code. Prefer local imports inside function or class scopes, or import only specific symbols.

