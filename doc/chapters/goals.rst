..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    !single: Goals

*****
Goals
*****

.. grid:: 2

    .. grid-item-card:: :fas:`link-slash;sd-text-success` Minimal Dependencies
        :shadow: md

        Only requires C++20 and the standard library—nothing else.

    .. grid-item-card:: :fas:`code;sd-text-success` Expressive Interface
        :shadow: md

        Reduces boilerplate in user code, making integration clean and concise.

Overview
========

This C++ parser serves as the **reference implementation** for the *Erbsland Configuration Language* (ELCL). Its primary goals are security, robustness, and full feature coverage—offering a clean, understandable codebase that serves both as a production-ready parser and as a guide for implementors of other ELCL parsers.

Full Feature Coverage
=====================

The parser supports the entire ELCL specification, including:

* Core features such as names, values, and sections
* Standard features like multiline values, byte counts, and value lists
* Advanced types such as regular expressions, code blocks, and time deltas

As the reference implementation, it ensures compatibility with all language features and serves as the basis for validating conformance through the official test suite.

Secure by Design
================

Security was a core focus from the start:

* The parser is implemented in **plain, explicit C++**, avoiding code generation or macro-based lexers.
* Input handling and buffer processing are written defensively, with **comprehensive error checks**.
* It is designed to safely handle data from untrusted sources, making it suitable for embedded, backend, or toolchain usage.

This approach ensures predictable behavior and minimizes the risk of memory or parsing vulnerabilities.

Dependency-Free
===============

The parser is intentionally designed with **zero external dependencies**:

* No third-party libraries are required.
* Self-contained implementations are included for tasks such as:
  - Unicode-aware string handling
  - Basic date/time parsing
  - Value formatting and escaping

This makes the parser **easy to integrate** into any C++ project and ideal as a portable reference for reimplementations in other programming languages.

Modern, Readable C++
====================

Written in modern C++ (up to C++20), the codebase is:

* Compatible with a wide range of C++ compilers
* Cleanly structured for readability and extensibility
* Explicit rather than clever—emphasizing clarity over abstraction

You should be able to understand and extend the parser without reverse-engineering dense templates or macro magic.

Robustness Over Performance
===========================

Parsing configuration files is rarely a performance bottleneck, but **robust behavior** is critical:

* The parser prioritizes **correctness, consistency, and transparency** over raw speed.
* Error reporting is designed to help users quickly find and fix mistakes in their configuration files.
* Performance optimizations are applied where helpful, but never at the cost of readability or correctness.

This focus ensures the parser remains a trustworthy building block for your systems.
