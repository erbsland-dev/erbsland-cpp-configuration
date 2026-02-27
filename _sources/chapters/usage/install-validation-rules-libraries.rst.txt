..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: install
    single: build
    single: validation rules
    single: static library
    single: usage

*****************************************
Install Validation-Rules Static Libraries
*****************************************

The default installation flow described in :doc:`install-parser-as-library` installs:

* The parser static library
* The public header files
* The exported CMake package configuration

If you also want validation-rules support in your installation, configure
which validation-rules variant should be installed. During installation,
CMake will:

* Build the selected static library
* Install it together with the parser library
* Export its CMake target alongside the parser target

Configure the Validation-Rules Variant
======================================

Select the validation-rules variant during CMake configuration using
``ERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT``.

Example (``std::regex`` variant):

.. code-block:: console

    $ git clone https://github.com/erbsland-dev/erbsland-cpp-configuration.git
    $ mkdir build
    $ cd build
    $ cmake ../erbsland-cpp-configuration -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DERBSLAND_CONFIGURATION_INSTALL_VR_VARIANT=re-std

Available values:

.. list-table::
    :width: 100%

    *   -   ``none``
        -   Default. Install the parser only.
    *   -   ``re-disabled``
        -   Install ``erbsland-configuration-vr-re-disabled`` (no ``matches`` support).
    *   -   ``re-std``
        -   Install ``erbsland-configuration-vr-re-std`` (``matches`` using ``std::regex``).
    *   -   ``re-erbsland``
        -   Install ``erbsland-configuration-vr-re-erbsland`` (``matches`` using ``erbsland-re``).
    *   -   ``all``
        -   Install all validation-rules variants.

Unless you are packaging for distribution, we strongly recommend selecting
**exactly one** variant in consumer projects.

Choosing the Right Variant
--------------------------

If you plan to use ``matches`` constraints with regular expressions, we
recommend the `Erbsland Regular Expression <https://re.erbsland.dev>`_
library. It is designed for stability, safety, and predictable behavior across
platforms.

Using ``std::regex`` reduces external dependencies, but keep in mind:

* Behavior may differ between platforms.
* Stability and performance depend heavily on the compiler and standard library implementation.
* Edge-case handling is not consistent across environments.

If you do **not** plan to use the ``matches`` constraint, selecting
``re-disabled`` is a safe and dependency-free choice.

Build and Install
=================

After configuration, build your project as usual and run the install step:

.. code-block:: console

    $ cmake --build .
    $ cmake --install . --prefix /usr/local

This installs:

* ``liberbsland-configuration-parser.a``
* The selected validation-rules archive
  (for example ``liberbsland-configuration-vr-re-std.a``)
* Parser and validation-rules headers required by the public API
* CMake package configuration files exporting both targets

Link from a Consumer Project
============================

The installed CMake package exports both the parser and the selected
validation-rules target.

Link exactly one validation-rules target:

.. code-block:: cmake

    find_package(erbsland-configuration-parser REQUIRED)

    add_executable(example src/main.cpp)
    target_compile_features(example PRIVATE cxx_std_20)
    target_link_libraries(example PRIVATE
        ErbslandDEV::erbsland-configuration-vr-re-std
    )

The validation-rules target links the parser target transitively, so you do
not need to link ``ErbslandDEV::erbsland-configuration-parser`` separately.

.. note::

    For most projects, we recommend direct source integration using a Git
    submodule. See :doc:`integration-submodule-validation-rules` for details.
