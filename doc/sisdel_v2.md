% Simple Syntax Dependency Language (Sisdel)
% Mats Liljegren

License
=======

![CC-BY-4.0](CC-BY-license-88x31.png "Creative Commons 4.0 International License")
SPDX-License-Identifier: CC-BY-4.0

This work is licensed under a Creative Commons Attribution 4.0 International
License. See <http://creativecommons.org/licenses/by/4.0/> for license text.

Entity
======

The Sisdel language defines a few basic "things" that the application is built from. These "things" are called entity. An entity can be either of the following:

- Scope
- Data
- Meta-data

Sub-sequent chapters describe the above entities.

Scope
=====

A scope works as a namespace when referring to entities by name. There are two types of scopes, thread scope and lexical scope. The current scope is always a lexical scope.

Scopes are nested in a strict hierarchy, where the current scope is the top of the hierarchy. Sub-scopes are reached by naming them one by one until the correct scope has been reached.

Import statements are used to make scopes visible to the current scope. An import statement can choose to import only a single entity from some sub-scope, or the whole sub-scope. Only entities declared as public will be reachable when imported.

Lexical scope
-------------

The lexical scope is determined by where the code is written. The inner-most lexical scope is given by the operator scope, which can be several layers of operator scopes within a source file.

A source file defines a module scope. All symbols within a module are automatically imported. Symbol definitions are by default private however, so to use the automatical import the symbol must be defined as public.

Just like operators can be hierarchical, so can modules. So a directory tree with several modules (source files) can by itself be a module.

To import symbols from another module, an import statement must be executed.

Thread scope
------------

A thread scope is created when an application starts executed, and its contents will be the parameters given when application started.

A new thread scope is created for each operator call, and the contents of this new scope will be the parameters given to the operator call.

The thread scope is created whenever there is a possibility for a new parallell activity, regardless of whether the VM chooses to actually run this activity in parallell or not.

A new thread scope is therefore created when an instance of the program is started, and for each operator invocation.

The scope content of an thread scope is given by the parameters given when the scope was created.

All symbols created in thread scope are private by default, but can be made public. If public, the symbols are exported to child thread scopes. No import statements are needed.

Thread scope is reached from code using a pre-defined symbol "thread".

Data
====

Data is a piece of information that has a meaning for the application. The data can be of the following types:

- Immediate
- Operator
- Map

Immediate is data that can be understood as is. Operator requires an operator call, which then returns data. This returned data can be immediate data, and thereby be understood with no further operator calls. If the returned data is another operator, then another operator call will be required. This can be repeated as many times as needed to get immediate data.

Immediate
---------

Immedate data can be understood with no further calculation needed. There are two types of immedate data:

- Number
- String

There is no limit to the size of precision for numbers, and a generic number should therefore be used with care due to performance reasons. Numbers can be restricted, e.g. to make them integers, limit their value range etc.

String is a sequence of any valid and printable Unicode characters, including white space characters.

Operator
--------

Map
---
This is a two-way multi-map, meaning that you can both search for key given value, as well as search for value given a key. Being a multi-map means that you can get duplicates for both keys as well as values.

If multiple matches apply for a given search, then the map would return a list of them all.

Constraints that can be applied on maps are:

- Ordered - The order in which elements where inserted is kept.
- Sorted - Values are sorted based on keys.
- One-way - Can only go from key to value, not from value to key.
- 

Meta-data
=========

- Constraint
- ID

Constraint
----------

When specifying a constraint the interpreter/compiler will ensure the constraint is fullfilled and report compile time or runtime error if not.

If a symbol reference is ambigious a constraint can be used to resolve this. Constraints are also good indicators for what and how things can be optimized.

[] are used to specify constraints, and are specified right after the entity to be constrained. Example:

    myWeight is 37 [ type : unsigned-integer , unit : kg ]

In this case the entity is the immedate value 37. This entity has the type constrained to unsigned integer, and the unit to kilogram.

If a constraint is given on a scope that is only sensible for other entities, this becomes the default constraint applied on all created entities within this scope. In this case, a constraint of the same name specified on such an entity, will override this default. Example:

    myFunc is function [ unit : mm ] do (
        var1 is 50              # Uses default
	var2 is 5 [ unit : cm ] # Overrides the default
	return ( var1 = var2 )

This defines a function named myFunc. This creates a lexical function scope myFunc with the default unit millimeters. Within this scope, var1 is created without any constraints, which means that the default unit apply. So var1 is assigned 50 millimeter.

var2 however has an overriding constraint centimeter, and will therefore be assigned 5 centimeters.

This will make the return statement to return true, since var1 and var2 are equal.

Once an entity has been created any defined constraints cannot be overridden. The myFunc above cannot get a new constraint changing the default unit, but entities not already created can override the default when created.

It is however possible to add constraints afterwards if this is done in the same scope as the entity was created in. To add a default type for numbers as well, the above example can be extended into the following:

    myFunc is function [ unit : mm ] do
        var1 is 50              # Uses default
	var2 is 5 [ unit : cm ] # Overrides the default
	return ( var1 = var2 )
    myFunc [ type : integer ]

To use the above example and apply a constraint on thread scope, it can be done in the following way:

    myFunc is function ( thread : [ max-time : 50 [ unit : s ] ] ) do
        return "My fast function"

This puts a restriction on the execution time to 50 seconds on the myFunc execution thread.

It is possible to specify conversion functions for constraints. For example, to specify a function to convert unit millimeters to centimeters;

    [ unit is function input [ unit : mm ] return ( input * 10 ) [ unit : cm ] ]

As can be seen here, the function declaration is done within meta-data scope using []. Furthermore, the name of the conversion function is the name of the constraint to be converted.

The constraint on the input makes this function only applicable when the input parameter is in millimeters. By specifying a constraint on the output, the returned unit can be set.

The following constraints are pre-defined by the language:

- type
- unit
- storage-size
- value-range
- license

ID
--

The ID is a hash calculated from the code. It is expected that two entities with the same ID are actually the same object, or identical enough that there is no way to tell them apart.

