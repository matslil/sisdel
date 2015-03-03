Simple Syntax Dependency Language (Sisdel)
==========================================

Sisdel is an experimental language intended to examine how to make
dependency tracking as accessible, useful and simple as possible.

Principles
----------
Principles are statements that should guide the langauge design. These
are seen as truths for having the best design, and have no internal
order of priority. The numbering is only for easy reference.

1. Each piece of code should have one specific purpose only, but the
   language may specify how generic that piece of code is.
   *Rationale:* Makes the code easier to understand, maintain and debug.
2. Software entities should be open for extensions, but closed for
   modifications.
   *Rationale:* Otherwise it becomes hard to know whether the piece of
   code you read is actually what will be executed.
3. A type should be replaceable with one of its sub-types, without
   altering the correctness of the program.
   *Rationale:* This is a a non-object oriented variant of Liskov
   substitution principle. In Sisdel this is actually the definition of
   a type, to differentiate it from units. This is the expected behavior.
4. Algorithms should by default be as generic as possible without
   violating the correctness of the program.
   *Rationale:* This is to ensure code reuse. This will help to keep
   the amount of needed code down, and will make sure there is less
   code that needs to be maintained and debugged.
5. Only interfaces shall be inherited, not algorithm.
   *Rationale:* Inheriting algorithm can easily result in subtle
   issues since the inherited algorithm wasn't written for the new
   interace.
6. Mutable states shall only be used where strictly necessary.
   *Rationale:* Mutable states make correctness validation harder, and
   should therefore be kept to the necessary minimum. It also makes
   the code easier to understand.

Goals
-----
1. The language syntax should be easy to learn and master.
2. It should be easy to understand the design when reading the code.
3. It should require significant effort to write code that isn't well
   behaving, intuitive, secure and robust.
4. When errors do occur, it should be easy to understand what made the
   error to occur.
5. The language should be a generic language, suitable for compiled
   applications, shell scripts as well as build scripts.
6. It should have good support for cloud computing, where different
   parts of the code runs on different hardware in a network.
7. It should offer good parallellization of execution.
8. The language should offer a robust and simple way for code re-use.

Features
--------
1. It shall be possible from outside as well as from the code to save
   the complete internal state of the program. This state shall be
   complete enough that it is possible to restart the program from
   this state. If communicating with external entities, the program
   shall be able to tell the external entities from where it is
   restarting. If the external entities supports this kind of restart,
   e.g. because they are implemented in Sisdel, a restart of a
   complete system shall be possible.
2. It shall be possible, from outside as well as from the code, to ask
   how a certain value was calculated. How fine grained the answer
   will be shall be determined by code and the request.
3. Value dependencies shall be tracked in a way that correct value
   types can always be determined at compile time, even when re-using
   code.
4. It shall be possible to store code elements in a cloud manner,
   where other programs may be re-using these code elements in their
   implementation. Each element shall have meta-data indicating what
   kind of characteristics can be expected from the code as well as
   how robust and well-tested it is.
5. Language syntax shall be as simple as possible, to make it as easy
   to learn as possible and also providing the least amount of
   surprises. The lazy programmer shall get a robust and easy to
   understand program, while it should require significant efforts to
   make the program unstable and hard to read.

High-level design decisions
---------------------------
There are many ways of implementing the goals above. This chapter
describes the way chosen for Sisdel.

The design decisions are numbered for easy reference:
1. Use functional oriented programming by default.
   *Rationale:* Functional oriented programming does not use mutable
   states, which supports principle 6.
2. Make imperative oriented programming an option.
   *Rationale:* There are cases when it is easier to talk about states
   rather than stateless functions. Therefore the language should have
   support for this. This should not be the default though.
3. Support constraint oriented programming.
   *Rationale:* This helps in making the default assumption that the
   code should be as generic as possible, while being possible to
   specify using constraints where this does not make sense.
4. All components that builds up the language shall be made first
   class citizens.
   *Rationale:* If a component exists in the language and is not
   sensible for having as a first class citizen, then this is an
   indication that the programming language isn't designed in the most
   intuitive way.
5. Data shall have a unit.
   *Rationale:* Units describes what data is compatible for cases
   where the types are compatible. This makes it possible to
   distinguish for example the value 1 representing an ordering number
   from cases when it is intended to be an index. Or a 1 meaning
   weight in kilogram as opposed to a 1 meaning weight in pound. But
   for all these cases, the same algorithms that does addition,
   subtraction, division etc can be used.
6. Context oriented parsing. For each token being parsed, the context
   for the next token is determined by previous tokens. So context
   changes not only by which block the code is in, but also where on
   the line the code lies.
   *Rationale:* This is similar to how natural language works.

Types
-----
At the heart of Sisdel there are the types. The types have a strict
hierarchy where each entity have exactly one parent, except for the
top-most entity which has none.

This is the tree of types being pre-defined by Sisdel:

Thing
 |-- Code
 |    |-- Constraint
 |    |-- Operator
 |
 |-- Value
 |    |-- Number
 |    |
 |    |-- String
 |         |-- Documentation
 |
 |-- Set
 |    |-- Map
 |    |    |-- Namespace
 |    |
 |    |-- Stream
 |         |-- Array
 |
 |-- Unit
 |-- Dependency
 |-- Security token
 |-- License
 |-- Author

Note that while the hierarchy describes what operations that can be
performed for each type, constraints and security tokens can be used
to further limit what operations that are allowed.

Description of each type:
*Thing* - The top-most type. It serves the purpose of saying "any
	type".
*Code* - Any piece of code that can be executed.
*Constraint* - Code the describes a constraint. Made its own type
	because together with type it is essential for describing an
	interface.
*Operator* - Code which optionally accepts an argument, and when
	executed produces a thing.
*Value* - Data that isn't natively interpreted as part of Sisdel
	itself, but rather data that is part of the program itself.
*Number* - Any number including integers and floats.
*String* - Unicode UTF-8 string.
*Documentation* - String attached to code to describe the code.
*Set* - An unordered collection of things. A set can be iterated, but
	there is no concept of "next" and "previous".
*Map* - A pair consisting of a key and a data, where the key is used
	as an index name to access data. If the key is an unsigned
	integer, it differs from array by being unordered.
*Stream* - An ordered collection which only has a "next" entry, no
	"previous". Especially useful for describing cases where a read
	alters a state, e.g. reading the next random number from a random
	number device.
*Array* - An ordered collection of things, having both "next" and a
	"previous" entry. Note that this includes linked list.
*Unit* - A tag placed on a type to describe compatibility. The unit is
	unique per type, i.e. same unit name used on incompatible types
	means different things. Unit does not affect what code is
	compatible with the type, only what other type that can be
	calculated together with this type.
*Dependency* - Describes from other things a certain thing was
	derived.
*Security token* - A token describing access priviledges, i.e. what
	operations that are allowed on that thing.
*License* - Describes what code can be mixed with what other code. It
	is also used for calculating the resulting license for the
	application.
*Author* - The copyright holder of the code.

Namespace
---------
All things reside in a namespace. Namespaces provides a way of
describing what you mean with a certain name, when this name can be
used in different places. For example, the type "client" might mean
different things in a business context from a database context, and
can therefore be placed in different namespaces.

When code refers to a name, namespace helps in determining what this
name really refers to. The current namespace is searched first, and
then any parent namespace until file scope has been reached. This
means that parent namespace is auto imported into child namespace
within a file, but not across files.

It is also possible to import namespaces across files, but this needs
to be done explicitly using "import" keyword. This also requires that
the other files have exported the namespaces.

There are a number of namespaces pre-defined by Sisdel:
*env* - This is the environment in which an application runs. It
	typically contains the environment variables stored by the
	operating system.
*arg* - The argument given when importing a namespace or when invocing
	an operator.
*thread* - Namespace given for this thread of execution. Note that
	this is not thread as seen by an operating system. An execution
	thread can extend with messages being passed around.

Contraints
----------

Constraints applicable for different types:

* Numbers
  - integer
  - unsigned
  - range [from <value>] [to <value>] [every <value>]
  - odd | even
  - size <value>
  - precision <value>
  - accuracy <value>
* string
  - upper-case
  - lower-case
  - normalized
  - range [from <value>] [to <value>]
  - size <value>
  - length <value>
* Set
  - size <value>
  - size [from <value>] [to <value>]
* Array
  - size <value>
  - size [from <value>] [to <value>]
  - sort-ascending
  - sort-descending
* security-token
  - once-only

Sisdel Object Repository
------------------------
There is a hierarchy of Sisdel object repositories to enable re-use of
Sisdel code. Each interpreter is aware of at least one top repository
from which it can import objects.

Each object in the object repository has some meta-data associated
with it:
* Author name, e-mail and organization
* License
* Test status, or "trustness"

It tries to solve the following high level goals:
1. Maximize software reuse on object level, to make full use of what
   others have written earlier. Similar thinking as cloud computing
   and network function virtualization.
2. Application should be able to tell its dependency needs to whoever
   it may concern, e.g. to interpreter, software package manager, end
   user.
3. When accepting a dependency, also consider object version,
   trustfulness, security, and characteristics in addition to pure
   functionality. Such constrictions are inherited.

To implement above high-level goals, the following low-level goals are set:
1. Class-less object orientation.
2. Inheritance only done on interface level, not on implementation
   level.
3. Interface descriptions are automatically derived from object
   implementation.
4. Let dependecies be first class objects.
5. Let restrictions be first class objects.
6. Restrictions should be able to describe object relations, object
   interface ID, object implementation ID, trustfulness, security
   considerations, and characteristics.
7. Have message paradigm rather than function call paradigm.
8. Let messages be first class objects.
9. Use immutable objects by default, to make parallelism easier.
10. All objects shall have an interface ID.
11. All objects shall have an implementation ID.
12. All objects shall have a trustfulness level, indicating how well
    tested this object is. This information is handled separately from
    the application, and is stored in a distributed database.
13. Keep the basic foundation of the language trivial, but designed in
    a way that makes it extensible.
14. Compiled as well as interpreted, decision can be made at run-time.

Syntax
------
The syntax is intended to be as easy to parse as possible, without
loosing readability. For readability, the most important aspect is
that what you read should be what will be executed, no surprises. You
shouldn't require experienced programmers to be able to read the
code. Ideally, even non-programmers should be able to guess what the
code does given that good symbol names was used when programming.

Regarding comments: These are seen as part of application
documentation, and are therefore defined using the object "#", hash
character. In cases where the documentation is not supposed to be
exported, the tripple hash can be used, i.e. "###".

### Scanner syntax in extended Backus-Naur format:
    (* All names below ending with “-token” are entities returned by
       the scanner *)
    program = { line } ;
    line = { token } , newln , line-continuation , line-indented ;
	line-continuation = { '...' , { token } , newln }
	line-indented = { indent-token , line }
    indent-token = { ? tab ? } ;
    token-sequence = { { token-separator } ,  symbol } ;
    token-separator = ( ? tab ? | ? space ? ) ;
    newln = ? new line ? , { ? new line ? } ;
    newln-token = newln ;
    token = number-token | string-token | comment ;
    number-token = [ '-' ] , ( dec-real )
	    | ( '0x' , hex-real )
	    | ( '0o' , oct-real )
	    | ( '0b' , bin-real ) ;
    bin-real = bin-unsigned , [ '.' , bin-unsigned ] , [ 'E' , bin-unsigned ] ;
    oct-real = oct-unsigned , [ '.' , oct-unsigned ] , [ 'E' , oct-unsigned ] ;
	dec-real = dec-unsigned , [ '.' , dec-unsigned ] , [ 'E' , dec-unsigned ] ;
    hex-real = hex-unsigned , [ '.' , hex-unsigned ] , [ 'E' , hex-unsigned ] ;
    dec-unsigned = dec-digit , { dec-digit } ;
    hex-unsigned = hex-digit , { hex-digit} ;
    oct-unsigned = oct-digit , { oct-digit } ;
    bin-unsigned = bin-digit , { bin-digit } ;
    bin-digit = '0' | '1' ;
    oct-digit = bin-digit | '2' | '3' | '4' | '5' | '6' | '7' ;
    dec-digit = oct-digit | '8' | '9' | ' ;
    hex-digit = dec-digit | 'a' | 'b' | 'c' | 'e' | 'f'  ;
    string-token = ( '”' , { valid-consecutive-char } , '”' ) 
        | ( valid-first-char , { valid-consecutive-char } ) ;
    valid-first-char = ? printable chars except ”
	    - 0 1 2 3 4 5 6 7 8 9 ? ;
    valid-consecutive-char = ? printable chars except “ ?

### Parser syntax:
    (* All names below ending with “-token” comes from the scanner *)
    object-create = [ scope ] , object-instance , [ object-parameter ] ;
	scope = object
	object-instance = object
	object-parameter = object
	object = value | string | object-reference
	object-reference = string

### Examples:
    first is program option @ 0
    second is program option @ 1
    stdout print first , “ * “ , second , “ = “ , first * second ,
    ... newln

Scope
-----
<Merge this into namespace chapter above>

Sisdel is heavily scoped. Each token creates a new scope, each new
line resets scope to the same scope as the beginning of the line. When
indenting, the scope created by the whole line preceding the
indentation defines the scope for any following indented lines. And
this can be done recursively, creating a scope hierarchy.

Each token is also an object. Even an immediate constant, like "1", is
an object and can be assigned meta-data like unit. When the
interpreter reads a token, it creates an object. This object creation
is done within the current scope, and can optionaly have an argument. The
created object will then be the scope for the next object creation and
so on until a new-line with no following indentation occurs.

Undefined object names are objects as well, which is used by the
object "is". "is" expects current scope, i.e. left hand side, to be an
undefined object name, while the expected argument can be
anything. "is" will then define the current scope to contain
everything within the argument.

For example, the following:

    greeting is "Greetings to you!"

will create the object name "greeting" to refer to the string object
"Greetings to you!". An interesting things about Sisdel is that you
can always make this the reverse, like this:

    "Greetings to you!" is greeting

This will create the object "Greetings to you!", and assign it the
value "greeting".

The object "," can be used for creating a set. "," will merge current
scope with its argument into a set. If "," is used repetively, the set
is extended. I.e. "1 , 2 , 3" creates one set consisting of the
objects "1", "2" and "3". If this set creation uses several lines, use
"..." continuation token for this.

It is also possible to create sets using indentation, e.g. we could
assign the object i to contain the set of objects "1", "2" and "3"
like this:

    i is
        1
        2
        3

Note that the two following lines does different things:
    weight is 5 unit kg
	weight is ( 5 unit kg )

The first line will define object "weight" to be a copy of object
"5". It then creates a new, unnamed object based on a copy of "weight"
and assigns the unit "kg" to it.

The second line will define object "weight" to be a copy of the object
created by the object "5" with the unit "kg".

Argument
--------

An optional argument can be given both when importing a namespace as
well as when invocing an operator. This argument is accessed by the
"arg" namespace.

Only one argument can be given, but this argument can be a container
(set, array, sequence, etc) of things as a way of providing multiple
arguments.

The comma character is used to specify a set, which means that the
following works as a way of specify several arguments:

    add-all 3 , 5 , 7 , 10

The type of the argument may be restricted further, if needed:

        two-ints is ( set ( integer , size 2 ) )

Type inference
--------------
Types can be explicitly specified, which is seen as a restriction
being put on an identifier. If no type is specified for a function,
then the valid set of types is determined based on
implementation. When function is then invoked, a check is done that
the parameters supplied to the function matches on of the types found
in the valid type set. Each invocation will use the supplied type,
making it possible for the function to adapt its behavior based on the
types of the calling parameters being used.

Parallel programming
--------------------
Sisdel is designed to give as good opportunity for parallellization as
possible. The language is declarative with good dependency tracking
which also spans over public interfaces to make this possible.  The
goal with Sisdel is that how and how much the code is parallellized is
something that can be decided late, and also be changed without
changing the core algorithm.

Imperative programming
----------------------

Sisdel is functional by default, but it is possible to use imperative
programming style as well. This is accomplished by setting the
constraint "mutable" on an object. An object with this constraint can
have snapshots, each such snapshot producing a snapshot
object. Example:

    integer myobj is mutable
    myobj assign 1
    print myobj snapshot
    myobj assign 2
    print myobj snapshot

This will print "12" on the screen. Normally the order of the lines
are not important, but when "snapshot" and "assign" are used, the
order becomes important. Sisdel will adapt to this automatically.

Due to type inference, the first line of code above can actually be
omitted. The "assign" object expects a mutable object, which means
that this constraint will be inferred to "myobj".

Security model
--------------
Sisdel supports having security tokens that are passed implicitly when
functions are called. A security token can be created, checked and
consumed.  The reason for passing the security tokens implicitly
rather than by having them as arguments in function calls is to be
able to use the same overall algorithm even though there might be some
special case needing a security token.  One securit concern is the
fact that all objects are dependent on the scope in which they
exist. It is possible to change this environment in a way that would
create security problems. For this reason Sisdel supports
signatures. Signatures are used to verify that a certain object comes
from a known supplier.  Using object versioning the security can be
tightened even further, but requiring a certain version,
i.e. implementation, of an object. This creates a trade-off between
flexibility and predictability.

Trust Model
-----------
No code of any significance can be fully tested to ensure there are no
bugs. It seems theoretically possible to verify the code against a
specification, but this only moves the problem to achieving bug free
specifications.  The implementer might not be fully aware of what is
required of the software to fully support the users expectations, and
the users themselves might not be able to provide this information at
the design stage. Most users however are capable of saying what they
believe is a functional behavior as opposed to buggy behavior.  This
is why trust model is introduced. They idea is a rapid prototyping
approach, where a first hack of the software is done and then tested
by the users. The users will then need to provide information about
what is buggy behavior and what is functional behavior. This needs to
be handled by the language by determining what dependency chains that
get the thumbs up, and what dependency chains that get the thumbs
down.  This process can also be done by test systems, manual as well
as automatic. By analysing the dependency chains, each dependency can
get a trustfulness score. Three areas are of interest: Those
dependencies that get very high trustfulness scores, which indicates
good quality. Those dependences that get very low trustfulness scores,
which indicates a problem area. And lastly, those dependencies that
does not have much information at all, which indicates either
redundant code, or code that needs more testing.  Since each object
can be upgraded, leading to new dependencies, this whole process is
continual. There needs to be a way of collecting this statistics, and
this collection might involve security and privacy concerns that needs
to be handled.

Object versioning
-----------------
Each object in Sisdel is first translated into an intermediate format
used when interpreting. When an object is in this intermediate format
a checksum is calculated. This checksum is then used as the objects
version.  This is used in a distributed environment to detect objects
that has been changed and to detect when two objects are actually the
same object.

Interpreted vs compiled
-----------------------
Sisdel code is always translated into an intermediate form. It can
then be executed directly from this intermediate form, which is called
“interpreted” in this document. Alternatively it can be compiled to a
machine native format and then executed in this form instead, which is
called “compiled” in this document. There is no source code difference
for these two cases, and it is also possible to go back and forth
between these two cases at any time.  The reason for compiling is
mainly for performance reasons, but could also be used to make it
harder to change the design by mistake.

Dependencies
------------
Dependencies are important to Sisdel. They are also visible to the
programmer to enable good error messages and opportunities for runtime
optimizations.  The dependencies spans over public interfaces, and can
therefore be tracked through third-party libraries. They can even be
made visible to the application invocer, which is useful for building
scripts and determining environment dependencies.

Interfaces
----------
There are no separate interface files associated with the source
code. Sisdel can however produce interface files when compiling or
interpreting the code. These auto generated interface files are used
by Sisdel to determine how library code works and what dependencies
and constraints it has. They could also be used by external utilities
to determine dependencies.

Constraints
-----------
Each expression in Sisdel can have constraints, saying when the
expression applies. It can also be used to put requirements on values.
Sisdel Foundation Sisdel is modelled around value. Value is something
that contains some data along with some meta-data describing what that
data is and how it is encoded. Value contains: Reference to a scope
where the value is. This might be a function scope, a collection,
parameter to a function or thread scope used for passing around
security tokens.  Encoding. This tells how the data associated with
the value is read. It could be UTF8 when data is a string, or
little-endian 64-bit unsigned integer if value is a number.
Unit. This is a high level description of what the value is. It could
be used to describe physical units like metric tons, seconds and so
on. Or it could be ordering number, base number, process ID or user ID
to just give a few examples.  Piece of data. This is the actual
storage of bits of information. This might be zero size if value is a
reference, or dynamic in size if value is a string.  When an operation
is done on value, the same operation is done for data and unit. A
function either tells how to calculate parameters of given data with
encoding, while other function can specify how to handle the parameter
units.  So for an integer addition, one function says how to add two
numbers together given data and encoding, while another function says
what unit you will end up having when adding the two units
together. It is of course possible to have a function that can handle
it all.

Pre-defined values
------------------
Sisdel has a number of pre-defined values that can be used to build
further values. These objects has special semantics important to the
language itself, which is what differ them from standard library
objects.  dependency – Describes how one value depends on another.
root – This is the language scope, where all primitives resides.
param – When a function is called, Sisdel will store all parameters
into a container called param. All arguments to the function can be
found there.  thread – Scope specific to a thread of execution. Note
that a Sisdel thread is more than an OS thread, since a Sisdel thread
encompasses remote calls. Sisdel makes no syntactic difference for a
local function call or a remote function call.  reference – A name
which refers to a value.  map - Associates a key with a value. Both
key and value can be of arbitrary types, which can be different for
each entry. Maps can be used for anything from structs, which is only
a static version of maps, arrays where the key must be integer and all
values must be of same type, or scope where key is of type
reference. Maps containing maps can be used for achieving
multi-dimensional arrays.  fifo, stack - Serializes accesses between
readers and writers. For fifo, reads are done in the same order as
writes. For stack, reads are done in reverse order compared to writes.
string - A sequence of characters. Each encoding is a sub-object of
string.  integer - An integer value. It can be constrained to have a
specific range, e.g. unsigned.  collection - An unordered collection
of objects. Can be constrained to only represent specific types of
objects.  Constraints A constraint in Sisdel is basically an
assignment to a value. This assignment might be an immediate value,
making it very constrained. Or it could assign a certain type to
it. It can be used on parameter values to indicate valid types, value
ranges, units etc.  Since a value cannot change once it's set, an
assignment that would break this would cause a failure. If this
happens with a function, another function with the same name is tried
instead. If no such function exists, then this becomes a parse
failure.

Value passing
-------------
Objects can return values explicitly, but will always return success
or fail and optionally a security token implicitly. The success or
fail status is used to affect execution paths while the security token
is used to determine object use permissions. It is valid to possess an
object for which you do not have permission to request the value
of. Further permissions affects other aspects of the object such as
dependencies.  Each value has the following meta information:
Dependency on other values, including constraints. The dependency
includes version number and value id.  Version number of this value.
Value ID, which is the actual value itself if its small or a checksum
of it if the actual value is large.

Execution environment
---------------------
Sisdel can either be compiled or interpreted with no change of the
source code. The decision can be made on per file basis and an
application can consist of a mix of both with no change of the source
code itself.  There is a distinction between execution context and
declaration context. Security tokens and success/fail status follows
execution context while objects and signatures resides in declaration
context.

Brain-Dump Area
---------------
Namespace, Scope and Context The terminology used in this document is
as follows: Namespace is the scope within where the code is defined,
which usually translates to a source file.  Context is the running
scope, which follows how code is executed.  Scope can either be a
namespace, a context or the combination of both.  Identifiers declared
in a namespace encompasses the namespace within which they are
declared. For identifiers declared in a context, they are valid within
the context within which they are declared.  “namespace” will select
current namespace, and can be used as a pre-pend for an identifier
declaration to declare it in current namespace. “context” can be used
in a similar fashion.
