
*****************
Using Value Lists
*****************

When configuring the position and size of rooms, specifying each value individually for every rectangle can quickly become repetitive and cluttered. To make your configuration files cleaner and easier to maintain, it's often more practical to use grouped value lists.

This tutorial introduces three alternative keys you can use to define rectangles more concisely: ``rectangle``, ``position``, and ``size``. These can be used interchangeably in the configuration file, giving you the flexibility to choose whichever makes the structure clearer and easier to read.

Below is an example that demonstrates how you can use these alternatives in practice:

.. literalinclude:: example_v4/configuration.elcl
    :caption: configuration.elcl
    :linenos:
    :language: erbsland-conf

:download:`Download this Configuration<example_v4/configuration.elcl>`

.. role:: cpp-code(code)
   :language: cpp

Improving our Configuration Logic
=================================

To support these flexible input options in code, we need to enhance how we read the rectangle values. We’ll introduce a new method called ``rectFromSection()`` that can interpret the configuration from any of the three input styles.

In addition, we’ll add ``exitWithErrorInValue()`` to handle errors gracefully whenever a required value is missing or misconfigured.

.. literalinclude:: example_v4/Application.hpp
    :caption: robot-escape/src/Application.hpp
    :lines: 45-100
    :emphasize-lines: 1, 10
    :lineno-match:
    :linenos:
    :language: cpp

:download:`Download the updated Application.hpp<example_v4/Application.hpp>`

Testing for Values
------------------

Before using a configured value, it's important to verify whether it was actually set. You can do this using:

:cpp-code:`value->hasValue(u8"rectangle")`

This method returns ``true`` if a value exists for the given key. Keep in mind, however, that the value could be of any type, including a section. So you’ll still need to validate the type afterward.

.. literalinclude:: example_v4/Application.hpp
    :lines: 54-72
    :emphasize-lines: 3, 7, 9
    :lineno-match:
    :linenos:
    :language: cpp

Reading a Value List
--------------------

To retrieve a list of values from the configuration, use the following method:

:cpp-code:`value->getListOrThrow<int>(u8"rectangle")`

.. literalinclude:: example_v4/Application.hpp
    :lines: 54-72
    :emphasize-lines: 4, 10, 12
    :lineno-match:
    :linenos:
    :language: cpp

The ``getListOrThrow<>`` template method is a convenient all-in-one call. It first checks whether the value exists at the given location. Then, it verifies whether the value is a list and ensures that each entry matches the expected type.

In this case, we expect a list of integers. If all values in the list are valid and convertible to ``int``, they are returned as a C++ vector. If any value is invalid—be it due to type mismatch or exceeding the ``int`` range—an exception is thrown with a clear, helpful error message.

Give it a try and see how it behaves!

Reading Integers with a Cast
-----------------------------

Previously, individual values like ``x`` were read using:

:cpp-code:`static_cast<int>(roomValue->getIntegerOrThrow(u8"x"))`

We’ve now switched to this cleaner syntax:

:cpp-code:`value->getOrThrow<int>(u8"x")`

.. literalinclude:: example_v4/Application.hpp
    :lines: 54-72
    :emphasize-lines: 16-17
    :lineno-match:
    :linenos:
    :language: cpp

Not only is this new version more concise, but it also adds an important benefit: range checking. ``getOrThrow<>`` ensures that the retrieved value fits within the target type—in this case, ``int``. If it doesn’t, it throws an exception with a descriptive message.

You can test this by deliberately entering an out-of-range value like ``x = 0x7000'0000'0000'0000``. The application will now reject this gracefully, whereas previously it may have defaulted silently to zero.

Compile and Run the Updated Game
================================

With our improved configuration logic in place, it's time to compile and run the updated game.

.. code-block:: console

 $ cmake --build cmake-build
 $ ./cmake-build/robot-escape/robot-escape configuration.elcl

.. ansi-block::
    :escape-char: ␛

    ----------------------------==[ ROBOT ESCAPE ]==-----------------------------
    Welcome to Robot Escape!
    You (☻) must run to the exit (⚑) before any robot (♟) catches you.

    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░␛[32m┏━━━━━━━━━━━━━━━━┓␛[90m░░░░░░░░
    ░░░░░░␛[32m┃␛[0m                ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┏━┛␛[0m                ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┃␛[0m   ␛[93m☻␛[0m              ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┃␛[0m        ␛[32m┏━━┓␛[0m      ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┗━┓␛[0m      ␛[32m┃␛[90m░░␛[32m┃␛[0m      ␛[32m┃␛[90m░░░░░░░░
    ░░░░░░␛[32m┃␛[0m      ␛[32m┃␛[90m░░␛[32m┃␛[0m      ␛[32m┗━━━━┓␛[90m░░░
    ░░░░░░␛[32m┃␛[0m    ␛[91m♟␛[0m ␛[32m┃␛[90m░░␛[32m┃␛[0m          ␛[91m♟␛[32m┃␛[90m░░░
    ░░░░░░␛[32m┃␛[0m      ␛[32m┃␛[90m░░␛[32m┃␛[0m      ␛[32m┏━━━━┛␛[90m░░░
    ░░░░░░␛[32m┃␛[0m      ␛[32m┗━━┛␛[92m⚑␛[0m     ␛[32m┃␛[90m░░░░░░░░
    ░░░░░░␛[32m┃␛[0m                ␛[32m┗━━┓␛[90m░░░░░
    ░░░░░░␛[32m┃␛[0m     ␛[91m♟␛[0m             ␛[32m┃␛[90m░░░░░
    ░░░░░░␛[32m┃␛[0m                   ␛[32m┃␛[90m░░░░░
    ░░░░░░␛[32m┗━━━━━━━━━━━━━━━┓␛[0m   ␛[32m┃␛[90m░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░␛[32m┗━━━┛␛[90m░░░░░
    ␛[0m
    Enter your move (n/e/s/w/q=quit): e
    (...)
    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░␛[32m┏━━━━━━━━━━━━━━━━┓␛[90m░░░░░░░░
    ░░░░░░␛[32m┃␛[0m                ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┏━┛␛[0m                ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┃␛[0m       ␛[91m♟␛[93m∙∙␛[93m☻␛[0m       ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┃␛[0m       ␛[91m∙␛[32m┏━━┓␛[0m      ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┗━┓␛[0m    ␛[91m∙∙␛[32m┃␛[90m░░␛[32m┃␛[0m     ␛[91m♟␛[32m┃␛[90m░░░░░░░░
    ░░░░░░␛[32m┃␛[0m     ␛[91m♟␛[32m┃␛[90m░░␛[32m┃␛[0m     ␛[91m∙␛[32m┗━━━━┓␛[90m░░░
    ░░░░░░␛[32m┃␛[0m    ␛[91m∙∙␛[32m┃␛[90m░░␛[32m┃␛[0m     ␛[91m∙∙␛[0m    ␛[32m┃␛[90m░░░
    ░░░░░░␛[32m┃␛[0m    ␛[91m∙␛[0m ␛[32m┃␛[90m░░␛[32m┃␛[0m      ␛[32m┏━━━━┛␛[90m░░░
    ░░░░░░␛[32m┃␛[0m      ␛[32m┗━━┛␛[92m⚑␛[0m     ␛[32m┃␛[90m░░░░░░░░
    ░░░░░░␛[32m┃␛[0m                ␛[32m┗━━┓␛[90m░░░░░
    ░░░░░░␛[32m┃␛[0m                   ␛[32m┃␛[90m░░░░░
    ░░░░░░␛[32m┃␛[0m                   ␛[32m┃␛[90m░░░░░
    ░░░░░░␛[32m┗━━━━━━━━━━━━━━━┓␛[0m   ␛[32m┃␛[90m░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░␛[32m┗━━━┛␛[90m░░░░░
    ␛[0m
    Enter your move (n/e/s/w/q=quit): e
    (...)
    ␛[0m␛[90m░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
    ░░░░░░␛[32m┏━━━━━━━━━━━━━━━━┓␛[90m░░░░░░░░
    ░░░░░░␛[32m┃␛[0m                ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┏━┛␛[0m                ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┃␛[0m                  ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┃␛[0m        ␛[32m┏━━┓␛[0m ␛[91m∙␛[0m    ␛[32m┃␛[90m░░░░░░░░
    ░░░░␛[32m┗━┓␛[0m      ␛[32m┃␛[90m░░␛[32m┃␛[0m ␛[91m∙␛[0m    ␛[32m┃␛[90m░░░░░░░░
    ░░░░░░␛[32m┃␛[0m      ␛[32m┃␛[90m░░␛[32m┃␛[0m ␛[91m∙␛[0m    ␛[32m┗━━━━┓␛[90m░░░
    ░░░░░░␛[32m┃␛[0m     ␛[91m∙␛[32m┃␛[90m░░␛[32m┃␛[0m ␛[91m♟␛[91m∙␛[0m        ␛[32m┃␛[90m░░░
    ░░░░░░␛[32m┃␛[0m     ␛[91m∙␛[32m┃␛[90m░░␛[32m┃␛[0m ␛[93m∙␛[91m∙␛[0m   ␛[32m┏━━━━┛␛[90m░░░
    ░░░░░░␛[32m┃␛[0m    ␛[91m♟␛[91m∙␛[32m┗━━┛␛[93m☻␛[91m♟␛[0m    ␛[32m┃␛[90m░░░░░░░░
    ░░░░░░␛[32m┃␛[0m                ␛[32m┗━━┓␛[90m░░░░░
    ░░░░░░␛[32m┃␛[0m                   ␛[32m┃␛[90m░░░░░
    ░░░░░░␛[32m┃␛[0m                   ␛[32m┃␛[90m░░░░░
    ░░░░░░␛[32m┗━━━━━━━━━━━━━━━┓␛[0m   ␛[32m┃␛[90m░░░░░
    ░░░░░░░░░░░░░░░░░░░░░░␛[32m┗━━━┛␛[90m░░░░░
    ␛[0m
    You won!


Recap
=====

Nice work! You've just made your configuration format much more flexible and developer-friendly.

Here's a quick summary of what you accomplished:

#. Introduced grouped value lists like ``rectangle``, ``position``, and ``size`` to simplify the configuration syntax.
#. Enhanced the configuration parser to accept and validate these new input formats using ``rectFromSection()``.
#. Learned how to check for values and read them safely using ``hasValue()``, ``getListOrThrow<>``, and ``getOrThrow<>``.
#. Replaced manual ``static_cast`` calls with cleaner, type-safe conversions that also validate ranges.

.. button-ref:: 10-the-next-steps
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    The Next Steps →
