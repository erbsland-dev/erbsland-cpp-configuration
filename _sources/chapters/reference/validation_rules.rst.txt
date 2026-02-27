..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

****************
Validation Rules
****************

Tutorials
=========

* :doc:`../usage/tutorial-validation-rules-embedded-elcl` - Define validation rules from an embedded ELCL document.
* :doc:`../usage/tutorial-validation-rules-code` - Build validation rules directly in C++ with ``RulesBuilder``.

Interface
=========

.. doxygenclass:: erbsland::conf::vr::RulesBuilder
    :members:

.. doxygenclass:: erbsland::conf::vr::Rule
    :members:

.. doxygentypedef:: erbsland::conf::vr::RulePtr

.. doxygenclass:: erbsland::conf::vr::RuleType
    :members:

.. doxygenclass:: erbsland::conf::vr::Constraint
    :members:

.. doxygentypedef:: erbsland::conf::vr::ConstraintPtr

.. doxygenclass:: erbsland::conf::vr::ConstraintType
    :members:
