================
Design Decisions
================


This document tries to answer the question why the language is designed the way it is.

Goals
=====

1. Well defined language, no undefined nor implementation defined behavior
2. Robust, errors are detected as early as possible, preferably at compile time
3. Modularized, errors in one module does not propagate into another
4. Composable, modules should never be more specific than needed
5. Error indications pointing at root causes, not just symptoms

Well defined language
---------------------

Code written should have a non-ambiguous translation into machine behavior. There might be fancy things happening for sake of performance, but this must never cause the code to behave differently except for the needed resources like time and memory.

The code should also have a consistent syntax, using a few base concepts that are applied to build more complex constructs. This should make the language easier to learn and easier to port between platforms.

Robust
------

- Handle resource ownership, so compiler can make sure no resource leaks occur
- Handle resource access, so compiler can make sure no race conditions occur

Composable
-----------

- Be able to build big modules out of smaller modules without any changes in behavior of the small modules
- Be able to write knew variants of a module without needing to recompile the other parts of the code
- Simple syntax to promote using the language as a domain specific language (easier to modify its looks)

Values
======

Following standard values are defined:

- nil - Represents the empty value
- true - For true
- false - For false
- any - for any one value
- all - for all possible values
- err - for errors

Source of Inspiration
=====================

Paradigms
---------

Structured programming
   removed possibility to explicitly transfer flow of control to some other part of the code, typically using the "goto" statement. The structured programming paradigms said that such explicit transfer of control should be done using structures like conditionals and loops.

Functional programming
   removed possibility for having variable assignment, only initializations were allowed. This removed a lot of robustness problems around illegal states due to parts of an object state to be modified but missed to update other parts of the state.

Object-oriented programming
   removed the possibility to do indirect transfer of control directly, typically using function pointers. Instead this is done using polymorphism or interface.

Declaractive programming
   express what the end state looks like.

Imperative programming
   describes sequential steps for reaching the end step.

Common problems
---------------

Module and language versioning
   See for example the mess with Python versions, where virtual environments are needed in order to handle the version problem. Upgrading the system wide Python interpreter is not for the faint hearted.

Snapshots
   Git has a good concept of snapshots, this is also reinvented in other tools like Docker, Vagrant, etc. Instead of constantly reinventing this wheel, the programming language itself should have a support for doing a snapshot of the current state, and have mechanisms for determining what should be included in such a snapshot.

Meta-data sharing
   Solving above problems for just one language is not enough. The programming language should have support for understanding standard ways other languages use for describing different kinds of meta-data. As for versioning, this could be being co-operative with the system package manager, Python pip, etc. So whenever there is a dependency on modules written in other languages, meta-data support for describing what version, variant etc is needed should also be included.

Configuration storage
   How to (persistently) store configuration should not be a concern for the application needing to store its configurations. There should be a standard API defined by the programming language how this should look like. An adapter component could then define how a set of components store their configurations. How configuration is stored depends on how and where the component is deployed. In docker containers, there might be one preferred way, for a Linux system another, for Windows a third, if deployment is system wide this might be different compared to if it is for a specific user only, if using cloud micro-service architecture there might be yet another preferred way, and so on.

Domain-Specific Languages
-------------------------

Looking at domains-specific languages should give inspiration into what syntax fits a specific application, and also what kind of abstractions that are most useful. The end goal is to have a language which can have different incarnations suiting different problem domains. This way, programmers do not need to learn a completely new language to be able to write code for a specific problem domain, they only need to learn a new area of Sisdel.

The following problem domains were considered:

- Text search
  - Regular expressions
- Text type setting
  - LaTeX
- Database queries
  - SQL
  - GraphQL
- Relational diagrams, e.g. state machines, transaction diagrams, dependency diagrams
  - Graphviz
  - PlantUML
- Protocol/interface description
  - CORBA
- Data sequencing
  - YAML
  - JSON
  - XML
  - Google's Protocol Buffers
- Hardware description
  - SpinalHDL
  - VHDL
  - Verilog
- Build description
  - Make
  - CMake
  - Rust Cargo
- Sound creation
  - CSound
  - SuperCollider
  - Chuck
- Syntax description
  - Backus-Naur Format

### Text search

Regular expressions are among the most used for text search. It has some advantages:

- Can search any text
- Commonly used

It has also some disadvantages:

- Hard to read, especially for complex search patterns
- Has no concept of scope, e.g. search for a word, paragraph, path entry, etc

The disadvantages can be solved by:

1. Make literals more explicit, so no escapes are needed for characters with special meanings
2. Make it possible to define context, and have operators that can make use of that (e.g. word, path entry, etc)

Examples:

    search-for <- match text ignore-case '(prefix: )' ? int ( '(,)' int ) * '(: )' word nl

This creates a match expression similar to how regular expressions work. It will match when a string is found which optionally begins with the string literal 'Prefix: ', followed by on or more integers separated with literal ',', followed by literal string ': ', followed by a word and ending with a platform dependent new-line character sequence. The matches are done with character case ignored.

Type
====

Type in Sisdel consists of three parts:

1. Base type
2. Constraints
3. Representation

Base type are meant to describe fundamentally different things, while constrictions are meant to limit the use of the type. The representation is how the data is stored, and does not by itself prohibit use but rather triggers conversions.

Base type
---------

- Number, representations: integer, decimal, rational
- String, representations: utf-8
- Association, name/value pair
- Set, unsorted list of items
- List, sorted list of items
- One of, only one of the types listed apply

Note: A single item is compatible with both set and list, since it can be viewed as a list or set with only one item and since the list or set has only one item it can be viewed as either sorted or unsorted.

Note: To simulate synchronization points in the code, a list of sets can be used. This way each set can be executed in parallel, but the sets themselves are executed sequentially.

Constraints
-----------

- [list/set] finite: There is an upper bound for length of list or set
- [list/set] type: All items in the list/set are compatible with type
- [all] storage: Can be maximum number of bytes to store data, or need to adhere to certain standard like IEEE 754
- [all] unit: Manually specified constraint to help distinguish different things that otherwise would be type compatible
- [number] value range: Maximum and/or minimum value
- [string] character set: Allowed characters in string
- [number] precision: How many digits are valid

Syntax Playground
=================

## Switch expression

Due to the base syntax of the language, a special switch statement is not needed. Instead, switch can be written in the following way:

    myvar
    	= int then print ( '(is int)' nl )
    	= match ( int ( [ space | tab ] * '(,)' [ space | tab ] int ) * ) then print '(is list of int)'
    	< 0 then print ( '(is negative)' nl )

If myvar is a negative integer, the above will print "is int" as well as "is negative". Since a block of statements is by default a set of statements, there is no priority between them. This means that all statements are evaluated, and must not be a dependency on the order. The expressions are however executed in the order given, i.e. "is int" will be printed before "is negative" for a negative integer.

If `true then <expression>` is used then this will always be run. If this expression is placed last in the block of statements, it will be executed after any other match.

If you want the statements evaluated in the order given, make the block of statement a list by simply adding the `list` operator:

    myvar list
    	= int then print ( '(is int)' nl )
    	= match ( int ( [ space | tab ] * '(,)' [ space | tab ] int ) * ) then print '(is list of int)'
    	< 0 then print ( '(is negative)' nl )

If `true then <expression>` is used then this will be a catch all, i.e. if no other expression matched this expression will be executed.

## then operator

Syntax:

    <boolean> then <expression>

<expression> is executed when <boolean> evaluates to true. The expression returns the result of <expression> if executed, or `nil` otherwise.

Example:

    a > b then print ( '(a is greated than b)' nl )

## /? Match expression optional

/([)/ /?

Means zero or one [ character

## ! Assertion operator

a < b !

## : Assignment operator

a : 5

## <=> compare operator

a <=> b ?
	> print "(larger)"
	= print "(equal)"
	< print "(smaller)"

Rules
=====

1. Types can be fully specified, partly specified or not specified at all
2. Operators are context sensitive, i.e. what operator that will be invoked depends on type for current context
3. If type of current context allows several operator implementations, this is a compile error

References
==========

- Elements of Programming
  http://elementsofprogramming.com/eop_bluelinks.pdf

