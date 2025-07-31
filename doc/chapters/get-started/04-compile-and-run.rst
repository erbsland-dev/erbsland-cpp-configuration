*******************************
Compile and Run the Application
*******************************

Now that we have a minimal application in place, let’s build and run it to verify that everything is working as expected.

We’ll create a ``cmake-build`` folder inside the project directory. This is a common practice to keep build files separate from your source code, and it also keeps the command-line examples simple and clean.

#.  Create the build directory and initialize the CMake build system:

    .. code-block:: console

        $ mkdir cmake-build
        $ cmake . -B cmake-build
        ...
        -- Build files have been written to: ~/robot-escape/cmake-build

#.  Build the application:

    .. code-block:: console

        $ cmake --build cmake-build
        [  1%] Building CXX object TokenDecoder.cpp.o
        [  3%] Building CXX object Core.cpp.o
        ...
        [100%] Linking CXX executable robot-escape
        [100%] Built target robot-escape

#.  Run the application:

    .. code-block:: console

        $ ./cmake-build/robot-escape/robot-escape
        Usage: ./cmake-build/robot-escape/robot-escape <config-file>

You should see a usage message indicating that the application is running correctly and expects a configuration file.

Writing a Sample Configuration
==============================

To properly test our parser, we need to provide a configuration file. Let’s create one in the project’s root directory named ``configuration.elcl``.

#.  Create and open the configuration file:

    .. code-block:: console

        $ nano configuration.elcl

#.  Add the following content:

    .. code-block:: erbsland-conf
        :caption: <project>/configuration.elcl

        [field]
        width: 30
        height: 16

This defines a simple configuration with a section named ``field``, containing two integer values: ``width`` and ``height``.

Run the Application with the Configuration
==========================================

Now let’s run our application again, this time passing the configuration file as an argument:

.. code-block:: console

    $ ./cmake-build/robot-escape/robot-escape configuration.elcl
    Field width = 30, height = 16

Success! 🎉 The application correctly parsed the configuration file and printed the values to the console.

.. button-ref:: 05-provoking-errors
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Provoking some Errors →

