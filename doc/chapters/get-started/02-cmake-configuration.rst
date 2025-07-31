***********************
The CMake Configuration
***********************

Writing the Main CMake File
===========================

To link the Erbsland Configuration Parser with your application, we'll create a top-level CMake configuration file that includes both the parser and your app directory.

We’ll start by creating the root `CMakeLists.txt`:

#.  Create and open the file:

    .. code-block:: console

        $ nano CMakeLists.txt

#.  Add the following contents:

    .. code-block:: cmake
        :caption: <project>/CMakeLists.txt

        cmake_minimum_required(VERSION 3.25)
        project(RobotEscapeProject)

        add_subdirectory(erbsland-cpp-configuration)
        add_subdirectory(robot-escape)

This file does the following:

* **Specifies the minimum CMake version**: Ensures you're using features available in CMake 3.25 or newer.
* **Names your project**: Here, we call it ``RobotEscapeProject``.
* **Adds subdirectories** in order:
  
  - :file:`erbsland-cpp-configuration`: The parser library.
  - :file:`robot-escape`: Your application code.

CMake processes these in the order given, which means all libraries are available before compiling the app itself.

Setting Up the Application CMake File
=====================================

Next, let’s set up the CMake configuration for your application specifically.

#.  Create and open the application-level CMake file:

    .. code-block:: console

        $ nano robot-escape/CMakeLists.txt

#.  Add the following contents:

    .. code-block:: cmake
        :caption: <project>/robot-escape/CMakeLists.txt

        cmake_minimum_required(VERSION 3.25)
        project(RobotEscapeApp)

        add_executable(robot-escape
            src/main.cpp)

        target_compile_features(robot-escape PRIVATE cxx_std_20)
        target_link_libraries(robot-escape PRIVATE erbsland-configuration-parser)

This file does a few important things:

* **Defines your app as an executable** using ``main.cpp`` as the entry point.
* **Enables C++20** using ``target_compile_features``.
* **Links your app to the parser library**, which was registered earlier by its CMake project name: ``erbsland-configuration-parser``.

The Current Project State
=========================

At this point, your project directory structure should look like this, with the newly added components marked:

.. code-block:: none
    :emphasize-lines: 5, 6

    robot-escape
        ├── erbsland-cpp-configuration
        ├── robot-escape
        │   ├── src
        │   └── CMakeLists.txt           # [new] The application CMake file
        └── CMakeLists.txt               # [new] The main project CMake file

.. button-ref:: 03-write-minimal-main
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Write a Minimal Main Function →

