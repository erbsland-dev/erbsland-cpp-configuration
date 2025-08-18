.. index::
    single: build
    single: usage

*****************************
Build the Parser as a Library
*****************************

The Erbsland Configuration Parser can also be built and installed as a standalone static library. This approach is useful if you plan to reuse the library across multiple projects or integrate it into an existing build system independently.

However, for most use cases, we recommend integrating the parser directly as a Git submodule within your project. This method provides tighter version control and simplifies dependency management. For details, refer to :doc:`integration-submodule`.

To build and install the library, follow these steps:

.. code-block:: console

    $ git clone https://github.com/erbsland-dev/erbsland-cpp-configuration.git
    $ mkdir build
    $ cd build
    $ cmake ../erbsland-cpp-configuration -DCMAKE_BUILD_TYPE=Release
    -- The CXX compiler identification is (...)
    (...)
    -- Build files have been written to: (...)/build

    $ cmake --build .
    [  1%] Building CXX object (...)/TokenDecoder.cpp.o
    [  3%] Building CXX object (...)/Core.cpp.o
    (...)
    [100%] Linking CXX static library liberbsland-configuration-parser.a
    [100%] Built target erbsland-configuration-parser

    $ cmake --install .
    -- Install configuration: "Release"
    -- Installing: (...)/lib/liberbsland-configuration-parser.a
    -- Installing: (...)/include
    (...)

This will:

* Build the static library ``liberbsland-configuration-parser.a``.
* Install it to the default location along with the public header files.

By default, the installation path is platform-specific (e.g., ``/usr/local`` on Unix systems). You can customize it using the ``--prefix`` option on the install step if needed.
