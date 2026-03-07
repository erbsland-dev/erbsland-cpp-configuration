..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: build
    single: install
    single: usage

************************************
Install the Parser as Static Library
************************************

You can build and install the Erbsland Configuration Parser as a standalone static library.
This approach is useful if you:

* Want to reuse the parser across multiple independent projects.
* Prefer a system-wide installation.
* Integrate it into an existing build or packaging workflow.

For most use cases, however, we recommend integrating the parser as a Git submodule directly into your project. This gives you tighter version control, reproducible builds, and simpler dependency management. See :doc:`integration-submodule` for details.

.. note::

    This page describes the default **parser-only** installation.
    If you also need a validation-rules static library, see
    :doc:`install-validation-rules-libraries`.

Build and Install
=================

Follow these steps to clone, build, and install the library:

.. code-block:: console

    $ git clone https://github.com/erbsland-dev/erbsland-cpp-configuration.git
    $ mkdir build
    $ cd build
    $ cmake ../erbsland-cpp-configuration -DCMAKE_BUILD_TYPE=Release
    -- The CXX compiler identification is (...)
    (...)
    -- Build files have been written to: (...)/build

    $ cmake --build .
    [  1%] Building CXX object (...)/TokenDecoder.cpp.o
    [  3%] Building CXX object (...)/Core.cpp.o
    (...)
    [100%] Linking CXX static library liberbsland-configuration-parser.a
    [100%] Built target erbsland-configuration-parser

    $ cmake --install .
    -- Install configuration: "Release"
    -- Installing: (...)/lib/liberbsland-configuration-parser.a
    -- Installing: (...)/include
    (...)

After installation, you will have:

* The static library ``liberbsland-configuration-parser.a``.
* All public header files in the install include directory.
* An exported CMake package configuration for ``find_package``.

By default, this installation does **not** include static libraries for
validation rules. If you need one, refer to
:doc:`install-validation-rules-libraries`.

Installation Prefix
-------------------

By default, CMake installs to a platform-specific location:

* ``/usr/local`` on most Unix-like systems
* A platform-dependent directory on Windows

If you want to control the install location, set the prefix explicitly:

.. code-block:: console

    $ cmake --install . --prefix /your/custom/path

Alternatively, you can define ``CMAKE_INSTALL_PREFIX`` during configuration:

.. code-block:: console

    $ cmake ../erbsland-cpp-configuration \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/your/custom/path

.. card:: Faster Builds Using Ninja

    For faster and more reliable incremental builds, we recommend using *Ninja* as your build tool on all platforms.

    .. code-block:: console

        $ git clone https://github.com/erbsland-dev/erbsland-cpp-configuration.git
        $ mkdir build
        $ cd build
        $ cmake ../erbsland-cpp-configuration -G Ninja -DCMAKE_BUILD_TYPE=Release
        $ cmake --build . -j 8

Linking from a Consumer Project
===============================

The installation exports a CMake package that provides the parser target.

In your consuming project:

.. code-block:: cmake

    find_package(erbsland-configuration-parser REQUIRED)

    add_executable(example src/main.cpp)
    target_compile_features(example PRIVATE cxx_std_20)
    target_link_libraries(example PRIVATE
        ErbslandDEV::erbsland-configuration-parser
    )

This ensures:

* Proper include directories.
* Correct compile definitions.
* Clean and modern CMake-based integration.

If you prefer source-based integration with exported CMake targets for both the parser and validation rules, follow the submodule workflow described in :doc:`integration-submodule`.
