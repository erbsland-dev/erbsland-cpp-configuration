..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************************
Compile and Run Our Game
************************

Let’s compile our enhanced version of the project and try out the gameplay.

#.  First, build the project:

    .. code-block:: console

        $ cmake --build cmake-build
        ...

#.  Make sure to fix any errors we previously introduced in the configuration file.

    .. literalinclude:: example_v2/configuration.elcl
        :caption: configuration.elcl
        :linenos:
        :language: erbsland-conf

#.  Now launch the game:

    .. code-block:: console

        $ ./cmake-build/robot-escape/robot-escape configuration.elcl

The Gameplay
============

When the game starts, you’ll see a short welcome message followed by the rendered game field. A prompt invites you to enter your next move.

.. erbsland-ansi::
    :escape-char: ␛

    ----------------------------==[ ROBOT ESCAPE ]==-----------------------------
    Welcome to Robot Escape!
    You (☻) must run to the exit (⚑) before any robot (♟) catches you.

    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░␛[32m┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m       ␛[91m♟␛[0m                      ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                 ␛[93m☻␛[0m     ␛[91m♟␛[0m      ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m        ␛[92m⚑␛[0m                     ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m            ␛[91m♟␛[0m                 ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛␛[90m░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ␛[0m
    Enter your move (n/e/s/w/q=quit): s

Each move updates the world: the player and robots move one step, and their paths are visualized using trails. Here's how the game looks after a few steps:

.. erbsland-ansi::
    :escape-char: ␛

    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░␛[32m┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m       ␛[91m∙␛[0m                      ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m       ␛[91m∙␛[0m                      ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m       ␛[91m∙␛[91m♟␛[0m                     ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                 ␛[93m∙␛[0m  ␛[91m♟␛[91m∙∙∙␛[0m      ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m        ␛[92m⚑␛[0m      ␛[93m☻␛[93m∙∙␛[0m            ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m             ␛[91m♟␛[0m                ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m             ␛[91m∙␛[0m                ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m            ␛[91m∙∙␛[0m                ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛␛[90m░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ␛[0m
    Enter your move (n/e/s/w/q=quit): w

This is getting intense! The robots are just one step behind...

.. erbsland-ansi::
    :escape-char: ␛

    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░␛[32m┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m          ␛[93m☻␛[91m♟␛[91m∙␛[0m ␛[91m♟␛[91m∙␛[0m              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m          ␛[91m♟␛[93m∙␛[91m∙␛[0m  ␛[91m∙␛[0m              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m          ␛[91m∙␛[93m∙␛[91m∙␛[0m  ␛[91m∙␛[0m              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m        ␛[92m⚑␛[0m                     ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛␛[90m░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ␛[0m
    Enter your move (n/e/s/w/q=quit): w

And finally—you reach the goal unharmed!

.. erbsland-ansi::
    :escape-char: ␛

    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░␛[32m┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m        ␛[93m∙␛[91m∙∙∙∙␛[0m                 ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m        ␛[93m∙␛[91m∙␛[91m♟␛[0m                   ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m        ␛[91m♟␛[91m∙∙␛[0m                   ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m        ␛[93m☻␛[91m♟␛[0m                    ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┃␛[0m                              ␛[32m┃␛[90m░░░░
    ░░░░␛[32m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛␛[90m░░░░
    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ␛[0m
    You won!

Recap
=====

Your game is up and running—and it already feels fun and challenging! Give it a few more rounds, explore different strategies, and start thinking about how you might expand or improve the gameplay.


.. button-ref:: 08-improve-field
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Make the Field Configurable →

