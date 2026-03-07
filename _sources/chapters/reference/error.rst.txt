..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****
Error
*****

Usage
=====

.. code-block:: cpp
    :linenos:

    throw Error(ErrorCategory::Syntax, u8"my message");

.. code-block:: cpp
    :linenos:

    auto location = ...
    throw Error(ErrorCategory::Syntax, u8"my message", location);

.. code-block:: cpp
    :linenos:

    try {
        // ...
    } catch (const std::system_error &error) {
        throw Error(ErrorCategory::IO, u8"System error", error.code());
    }

.. code-block:: cpp
    :linenos:

    Location location = ...
    NamePath namePath = ...
    std::filesystem::path filePath = ...
    std::error_code errorCode = ...
    throw Error(ErrorCategory::Syntax, u8"my message", location, namePath, filePath, errorCode);

.. code-block:: cpp
    :linenos:

    try {
        // ... parse document, etc ...
    } catch (const Error &error) {
        std::cerr << error.toText().toCharString() << "\n";
        exit(1);
    }

Interface
=========

.. doxygenclass:: erbsland::conf::Error
    :members:

.. doxygenclass:: erbsland::conf::ErrorCategory
    :members:

