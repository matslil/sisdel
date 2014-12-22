Simple Syntax Dependency Language (Sisdel)
==========================================

Goals
-----
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
This chapter describes how the goals have been implemented, and the
rationale for the design decisions.

First, these are the design decisions, numbered for easy reference:
1. Use a class-less object orientation, albeit without support for
   inheritance.
2. Make a functional programming paradigm the default, and imperative
   programming paradigm possible as an option.
3. All components that builds up the language shall be made first
   class citizens.

Sisdel Building Blocks
----------------------

Sisdel has the following basic building blocks:

Built-in data types:
* real numbers
* string (Unicode, UTF-8 by default)

Built-in containers:
* map
* set
* list

Built-in meta data:
* type
* constraint
* dependency
* method



The smallest re-usable entity in Sisdel is referred to as an
object. Note that objects in Sisdel might be similar to objects in
traditional object oriented languages, but Sisdel lacks some
properties. For example, Sisdel objects does not inherit
implementation, only interface.

All Sisdel objects have the following properties:
* *value* --- Could be an integer, map, method, string, dependency,
  type, unit, constraint, reference.
* *inherited from* --- Builds an inheritance tree, which usually ends
  with a type object.
* *uuid* --- A hash code that uniquely identifies the objects value.

There is a set of basic objects pre-defined by the language:

### Constraint Object

Merely a set of rules describing constraints on the object value.

### Reference Object

A valid identifier name, and a reference to an object.

### Type Object

Contains a reference object and a constraint object.




Sisdel vs Object-Oriented Languages
-----------------------------------

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

Sisdel is an experimental language intended to examine how to make
dependency tracking as accessible, useful and simple as possible. It
tries to solve the following high level goals:
1. Maximize software reuse on object level, to make full use of what
   others have written earlier. Similar thinking as cloud computing
   and network function virtualization.
2. Application should be able to tell its dependency needs to whoever
   it may concern, e.g. to interpreter, software package manager, end
   user.
3. When accepting a dependency, also consider object version,
trustfulness, security, and characteristics in addition to pure
functionality. Such constrictions are inherited.  To implement above
high-level goals, the following low-level goals are set:
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

Scanner syntax in extended Backus-Naur format:
(* All names below ending with “-token” are entities returned by the scanner *)
program = { line } ;
line = indent-token , token-sequence , { newln , '...' , token-sequence }
	, newln-token ;
indent-token = { ? tab ? } ;
token-sequence = { { token-separator } ,  symbol } ;
token-separator = ( ? tab ? | ? space ? ) , { ? tab ? | ? space ? } ;
newln = ? new line ? , { ? new line ? } ;
newln-token = newln ;
symbol = identifier-token | number-token | string-token | comment ;
identifier-token = valid-identifier-first-char
	, { valid-identifier-consecutive-char }
	(* must not be '…' *) ;
valid-identifier-first-char = ? printable chars except ”
	- 0 1 2 3 4 5 6 7 8 9 ? ;
valid-identifier-consecutive-char = ? printable chars except “ ?
number-token = [ '-' ] , ( dec-unsigned , [ '.' , dec-unsigned ]
	, [ 'E' , dec-unsigned ] )
	| ( '0x' , hex-unsigned , [ '.' , hex-unsigned ] . [ 'E' , hex-unsigned ] )
	| ( '0o' , oct-unsigned , [ '.' , oct-unsigned ] . [ 'E' , oct-unsigned ] )
	| ( '0b' , bin-unsigned , [ '.' , bin-unsigned ] , [ 'E' , bin-unsigned ] ) ;
dec-unsigned = dec-digit , { dec-digit } ;
hex-unsigned = hex-digit , { hex-digit} ;
oct-unsigned = oct-digit , { oct-digit } ;
bin-unsigned = bin-digit , { bin-digit } ;
bin-digit = '0' | '1' ;
oct-digit = bin-digit | '2' | '3' | '4' | '5' | '6' | '7' ;
dec-digit = oct-digit | '8' | '9' | ' ;
hex-digit = dec-digit | 'a' | 'b' | 'c' | 'e' | 'f'  ;
string-token = '”' , { valid-identifier-consecutive-char } , '”'
comment = '#' , { ? printable char ? } , newln ;
Parser syntax:
(* All names below ending with “-token” comes from the scanner *)
program = { expression } ;
expression = value , { newln-token , indent-token , value }  newln-token ;
value = identifier-token (* procedure *)
	| number-token
	| string-token
	| value , { ',' , value } (* list *)
	| '(' , { value } , ')' (* precedence change *)
	| value , identifier-token (* unary operator *)
	| value , identifier-token , value (* binary operator *);

Examples:
first is program option @ 0
second is program option @ 1
stdout print first , “ * “ , second , “ = “ , first * second ,
... newln

Scope
-----
A scope is given by an object name. What other languages call
“namespace”, are in Sisdel just a container of objects. A sequence of
object names will refine the intended scope. I.e., “a b c” would mean
“in object a, there is object b, wherein there is object c”. The last
object in such a sequence is typically a message object.  The language
is heavily scope oriented, where a white-space is the scope
operator. It also supports the following kinds of functions:
unary-operator: Operates on the scope, i.e. on the left hand side.
binary-operator: Operates on the scope as its left hand side, and
expects one parameter as its right hand side.  Scope itself is an
unary operator, returning itself as contained in its parent. Assuming
“myscope” is a scope, and “my-operator” is a binary operator, the
following code: myscope my-operator “a parameter” is read as: myscope,
returns itself making “my-operator” accessible. my-operator then uses
this scope and then applies “a parameter” on it.  Everything in the
language is centered around “value”. Values as per Sisdel definition
means something that has the following properties: Lives in a scope Is
created from another value, which could be an operator invocation
Unit, indicates high level compatibility Coding, indicates how the
data area of the value is interpreted Data, the actual value

Example source:
repeat-message is binary-operator
	doc head-line “Repeat left-hand side right-hand side number of times”
	self repetitions-of print param
myRecord is set-of ( name is string UTF8 , age is integer unsigned , times )

Sisdel overview
---------------
The following buzz words can summarize Sisdel: Class-less object
oriented language with some limitations, e.g. no inheritance.
Functional (declarative) by default, but with limited support for
mutable values.  Strongly typed with type inference.  Supports
parallel programming using threads, SMP and distribution.  Implements
security features using security tokens and signatures.  Uses
checksums to detect changed objects.  Supports late decision of what
is interpreted and what is compiled, with no source code changes
needed.  Keeps track of dependencies and making them visible in the
language.  Interfaces are auto-generated with enough information to
enable good optimization support.  Supports constraints.  For each
bullt above a corresponding sub-chapter will explain it in more
detail.  Object model Sisdel is not a truly object oriented language,
although it has something it calls objects. There are no
classes. Instead, there are objects that act as object factories.
Inheritance is also not supported. The type inference is meant to
replace this, which by default means that there is a looser definition
of type compatibility. It is however possible to place additional
constraints which could mimic the limitations inheritance would put on
type usage.  There is also no overloading. Using type inference types
does not need to be specified. This should give enough flexibility
that there should be few cases when overloading will be missed.  All
function calls uses value passing. This means that no references are
being passed for a function call. Sisdel may however choose to use
reference whenever it doesn't change the semantics if this will
produce a more optimized code.  Functional Everything in Sisdel is
declarative unless otherwise specified. This means that loops does not
mean that the code actually should implement a loop, but rather should
be interpreted as “for this range the following should apply”. This
gives better optimization and possibility to implement parallell
algorithms automatically.  When states, or mutable values, are
actually needed they can be used in Sisdel with the limitation that
these cannot be exported by other means than as a snapshot value given
in a function call.  Everything in Sisdel are objects in Sisdel's
definition of the term. Functions are objects, and can therefore
implement mutable values. Each call to such a function might produce
different results even when the same set of argument values is
used. Sisdel will in this case treat each function call as though you
are calling different functions. This is similar to what Haskell calls
monads.  Sisdel uses promises for return values to make function calls
less synchronous. This means that there are no guarantees that a
functional succeeded when a receiving a return value. When the return
value is being looked at the function success or fail will be
visible. This is implemented as special values that the return value
can have.

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
