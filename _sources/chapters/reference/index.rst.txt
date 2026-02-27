..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*********
Reference
*********

.. toctree::
    :maxdepth: 1
    :hidden:

    parser
    value
    document
    source
    name
    error
    location
    signer
    source_resolver
    access_check
    signature_validator
    string
    data
    validation_rules
    namespace

Alphabetical List
=================

.. list-table::
    :width: 100%
    :widths: 30 70
    :header-rows: 1

    *   - Name
        - Description
    *   - :doc:`AccessCheck<access_check>`
        - The interface to access check implementations.
    *   - :doc:`AccessCheckPtr<access_check>`
        -
    *   - :doc:`AccessCheckResult<access_check>`
        -
    *   - :doc:`AccessSources<access_check>`
        - The source identifiers to verify in the access function.
    *   - :doc:`Bytes<data>`
        - A sequence of bytes.
    *   - :doc:`Constraint<validation_rules>`
        - A constraint of a validation rule.
    *   - :doc:`ConstValuePtr<value>`
        -
    *   - :doc:`Date<data>`
        - A date value with year, month and day parts.
    *   - :doc:`DateTime<data>`
        - A date-time value, with a date, time and time-offset part.
    *   - :doc:`Document<document>`
        - A configuration document.
    *   - :doc:`DocumentBuilder<document>`
        - Builds Configuration Documents Programmatically
    *   - :doc:`DocumentPtr<document>`
        -
    *   - :doc:`Error<error>`
        - The exception for all errors.
    *   - :doc:`ErrorCategory<error>`
        - The category of an error.
    *   - :doc:`EscapeMode<string>`
        -
    *   - :doc:`FileAccessCheck<access_check>`
        - A basic file access check.
    *   - :doc:`FileAccessCheckPtr<access_check>`
        -
    *   - :doc:`FileSourceResolver<source_resolver>`
        - A file source resolver.
    *   - :doc:`FileSourceResolverPtr<source_resolver>`
        -
    *   - :doc:`Location<location>`
        - Represents the location in a parsed document.
    *   - :doc:`Name<name>`
        - Represents a single name.
    *   - :doc:`NamePath<name>`
        - A name-path.
    *   - :doc:`NamePathLike<name>`
        - A name-path or convertible value.
    *   - :doc:`NameType<name>`
        -
    *   - :doc:`Parser<parser>`
        - This parser reads the Erbsland Configuration Language.
    *   - :doc:`Position<location>`
        - A position inside a document.
    *   - :doc:`Rule<validation_rules>`
        - A validation rule.
    *   - :doc:`RulesBuilder<validation_rules>`
        - Tool to build validation-rules documents via code.
    *   - :doc:`SignatureSigner<signer>`
        - The signer interface to create new signatures when signing documents.
    *   - :doc:`SignatureSignerData<signer>`
        - The data for the signer implementation.
    *   - :doc:`SignatureSignerPtr<signer>`
        -
    *   - :doc:`SignatureValidator<signature_validator>`
        - The interface for signature validation.
    *   - :doc:`SignatureValidatorData<signature_validator>`
        - The data from the parser to verify the signature of a document.
    *   - :doc:`SignatureValidatorPtr<signature_validator>`
        -
    *   - :doc:`SignatureValidatorResult<signature_validator>`
        -
    *   - :doc:`Signer<signer>`
        - The tool to sign configuration documents.
    *   - :doc:`Source<source>`
        - Interface for the data source to read the configuration.
    *   - :doc:`SourceIdentifier<source>`
        - Lightweight identifier for a configuration source.
    *   - :doc:`SourceIdentifierPtr<source>`
        -
    *   - :doc:`SourceList<source>`
        -
    *   - :doc:`SourceListPtr<source>`
        -
    *   - :doc:`SourcePtr<source>`
        -
    *   - :doc:`SourcePtrConst<source>`
        -
    *   - :doc:`SourceResolver<source_resolver>`
        - The interface for any source resolver implementation.
    *   - :doc:`SourceResolverContext<source_resolver>`
        - The context for resolving sources.
    *   - :doc:`SourceResolverPtr<source_resolver>`
        -
    *   - :doc:`String<string>`
        - Thin wrapper around std::u8string.
    *   - :doc:`Time<data>`
        - A time value with nanosecond precision and optional offset.
    *   - :doc:`TimeDelta<data>`
        - A combined time delta value.
    *   - :doc:`TimeOffset<data>`
        - A time offset.
    *   - :doc:`TimeUnit<data>`
        - A single time unit used in the configuration.
    *   - :doc:`Value<value>`
        - The base class and interface for all values.
    *   - :doc:`ValueIterator<value>`
        - Const iterator for the Value class.
    *   - :doc:`ValueList<value>`
        -
    *   - :doc:`ValuePtr<value>`
        -
    *   - :doc:`ValueType<value>`
        - The type of value.
    *   - :doc:`toNamePath<name>`
        -

By Topic
========

.. list-table::
    :header-rows: 1
    :widths: 30 70
    :width: 100%

    *   -   Topic
        -   Description
    *   -   :doc:`Access Check <access_check>`
        -   Describes the access check API.
    *   -   :doc:`Names and Name-Paths <name>`
        -   Covers name and name-path classes and types.
    *   -   :doc:`String <string>`
        -   The string wrapper interface.
    *   -   :doc:`Document API <document>`
        -   The document API (see :doc:`Value API <value>` for more details).
    *   -   :doc:`Data Classes <data>`
        -   Data classes like ``Date``, ``Bytes``, and others.
    *   -   :doc:`Namespace <namespace>`
        -   The namespace object and its behavior.
    *   -   :doc:`Error <error>`
        -   The error class and related helpers.
    *   -   :doc:`Signer <signer>`
        -   The signer tool and its API.
    *   -   :doc:`Location <location>`
        -   Classes for representing a document location.
    *   -   :doc:`Source Resolver <source_resolver>`
        -   The source resolver and its lookup logic.
    *   -   :doc:`Parser <parser>`
        -   Parser classes and their API.
    *   -   :doc:`Source <source>`
        -   The source class and related helpers.
    *   -   :doc:`Signature Validator <signature_validator>`
        -   Signature validation interface.
    *   -   :doc:`Value API <value>`
        -   The value API used throughout the document system.
    *   -   :doc:`Validation Rules <validation_rules>`
        -   The API to create validation-rules objects for document validation.
