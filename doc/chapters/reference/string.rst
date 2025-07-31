
******
String
******

Usage
=====

.. code-block:: cpp
    :linenos:

    auto str = String{u8"text"}; // unchecked, assuming valid UTF-8
    // ...
    std::cout << str.toCharString();

.. code-block:: cpp
    :linenos:

    void processData(std::string_view view) {
        auto str = String::fromCharString(view); // checked, throws on encoding errors
        // ...
    }

Interface
=========

.. doxygenclass:: erbsland::conf::String
    :members:

.. doxygenenum:: erbsland::conf::EscapeMode

