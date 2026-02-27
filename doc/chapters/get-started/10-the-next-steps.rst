..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
The Next Steps
**************

I hope this small example application has helped you get a feel for how to use the configuration parser and its interface in a real-world context. While we focused on the basics here, the parser and the configuration language offer a lot more than what we could cover in this short tutorial.

Unfortunately, due to time constraints, this is where our guided journey ends—but we’d love to see you take it further on your own!

.. role:: cpp-code(code)
   :language: cpp

Here are a few fun ideas to enhance the game and challenge yourself:

* Add a fallback configuration using :cpp-code:`Source::fromText()` that is compiled into the binary and used when no external configuration file is provided.
* Increase configuration nesting by introducing a ``level`` list. The game can start at the first level and progress to the next once the player reaches the goal.
* Make the spawn location and number of robots configurable per level.
* Add support for player lives, and make the number of lives configurable.
* Make in-game texts (like win/loss messages) customizable through the configuration.
* (... and whatever else you dream up!)

What to Read Next?
==================

The :doc:`reference documentation</chapters/reference/index>` covers all the details of the configuration parser's API. If you're working directly with the parser, start with the ``Value`` and ``Parser`` interfaces—they're the most essential building blocks.

You might also want to explore more advanced topics like creating your own :doc:`access checks</chapters/reference/access_check>` or customizing how files are loaded via :doc:`file access</chapters/reference/source_resolver>`.

.. button-ref:: ../reference/index
    :ref-type: doc
    :color: info
    :align: center
    :outline:
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Check Out the Reference Documentation →

Don't forget to explore the full `Erbsland Configuration Language documentation <https://config-lang.erbsland.dev>`_. It’s structured for quick lookups or deep dives, depending on your needs and time. This resource will help you unlock the full potential of the configuration language and inspire ideas for how to use it more effectively in your projects.

.. button-link:: https://config-lang.erbsland.dev
    :color: info
    :align: center
    :outline:
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Read the Erbsland Configuration Language Documentation →

If your project requires strict schema checks, optional defaults, and typed constraints, explore the dedicated validation-rules tutorials:

.. button-ref:: /chapters/usage/tutorial-validation-rules-embedded-elcl
    :ref-type: doc
    :color: info
    :align: center
    :outline:
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-mt-4

    Use Validation-Rules as Embedded ELCL Document →

.. button-ref:: /chapters/usage/tutorial-validation-rules-code
    :ref-type: doc
    :color: info
    :align: center
    :outline:
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-mb-4

    Use Validation-Rules Built from Code →
