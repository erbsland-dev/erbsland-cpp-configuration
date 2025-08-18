
******
String
******

The :cpp:class:`String<erbsland::conf::String>` wrapper’s primary purpose is to provide a safe and lightweight way of working with both ``char8_t`` and ``char`` strings. It helps you avoid common pitfalls while keeping your code simple and efficient, especially when working with string literals.

A typical use case looks like this:

.. code-block:: cpp

    // Use a u8"" string literal.
    auto rooms = value->getSectionListOrThrow(u8"room");

    // It also works with plain char "" string literals.
    level->_title = value->getOrThrow<std::string>("title");

    // Prefer constexpr std::u8string_view for identifiers.
    static constexpr std::u8string_view cNameColor = u8"color";
    if (value->hasValue(cNameColor)) {
        settings->_colorEnabled = value->getBooleanOrThrow(cNameColor);
    }

String literals (like ``u8"..."``) have a known length at compile time, which allows the compiler to optimize access. However, when the string length is undetermined at runtime, you need to provide an explicit wrapper:

.. code-block:: cpp

    // This looks like it should work...
    constexpr auto cName = u8"name"; // but this is just a "const char8_t*"
    auto name = value->getOrThrow(cName); // ❌ ERROR: Ambiguous overload

    // Must be written like this:
    auto name = value->getOrThrow(std::u8string{cName}); // ✅ OK

    // ... or, even better, declare it as a string_view:
    constexpr std::u8string_view cName = u8"name";
    auto name = value->getOrThrow(cName); // ✅ OK


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

