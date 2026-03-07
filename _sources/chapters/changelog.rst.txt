..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: Changelog
    single: Changes

*********
Changelog
*********

Version 1.3.0 — 2026-02-28
==========================

This release introduces full support for validation rules in the configuration parser.

The validation-rules implementation is intentionally separated from the core parser. It is only compiled if your application links against one of the dedicated validation-rule targets. This keeps the base parser lightweight while allowing you to enable validation when needed.

See the validation-rules documentation for details and usage examples.

Main Changes
------------

*   Added the new namespace ``erbsland::conf::vr`` containing all validation-rule interfaces.
*   Extended the documentation:

    -   Added a complete validation-rules reference.
    -   Added tutorials demonstrating how to use validation rules.
    -   Modernized the documentation build dependencies and configuration.

*   Extended the CMake build system:

    -   Added three validation-rule variants:

        -   Without regular expression support
        -   Using ``std::regex``
        -   Using the *Erbsland Regular Expression* library

    -   Added an installation workflow for static library builds.

API Changes Since Version 1.2
-----------------------------

Added Public Types and Headers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*   ``CaseSensitivity``
*   ``Matrix<T>``
*   ``StringList``
*   ``ValueList``, ``ConstValueList``
*   ``ValueMatrix``, ``ConstValueMatrix``
*   ``StringConvertible`` and ``StringLike`` concepts

Extended ``Value`` API
~~~~~~~~~~~~~~~~~~~~~~

*   Added:

    -   ``wasValidated()``
    -   ``validationRule()``
    -   ``isSecret()``
    -   ``isDefaultValue()``

*   Added matrix helpers:

    -   ``asMatrix()``, ``asMatrixOrThrow()``
    -   ``getMatrix()``, ``getMatrixOrThrow()``

*   Added conversions:

    -   ``toValueList()`` (const and non-const)
    -   ``toValueMatrix()`` (const and non-const)

*   Extended text getter templates to accept string-like defaults (``String``, ``std::string``, ``std::u8string``)

``ValueList`` Semantics
~~~~~~~~~~~~~~~~~~~~~~~

*   ``ValueList`` is now defined as ``std::vector<ValuePtr>`` (mutable pointers).
*   Added ``ConstValueList`` defined as ``std::vector<ConstValuePtr>``.

Extended ``Parser`` Convenience API
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*   ``parseFileOrThrow(...)``, ``parseFile(...)``
*   ``parseTextOrThrow(...)``, ``parseText(...)``

Extended ``Name`` and ``NamePath``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*   ``Name``:

    -   ``isReservedValidationRule()``
    -   ``isEscapedReservedValidationRule()``
    -   ``withReservedVRPrefixRemoved()``
    -   ``emptyInstance()``

*   ``NamePath``:

    -   ``Index``, ``Count``, ``npos``
    -   ``find()``, ``containsText()``
    -   ``subPath()``, ``popBack()``, ``clear()``
    -   ``NamePathList``

Extended ``String`` API (Unicode-Aware)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*   ``isValidUtf8()``
*   ``characterLength()``
*   ``characterCompare()``
*   ``nameCompare()``
*   ``startsWith()``, ``contains()``, ``endsWith()`` with ``CaseSensitivity``
*   ``split()``, ``join()``, ``transformed()``
*   ``forEachCharacter()``
*   ``toSafeText()``
*   Added ``char`` and ``char32_t`` append overloads

Extended ``Error``
~~~~~~~~~~~~~~~~~~

*   Added copy-and-modify helpers:

    -   ``withNamePathAndLocation()``
    -   ``withMessagePrefix()``
    -   ``withMessage()``

Extended ``Bytes``
~~~~~~~~~~~~~~~~~~

*   Added ``toHexForErrors()``.

Extended ``ValueType``
~~~~~~~~~~~~~~~~~~~~~~

*   Added ``isStructural()``
*   Added ``isScalar()``
*   Added ``toValueDescription(bool)``
*   ``isSingle()`` is now deprecated (use ``isScalar()``)

Extended ``ValueIterator``
~~~~~~~~~~~~~~~~~~~~~~~~~~

*   Now supports bidirectional iteration:

    -   ``operator--()``
    -   ``operator--(int)``

Changed ``RegEx`` API
~~~~~~~~~~~~~~~~~~~~~

*   Constructor now takes multiline state: ``RegEx(String text, bool multiLine)``
*   Added ``isMultiLine()``
*   Equality comparison now includes multiline state.

Minor Signature Change
~~~~~~~~~~~~~~~~~~~~~~

*   ``Signer`` constructor now takes ``SignatureSignerPtr`` by value.


Version 1.2.0 — 2025-08-18
==========================

This release extended the public API and resolved several issues.


Version 1.0.0 — 2025-07-31
==========================

First production release of the *Erbsland Configuration Parser for C++*.
