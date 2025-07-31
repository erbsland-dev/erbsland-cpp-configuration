
.. index::
    single: integration
    single: submodule
    single: build
    single: usage

*********************************
Integrate the Parser as Submodule
*********************************

The recommended way to include the Erbsland Configuration Parser in your project is by adding it as a Git submodule. This approach is especially suitable for new projects, as it allows you to lock the parser to a specific version. You can then manually update and test new versions when needed, maintaining better control over dependencies.

We also strongly recommend integrating the parser as a **static library**, rather than using dynamic linking. This avoids compatibility issues and simplifies distribution and deployment.

Project Structure
=================

A typical project layout using the parser as a submodule might look like this:

.. code-block:: none

    <project root>                       # Your project root directory
        ├── erbsland-cpp-configuration   # The Erbsland Configuration Parser as a Git submodule
        │   └── (...)
        ├── <project>                    # Your application source directory
        │   ├── src                      # Application source files
        │   │   └── (...)
        │   └── CMakeLists.txt           # CMake build file for your app
        ├── (...)
        └── CMakeLists.txt               # Top-level CMake configuration file

The CMake Configuration
=======================

Below is a minimal example of how to configure your project using CMake:

.. code-block:: cmake
    :caption: <project>/CMakeLists.txt

    cmake_minimum_required(VERSION 3.25)
    project(ExampleProject)

    add_subdirectory(erbsland-cpp-configuration)
    add_subdirectory(example)

.. code-block:: cmake
    :caption: <project>/example/CMakeLists.txt

    cmake_minimum_required(VERSION 3.25)
    project(Example)

    add_executable(example
        src/main.cpp
    )

    target_compile_features(example PRIVATE cxx_std_20)
    target_link_libraries(example PRIVATE erbsland-configuration-parser)

How to Compile
==============

To build your project with this setup, run the following commands from your project root:

.. code-block:: console

    $ mkdir cmake-build
    $ cmake . -B cmake-build
    ...
    -- Build files have been written to: ~/example/cmake-build

    $ cmake --build cmake-build
    [  1%] Building CXX object TokenDecoder.cpp.o
    [  3%] Building CXX object Core.cpp.o
    ...
    [100%] Linking CXX executable example
    [100%] Built target example

