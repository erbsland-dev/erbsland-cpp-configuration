..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******
Parser
******

Usage
=====

Minimal Example
---------------

.. code-block:: cpp
    :linenos:

    try {
        el::conf::Parser parser;
        auto document = parser.parseOrThrow(Source::fromFile(u8"configuration.elcl"))
        // ...
    } catch (const el::conf::Error &error) {
        std::cerr << error.toText().toCharString() << "\n";
        exit(1);
    }

Without Exceptions
------------------

.. code-block:: cpp
    :linenos:

    el::conf::Parser parser;
    auto document = parser.parse(Source::fromFile(u8"configuration.elcl"))
    if (document == nullptr) {
        std::cerr << parser.lastError().toText().toCharString() << "\n";
        exit(1);
    }

With Customized Behaviour
-------------------------

.. code-block:: cpp

    try {
        el::conf::Parser parser;
        parser.setSourceResolver(std::make_shared<MySourceResolver>());
        parser.setAccessCheck(std::make_shared<MyAccessCheck>());
        parser.setSignatureValidator(std::make_shared<MySignatureValidator>());
        auto document = parser.parseOrThrow(Source::fromFile(u8"configuration.elcl"))
        // ...
    } catch (const el::conf::Error &error) {
        std::cerr << error.toText().toCharString() << "\n";
        exit(1);
    }

See :cpp:class:`AccessCheck<erbsland::conf::AccessCheck>`, :cpp:class:`SourceResolver<erbsland::conf::SourceResolver>` and :cpp:class:`SignatureValidator<erbsland::conf::SignatureValidator>` for details.

Interface
=========

.. doxygenclass:: erbsland::conf::Parser
    :members:

