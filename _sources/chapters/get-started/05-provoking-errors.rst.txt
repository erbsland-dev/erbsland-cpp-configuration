..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*********************
Provoking Some Errors
*********************

Our application expects two required configuration values: ``field.width`` and ``field.height``. Both must be present, and both must be integers.

In this section, we'll intentionally introduce a few mistakes into the configuration file to see how errors are reported and handled by the parser.

Missing Values
==============

Let’s begin by commenting out one of the required values in :file:`configuration.elcl`:

.. code-block:: erbsland-conf

    [field]
    width: 30
    # height: 16

Now run the application again:

.. code-block:: console

    $ ./cmake-build/robot-escape/robot-escape configuration.elcl
    ValueNotFound: A required value was not found. name path = field.height

Because we're printing the error message using ``toText()``, the format looks like this:

.. code-block:: text

    <Error Class>: <Error Message> <Detail 1> <Detail 2> ...

This format provides both a high-level summary and detailed context, helping you quickly identify the problem.

Wrong Type
==========

Next, let’s see what happens when a value has the wrong type:

.. code-block:: erbsland-conf

    [field]
    width: 30
    height: "text"

Run the application:

.. code-block:: console

    $ ./cmake-build/robot-escape/robot-escape configuration.elcl
    TypeMismatch: A value has not the required type. Expected Integer but got Text. name path = field.height

This error tells you that the application expected an integer, but received a string instead.

Syntax Error
============

Finally, let’s introduce a syntax error. In the *ELCL* format, both section headers and value declarations must start at the beginning of a line. Any indentation will result in a syntax error:

.. code-block:: erbsland-conf
    :force:

    [field]
    width: 30
      height: 16

Running the app again:

.. code-block:: console

    $ ./cmake-build/robot-escape/robot-escape configuration.elcl
    Syntax: Value names must appear at the beginning of a line without leading spaces.
        at location = (...)/configuration.elcl:3:5

The error message clearly explains the issue and also includes the file name, line number, and column where the error occurred. This makes tracking down problems fast and straightforward.

Recap
=====

As you've seen, even in this early stage, the *ELCL* parser library takes care of the heavy lifting—handling validation, error reporting, and parsing details for you. This allows you to focus more on your application logic and less on boilerplate or edge-case handling.

.. button-ref:: 06-initial-game-logic
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Write an Initial Game Version →