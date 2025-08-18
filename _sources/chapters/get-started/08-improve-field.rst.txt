
***************************
Make the Field Configurable
***************************

So far, the game field has been a simple rectangle. That works—but it’s a bit too predictable. There’s nowhere to hide, and the game feels linear.

Let’s fix that by making the field configurable.

A simple way to add variety is by introducing **rooms**—rectangular sections that make up the walkable areas of the map. These rooms are defined in the configuration and merged into the playfield to build more interesting layouts.

We use a *section list* in the configuration to define individual rooms:

.. literalinclude:: example_v3/configuration.elcl
    :caption: configuration.elcl
    :linenos:
    :language: erbsland-conf

.. role:: cpp-code(code)
   :language: cpp

Updates to the Game Framework
=============================

Extending the Geometry Classes
------------------------------

First, we expand our geometry module with a new ``Rectangle`` class, which encapsulates the size and position of a room in a single value. We also add a few helper methods to simplify merging rooms and checking which tiles are inside or outside of a rectangle.

.. dropdown:: Changes in :file:`robot-escape/src/Geometry.hpp`
    :class-container: code-dropdown

    .. literalinclude:: example_v3/Geometry.hpp
        :linenos:
        :emphasize-lines: 26-31, 43, 51-53, 62-96
        :language: cpp

:download:`Download the updated Geometry.hpp<example_v3/Geometry.hpp>`

Minor Update to the Canvas
--------------------------

The ``Canvas`` now uses the new ``Rectangle`` class to render room shapes.

.. dropdown:: Changes in :file:`robot-escape/src/Canvas.hpp`
    :class-container: code-dropdown

    .. literalinclude:: example_v3/Canvas.hpp
        :linenos:
        :emphasize-lines: 40-42
        :language: cpp

:download:`Download the updated Canvas.hpp<example_v3/Canvas.hpp>`

Adding Rooms to the Field
--------------------------

In :file:`World.hpp`, we introduce a new class, ``Room``, and update the ``Field`` class so it can build the playfield based on a list of configured rooms.

.. dropdown:: Changes in :file:`robot-escape/src/World.hpp`
    :class-container: code-dropdown

    .. literalinclude:: example_v3/World.hpp
        :linenos:
        :emphasize-lines: 35-37, 39-68, 76, 84, 87, 97, 121
        :language: cpp

:download:`Download the updated World.hpp<example_v3/World.hpp>`

Reading the Section List from the Configuration
===============================================

Next, let’s look at how we read this list of room definitions from the configuration.

Here’s a quick overview of the updated code in :file:`Application.hpp`:

.. literalinclude:: example_v3/Application.hpp
    :caption: robot-escape/src/Application.hpp
    :linenos:
    :emphasize-lines: 18-20, 42-60, 74
    :language: cpp

:download:`Download the updated Application.hpp<example_v3/Application.hpp>`

To read the section list, we use:

:cpp-code:`*config->getSectionListOrThrow("field.room")`

This line checks for the key ``field.room``, confirms it contains a section list, and returns it. The ``*`` operator dereferences the result so it can be used directly in a :cpp-code:`for()` loop.

Here’s the key part of the loop:

.. literalinclude:: example_v3/Application.hpp
    :lines: 44-52
    :language: cpp

Inside the loop, we access each section’s individual values using relative paths. For instance:

:cpp-code:`roomValue->getIntegerOrThrow(u8"x")`

This retrieves the ``x`` value from the room’s section. If it’s the first room in the configuration, the full path to that value would be ``field.room[0].x``.


Compile and Run the Updated Game
================================

Let’s compile the updated game and try out the new configurable room layout.

.. code-block:: console

    $ cmake --build cmake-build
    $ ./cmake-build/robot-escape/robot-escape configuration.elcl

.. ansi-block::
    :escape-char: ␛

    ----------------------------==[ ROBOT ESCAPE ]==-----------------------------
    Welcome to Robot Escape!
    You (☻) must run to the exit (⚑) before any robot (♟) catches you.

    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░␛[32m┏━━━━━━━━┓␛[90m░░░░░░␛[32m┏━━━━━━━━┓␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m      ␛[91m♟␛[0m ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┗━━━━━━┛␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m                 ␛[91m♟␛[0m      ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m                        ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┏━━━━━━┓␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m      ␛[92m⚑␛[0m ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m    ␛[93m☻␛[0m   ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m  ␛[91m♟␛[0m     ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┗━━━━━━━━┛␛[90m░░░░░░␛[32m┗━━━━━━━━┛␛[90m░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ␛[0m
    Enter your move (n/e/s/w/q=quit): n
    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░␛[32m┏━━━━━━━━┓␛[90m░░░░░░␛[32m┏━━━━━━━━┓␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m      ␛[91m∙␛[0m ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┗━━━━━━┛␛[0m      ␛[91m♟␛[0m ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m                 ␛[91m∙␛[0m      ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m                 ␛[91m♟␛[0m      ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┏━━━━━━┓␛[0m    ␛[93m☻␛[0m   ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m      ␛[92m⚑␛[0m ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m    ␛[93m∙␛[0m   ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m  ␛[91m∙␛[91m♟␛[0m    ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┗━━━━━━━━┛␛[90m░░░░░░␛[32m┗━━━━━━━━┛␛[90m░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ␛[0m
    Enter your move (n/e/s/w/q=quit): w
    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░␛[32m┏━━━━━━━━┓␛[90m░░░░░░␛[32m┏━━━━━━━━┓␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m      ␛[91m∙␛[0m ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┗━━━━━━┛␛[0m      ␛[91m∙␛[0m ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m                 ␛[91m∙␛[0m    ␛[91m♟␛[0m ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m                 ␛[91m∙␛[91m♟␛[0m     ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┏━━━━━━┓␛[0m   ␛[93m☻␛[93m∙␛[0m   ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m   ␛[91m♟␛[0m  ␛[92m⚑␛[0m ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m    ␛[93m∙␛[0m   ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m  ␛[91m∙∙␛[0m    ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┗━━━━━━━━┛␛[90m░░░░░░␛[32m┗━━━━━━━━┛␛[90m░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ␛[0m
    Enter your move (n/e/s/w/q=quit): w
    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░␛[32m┏━━━━━━━━┓␛[90m░░░░░░␛[32m┏━━━━━━━━┓␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m      ␛[91m∙␛[0m ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m        ␛[32m┗━━━━━━┛␛[0m      ␛[91m∙␛[0m ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m                 ␛[91m∙␛[0m   ␛[91m♟␛[91m∙␛[0m ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m                 ␛[91m∙∙␛[0m     ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m   ␛[91m♟␛[0m    ␛[32m┏━━━━━━┓␛[0m  ␛[91m♟␛[93m∙∙␛[0m   ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m   ␛[91m∙␛[0m  ␛[92m⚑␛[0m ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m    ␛[93m∙␛[0m   ␛[32m┃␛[90m░░░
    ░░░␛[32m┃␛[0m  ␛[91m∙∙␛[0m    ␛[32m┃␛[90m░░░░░░␛[32m┃␛[0m        ␛[32m┃␛[90m░░░
    ░░░␛[32m┗━━━━━━━━┛␛[90m░░░░░░␛[32m┗━━━━━━━━┛␛[90m░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ␛[0m
    You lost!

But maybe next time... you'll make it! 😉

Recap
=====

The addition of configurable rooms already makes the game more interesting and challenging. It’s now up to you to experiment—try different room shapes, sizes, and positions to create more exciting layouts!

That said, writing out ``x``, ``y``, ``width``, and ``height`` for every room can get tedious. In the next section, we’ll streamline the configuration by introducing *value lists*—a much more compact and elegant way to describe the geometry of the rooms.

.. button-ref:: 09-using-value-lists
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Using Value Lists →
