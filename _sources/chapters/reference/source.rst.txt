..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******
Source
******

Usage
=====

.. code-block:: cpp
    :linenos:

    try {
        auto source = Source::fromFile(u8"configuration.elcl");
        el::conf::Parser parser;
        auto document = parser.parseOrThrow(source)
        // ...
    } catch (const el::conf::Error &error) {
        std::cerr << error.toText().toCharString() << "\n";
        exit(1);
    }

.. code-block:: cpp
    :linenos:

    try {
        auto source = Source::fromText(
            u8"[main]\n"
            u8"value = 12\n");
        el::conf::Parser parser;
        auto document = parser.parseOrThrow(source)
        // ...
    } catch (const el::conf::Error &error) {
        std::cerr << error.toText().toCharString() << "\n";
        exit(1);
    }


Interface
=========

.. doxygenclass:: erbsland::conf::Source
    :members:

.. doxygenclass:: erbsland::conf::SourceIdentifier
    :members:

.. doxygentypedef:: erbsland::conf::SourcePtr

.. doxygentypedef:: erbsland::conf::SourcePtrConst

.. doxygentypedef:: erbsland::conf::SourceList

.. doxygentypedef:: erbsland::conf::SourceListPtr

.. doxygentypedef:: erbsland::conf::SourceIdentifierPtr

