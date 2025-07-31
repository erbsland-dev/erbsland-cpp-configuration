*********************
The Project Structure
*********************

Organizing Your Project
=======================

The *Erbsland Configuration Parser* is provided as a static library and is intended to be linked directly into your C++ application. How you integrate it will depend on your existing setup—but our recommendation is simple and works well for most projects.

In this tutorial, we’ll use the parser as a Git submodule. This approach makes it easy to manage updates and keeps external dependencies cleanly separated from your own codebase.

By the end of this step, your project structure will look like this:

.. code-block:: none

    robot-escape                         # The project directory
        ├── erbsland-cpp-configuration   # The Erbsland Configuration Parser as a Git submodule
        │   └── (...)
        ├── robot-escape                 # The application we're building in this tutorial
        │   ├── src                      # Application source files
        │   │   ├── main.cpp             # Program entry point
        │   │   └── (...)
        │   └── CMakeLists.txt           # CMake build configuration for the app
        └── CMakeLists.txt               # Top-level CMake configuration file

Getting Started: Creating the Structure
=======================================

Let’s start by setting up the directory layout and initializing the submodule:

.. code-block:: console

    $ cd ~
    $ mkdir robot-escape
    $ cd robot-escape
    $ git init
    Initialized empty Git repository in ~/robot-escape/.git/
    $ git submodule add https://github.com/erbsland-dev/erbsland-cpp-configuration.git erbsland-cpp-configuration
    Cloning into '~/robot-escape/erbsland-cpp-configuration'...
    ...
    $ git submodule init erbsland-cpp-configuration

Now create the basic folder structure for your application:

.. code-block:: console

    $ mkdir robot-escape
    $ mkdir robot-escape/src

The Current Project State
=========================

At this point, your project directory structure should look like this, with the newly added components marked:

.. code-block:: none
    :emphasize-lines: 2-4

    robot-escape
        ├── erbsland-cpp-configuration   # [new] Submodule initialized and ready to use
        └── robot-escape                 # [new] Application source directory
            └── src                      # [new] Location for your C++ source files


.. button-ref:: 02-cmake-configuration
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    The CMake Configuration →

.. card:: About the Command-Line Examples

    Throughout this tutorial, we’ll use command-line examples to demonstrate essential steps like:

    - Adding Git submodules
    - Creating directories with ``mkdir``
    - Editing files with tools such as ``nano``

    These examples are intended to be simple and reproducible. Feel free to adapt them to your preferred tools or development environment.