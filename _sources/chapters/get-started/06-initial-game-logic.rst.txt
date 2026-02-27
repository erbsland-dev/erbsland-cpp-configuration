..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****************************
Write an Initial Game Version
*****************************

Now that we have a working foundation, let’s implement the first version of our game!

.. note::

    You don't need to type everything by hand. After each code listing, there's a link to download the full source file.

Geometry Classes
================

We’ll start with some basic data structures that serve as building blocks for the game.

.. literalinclude:: example_v2/Geometry.hpp
    :caption: robot-escape/src/Geometry.hpp
    :linenos:
    :language: cpp

:download:`Download Geometry.hpp<example_v2/Geometry.hpp>`

In this file, we define the classes ``Position`` and ``Size`` with a minimal set of utility methods. These will be used throughout the game code.

Additionally, we define the function ``randomInt()`` and the directional offset array ``cPosDelta4``, which will help with movement logic.

The Canvas
==========

Instead of building a complex graphical interface, we use a simple ``Canvas`` class that renders the game world as colored text in the console.

.. literalinclude:: example_v2/Canvas.hpp
    :caption: robot-escape/src/Canvas.hpp
    :linenos:
    :language: cpp

:download:`Download Canvas.hpp<example_v2/Canvas.hpp>`

The World
=========

Next, we define the data structures that represent the game world.

.. literalinclude:: example_v2/World.hpp
    :caption: robot-escape/src/World.hpp
    :linenos:
    :language: cpp

:download:`Download World.hpp<example_v2/World.hpp>`

This file introduces the main game elements: ``Player``, ``Robot``, and ``Exit``. These are placed within the ``Field`` class, which defines the map the game is played on. All of these are grouped into the ``World`` structure—a complete model of the game’s state.

This abstraction allows us to easily render the game state and apply game logic to it.

Game Logic
==========

So far, we’ve only set up static structures. Now it’s time to make things dynamic.

.. literalinclude:: example_v2/Logic.hpp
    :caption: robot-escape/src/Logic.hpp
    :linenos:
    :language: cpp

:download:`Download Logic.hpp<example_v2/Logic.hpp>`

The game logic includes:

* ``inputFromConsole()`` — handles user input.
* ``PlayerLogic`` and ``RobotLogic`` — define movement behavior for players and robots.
* ``Logic`` — combines everything and drives the game forward by updating the world based on player actions.

Using the Game Classes
======================

We’re ready to glue everything together.

.. literalinclude:: example_v2/Application.hpp
    :caption: robot-escape/src/Application.hpp
    :linenos:
    :language: cpp

:download:`Download Application.hpp<example_v2/Application.hpp>`

The ``Application`` class handles the full game flow:

* Parses command-line arguments.
* Loads the configuration file.
* Builds the initial ``World`` state.
* Starts and runs the game loop.

The loop displays instructions, initializes the game logic, renders the world, waits for input, processes the input, and repeats—until either the player or the robots win.

Replacing the Old Main Function
===============================

With the new ``Application`` class in place, we can simplify our ``main`` function dramatically:

.. literalinclude:: example_v2/main.cpp
    :caption: robot-escape/src/main.cpp
    :linenos:
    :language: cpp

:download:`Download main.cpp<example_v2/main.cpp>`

Adding the New Files to CMake
=============================

Finally, we have to add the new source files to our CMake file.

.. code-block:: cmake
    :emphasize-lines: 4-7
    :caption: robot-escape/CMakeLists.txt

    cmake_minimum_required(VERSION 3.25)
    project(RobotEscapeApp)
    add_executable(robot-escape
            src/Canvas.hpp
            src/World.hpp
            src/Geometry.hpp
            src/Logic.hpp
            src/main.cpp)
    target_compile_features(robot-escape PRIVATE cxx_std_20)
    target_link_libraries(robot-escape PRIVATE erbsland-configuration-parser)

Recap
=====

That was quite a bit of code! But with it, we now have a functioning game loop and a flexible foundation you can build on.

We’ve kept things as simple as possible while still providing enough structure to be engaging and fun to extend. Try tweaking parts of the game and see what happens—you’re in full control!

.. button-ref:: 07-compile-and-run
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Compile and Run our Game →

