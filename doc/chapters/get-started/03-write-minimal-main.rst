..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****************************
Write a Minimal Main Function
*****************************

Before we dive into more advanced features, let's start by writing a minimal ``main`` function. This allows us to verify that our project is correctly set up, builds successfully, and that our application can be executed.

#.  Create and open the file:

    .. code-block:: console

        $ nano robot-escape/src/main.cpp

#.  Add the following code:

    .. code-block:: cpp
        :caption: <project>/robot-escape/src/main.cpp

        #include <erbsland/all_conf.hpp>

        #include <filesystem>
        #include <iostream>
        #include <format>

        using namespace el::conf;

        int main(int argc, char **argv) {
            if (argc < 2) {
                std::cout << "Usage: " << argv[0] << " <config-file>\n";
                return 1;
            }

            const auto configFile = std::filesystem::path{argv[1]};
            try {
                Parser parser;
                const auto source = Source::fromFile(configFile);
                const auto doc = parser.parseOrThrow(source);

                auto width = doc->getIntegerOrThrow(u8"field.width");
                auto height = doc->getIntegerOrThrow(u8"field.height");

                std::cout << std::format("Field width = {}, height = {}\n", width, height);
                return 0;
            } catch (const Error &error) {
                std::cerr << error.toText().toCharString() << "\n";
                return 1;
            }
        }

Code Breakdown
==============

Header Includes and Namespace
-----------------------------

.. code-block:: cpp

    #include <erbsland/all_conf.hpp>
    #include <filesystem>
    #include <iostream>
    #include <format>
    using namespace el::conf;

We include all headers of the configuration parser in one line via ``all_conf.hpp``. This is a convenience for small examples—keeping things compact and readable.

However, in real-world applications, we recommend including only the headers you actually use, like this:

.. code-block:: cpp

    #include <erbsland/conf/Parser.hpp>
    #include <erbsland/conf/Source.hpp>
    #include <erbsland/conf/Error.hpp>
    #include <filesystem>
    #include <iostream>
    #include <format>

    void myFunction() {
        using el::conf::Parser;
        using el::conf::Source;
        using el::conf::Error;
        // ...
    }

Avoiding ``using namespace`` in production code helps keep your namespaces clean and prevents name collisions.

Handling Command-Line Input
----------------------------

.. code-block:: cpp

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <config-file>\n";
        return 1;
    }

    const auto configFile = std::filesystem::path{argv[1]};

We check whether a configuration file path is provided as a command-line argument. If not, we print a helpful usage message and exit.

There’s no need to manually check whether the file exists—the parser will handle that for us.

Parsing the Configuration File
------------------------------

.. code-block:: cpp

    Parser parser;
    const auto source = Source::fromFile(configFile);
    const auto doc = parser.parseOrThrow(source);

We create a ``Parser`` instance (a lightweight object), load the file into a ``Source``, and parse it.

The ``parseOrThrow()`` function throws an ``Error`` exception on failure (e.g., missing file, syntax error). We catch this below and print the error message in a readable form.

Accessing Parsed Values
------------------------

.. code-block:: cpp

    auto width = doc->getIntegerOrThrow(u8"field.width");
    auto height = doc->getIntegerOrThrow(u8"field.height");

    std::cout << std::format("Field width = {}, height = {}\n", width, height);

We use ``getIntegerOrThrow`` to read two values from the config. If the path doesn't exist or the type is wrong, an exception is thrown and caught in the same block as parsing errors.

If everything works, the application prints out the values from your configuration file.

The Current Project State
=========================

At this point, your project directory structure should look like this, with the newly added components marked:

.. code-block:: none
    :emphasize-lines: 5

    robot-escape
        ├── erbsland-cpp-configuration
        ├── robot-escape
        │   ├── src
        │   │   └── main.cpp             # [new] The main entry point
        │   └── CMakeLists.txt
        └── CMakeLists.txt

.. button-ref:: 04-compile-and-run
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Compile and Run the Application →

