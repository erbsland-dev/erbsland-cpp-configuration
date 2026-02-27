..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    !single: usage

*****
Usage
*****

.. toctree::
    :maxdepth: 3
    :hidden:

    integration-submodule
    integration-submodule-validation-rules
    install-parser-as-library
    install-validation-rules-libraries
    tutorial-validation-rules-embedded-elcl
    tutorial-validation-rules-code
    run-unit-tests

This chapter shows you how to work with the *Erbsland Configuration Parser* in real-world projects.

You’ll learn how to integrate it smoothly into your own codebase, build it as a standalone library if needed, and run the included unit tests to ensure everything works as expected.

.. grid:: 2
    :margin: 4 4 0 0
    :gutter: 1

    .. grid-item-card:: :fas:`code-branch;sd-text-success` Integrate as Submodule
        :link: integration-submodule
        :link-type: doc

        Learn the recommended way to include the parser as a Git submodule for seamless project integration and version control.

    .. grid-item-card:: :fas:`sliders-h;sd-text-success` Add Validation Rules (Submodule)
        :link: integration-submodule-validation-rules
        :link-type: doc

        Extend the recommended submodule setup to compile and link one of the validation-rules library variants.

    .. grid-item-card:: :fas:`file-code;sd-text-success` Tutorial: Embedded ELCL Rules
        :link: tutorial-validation-rules-embedded-elcl
        :link-type: doc

        Learn how to embed a validation-rules ELCL document directly into your application and validate configuration documents.

    .. grid-item-card:: :fas:`hammer;sd-text-success` Tutorial: Code-Built Rules
        :link: tutorial-validation-rules-code
        :link-type: doc

        Build validation rules with ``RulesBuilder`` in C++ code and apply them to parsed configuration documents.

    .. grid-item-card:: :fas:`cogs;sd-text-success` Install as a Static Library
        :link: install-parser-as-library
        :link-type: doc

        Prefer a standalone library? This guide walks you through building and installing the parser as a static library.

    .. grid-item-card:: :fas:`download;sd-text-success` Install Validation-Rules Libraries
        :link: install-validation-rules-libraries
        :link-type: doc

        Build validation-rules library variants and install their static archives for local system-wide usage.

    .. grid-item-card:: :fas:`vial;sd-text-success` Run Unit Tests
        :link: run-unit-tests
        :link-type: doc

        Validate your setup and contribute confidently by running the parser’s unit tests.

