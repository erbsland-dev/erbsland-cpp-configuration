..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: integration
    single: submodule
    single: validation rules
    single: usage

******************************************
Integrate Validation Rules as a Submodule
******************************************

This page extends the parser-only setup from :doc:`integration-submodule`.
The parser target is still the base dependency, while validation rules are added by linking one of the dedicated validation-rules targets.

Validation-rules targets are excluded from the default build and are only compiled when explicitly requested by linking or by building the target directly.

Choose a Validation-Rules Target
================================

Select exactly one variant:

* ``erbsland-configuration-vr-re-disabled``: validation rules without ``matches`` support.
* ``erbsland-configuration-vr-re-std``: validation rules with ``matches`` using ``std::regex``.
* ``erbsland-configuration-vr-re-erbsland``: validation rules with ``matches`` using ``erbsland-re`` (the *Erbsland Regular Expression* library).

If you plan to use ``matches`` constraints with regular expressions, we recommend using the `Erbsland Regular Expression <https://re.erbsland.dev>`_ library for stability and safety. To reduce dependencies, using ``std::regex`` is an option, but the support and stability of this regular expression implementation strongly depends on the used platform, compiler and standard library.

If you do not plan to use the ``matches`` constraint, disabling this functionality is a safe choice.

Update Your CMake Target
========================

Keep your top-level setup from :doc:`integration-submodule` and only extend the application target link libraries:

.. code-block:: cmake
    :caption: <project>/example/CMakeLists.txt

    cmake_minimum_required(VERSION 3.25)
    project(Example)

    add_executable(example
        src/main.cpp
    )

    target_compile_features(example PRIVATE cxx_std_20)
    target_link_libraries(example PRIVATE
        erbsland-configuration-parser
        erbsland-configuration-vr-re-std
    )

How to Compile
==============

The build flow stays the same:

.. code-block:: console

    $ mkdir cmake-build
    $ cmake . -B cmake-build
    $ cmake --build cmake-build

.. card:: Faster Builds Using Ninja

    For faster and more reliable incremental builds, we recommend using *Ninja* as your build tool on all platforms.

    .. code-block:: console

        $ mkdir cmake-build
        $ cmake . -G Ninja -B cmake-build
        $ cmake -B cmake-build -j 8

Minimal Usage Example
=====================

.. code-block:: cpp

    #include <erbsland/all_conf.hpp>
    #include <erbsland/all_conf_vr.hpp>

    int main() {
        using namespace el::conf;

        Parser parser;

        const auto configDoc = parser.parseFileOrThrow(String{"config.elcl"});
        const auto rulesDoc = parser.parseFileOrThrow(String{"validation-rules.elcl"});

        const auto rules = vr::Rules::createFromDocument(rulesDoc);
        rules->validate(configDoc, 1); // validate for version 1
    }

For practical examples, continue with:

* :doc:`tutorial-validation-rules-embedded-elcl`
* :doc:`tutorial-validation-rules-code`

For API details, see :doc:`../reference/validation_rules`.
