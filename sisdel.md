% Simple Syntax Dependency Language (Sisdel)
% Mats Liljegren

Sisdel is an experimental language intended to address modern
programming language challenges. Current trend within programming is:

- Internet of things, where every little piece of electronics is
  connected to the Internet generating information.
- Cloud computing, where computing resources is centralized and
  virtualized to make the most effective use of hardware.
- Private information like health status is being stored in the cloud
  where multiple agents needs access, e.g. family members, doctors
  etc.
- Code is being more and more used in safety critical applications,
  e.g. safe driving cars.

All this means that information is becoming very central, since it is
information that can cause privacy, security and safety
concerns. Information carries legal implications like copyright,
license conditions, and ownership. It also becomes important to track
trust and being able to revoke rights to information that you own.

Algorithms are a lot more volatile and can be executed at arbitrary
places determined by third parties. Algorithms are information in
themselves, meaning they carry copyright, license conditions,
ownership and trust. Execution can be parallelized and distributed
over the Internet.

Why Sisdel?
===========
Sisdel is data driven, dependency tracking and declarative
language. It has evolved from the observation that data survives way
longer than algorithm and many more problems stems from data rather
than algorithms.

Once you have collected data around something you want to save it for
the future. But how do you know that you can still correctly interpret
old data, so that you can compare it to new data being collected using
more modern methods and storage?

There is also the question of dependency. When an application returns
some data that appears out of the ordinary, it is often quite valuable
to be able to backtrack how that data was calculated, or at least,
what other data it was calculated from.

Some data might also have privacy concerns. Contacts might be fine to
share with close friends, but not to total strangers. How do you
ensure this when a third party provides the service of storage and
access to this data?

Concurrency problems usually stems from how data is handled, since the
usual bottleneck is that too much data is being shared too often. It
is therefore important to keep data as local as possible, to avoid
unnecessary sharing.

Code itself is also data, and this data has trust issues. How do you
know that the code is robust enough for your intended application?
This is often a balance between functionality and correctness. Using a
central repository for all language modules, this can also be used to
store trust related meta-data about these modules.

Code also has an author, and some companies might want to restrict
what author or authors that are allowed to provide modules to the
companies application. This can be for trustfulness reasons or business
reasons.

The author for the code also decides a license. When an application
consists of several modules collected from the Internet, all their
respective licenses combines and dictates requirements on the
application's license. How do you ensure this is done correctly?

Note that data created by a user of an application can also have an
author and a license. If so, Sisdel can help in ensuring the license
is obeyed as long as the license can be described in a way Sisdel can
understand.

Programming language challenges
-------------------------------
My first intention with Sisdel was to create a better "make" language,
hence its name. With time, the scope has broaden to not only solve
dependency problems. My current list of things I want Sisdel to be
good at is:

* *Security*. Code floats around the Internet and it can sometimes be
  hard to know what code to trust. This can either be whether the code
  is malicious or not or it can be whether it has been well tested or
  not.
* *Privacy*. We share personal data in order to get service back. But we
  want to know that this data is not used in a way we do not approve
  to. This includes malicious developers as well as eavesdropping third
  parties.
* *License*. All code is published with a certain license, and when code
  with different licenses are mixed, the application will have a
  license that is mixed from the code licenses. This can be hard to
  deal with, since there can be a number of licenses and even greater
  number of combinations.
* *Dependency*. Having dependency as a first class citizen in the
  language makes a lot about the environment visible. Error messages
  can be made more intelligent when some trace about what a certain
  value was calculated from, different parts of the code can by
  themselves both declare dependency as well as check for dependency
  giving the programmer a new tool for designing his or her
  application.
  Dependencies also has weights, which is essentially a value saying
  how much work it is to recalculate a certain value from another
  value compare to other dependencies. This value is most heavily
  affected by communication links, a slow link gives more penalty than
  a fast one. Passing module boundaries also give some penalty.
* *Concurrency*. Processing power is increased more by adding
  concurrency in the hardware rather than making the processor
  faster. The language should therefore encourage concurrent code.
* *Distributed*. Applications are now distributed among one or several
  clients and servers all working together. The language should make
  this easy, handling problems like reconnecting from a lost
  connection. The language should help the developer to take a late
  decision on what node that should perform what work, preferable this
  should be a runtime decision based on current load.
* *Time*. There needs to be a conception of time in the language. It
  must be possible to describe it in several ways, calendar time,
  passed time, execution time and ordering requirements. All these
  variants of time should be compatible with each other, so that
  different parts using different notations can still work together.
* *Modularity*. It must be able to split the design into several
  modules, with well defined interfaces between them.
* *Design freedom*. It must be possible to describe the design in several ways:
    - Event machine
    - State machine
    - Declaratively
    - Logically, similar to Prolog
* *Unambiguous*. If the code is not unambiguous, the compiler will
  generate a compile error and require that code is added to make it
  unambiguous. E.g. if the code can be parallelized but depending
  on how can give different results, than this will give a compiler
  error.

The above together with some firm believes regarding language design
is what has led to Sisdel.

Language Design
===============
When describing a design it is customary to first describe the
intended use cases. For Sisdel, this is kind of tricky, since on a
higher level this is quite obvious: Write code that when executed does
something. On a lower level, only the sky is the limit, and Sisdel
should put no restriction on its use unless necessary.

The chapter [Why Sisdel?] could be seen as a number of use
scenarios that the language intends to fulfill. The next section in
this chapter, [Principles], gives some collected wisdom
with current programming languages.

Next section, [Design decisions], describes design
decisions that have been made and the rationale for each of them.

Principles
----------
Principles are statements that should guide the language design. These
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
   interface.
6. Mutable states shall only be used where strictly necessary.
   *Rationale:* Mutable states make correctness validation harder, and
   should therefore be kept to the necessary minimum. It also makes
   the code easier to understand.

Design decisions
----------------
This section describes design decisions taken for Sisdel. The design
decisions are numbered for easy reference:

1. Use functional oriented programming by default.
   *Rationale:* Functional oriented programming tends to create less
   unneeded complexity, and should therefore be the default.
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

The foundation of Sisdel
========================
Everything is information, be it a string, a value or an algorithm. In
Sisdel this is referred to as a "thing". For things, the following is
being tracked:

* *Owner*, who owns the information?
* *Access token*, who have what kind of access to the information?
* *Trust*, for algorithms this is how proven the algorithm is, i.e. how
  bug free can you expect it to be? For data this is how certain can
  you be that the information is correct?
* *Dependency*, i.e. from where was this information calculated? Since
  ownership, licences, access rights et.c. can change it is important
  to keep track of how the information was calculated in case this
  affects this information. E.g. if some information is suddently
  deemed to be not to be trusted, this might affect other information
  as well.
* *Interpretation*, how is this information interpreted? This includes
  the type, i.e. whether the information is an algorithm, a string, a
  value, what kind of unit is has, and what constrictions that apply.
* *Storage format*, i.e. how is the information packaged? Includes
  things like endian, number of bits used, alignment requirements and
  other hardware and/or protocol related concerns.

Tracking ownership
------------------

So how do you decide who should own the information? And how to you
store the identity of the owner?

It might be the hardware that best knows who owns the information,
since it might be the owner of the hardware that should also be the
owner of the information. But this means that the hardware must have a
way of telling who this is, requiring some kind of agreed protocol.

Currently, no such protocol has been developed, so it becomes the
responsibility of the application to decide this. Between applications
or even between different parts of the same application, the protocol
is decided by Sisdel.

For Sisdel the ownership uses GPG, GNU Privacy Guard. It is the public
key fingerprint that is used for the owner identity.

Tracking access token
---------------------

An access token is needed to access information. Each information can
have any number of access tokens, and access tokens can be added or
removed compile time as well as runtime.

Each access token contains:

* The access right or rights that it grants.
* The number of times it can be used, unless unlimited.
* The user that may use the token, unless anyone is allowed to.

The access rights tracked by Sisdel are:

* Right to *read* the information.
* Right to *copy* the information.
* Right to *execute* the operator.

Note that Sisdel is a copy by value language, which means that the
copy right also gives the opportunity to create a new piece of
information containing part of the original information with own
modifications. Unless the read access right has also be granted the
original information may still not be read, even when it is part of
some new information.

Copy access right also gives the right to delegate whatever access
rights currently granted to some other user.

The access rights affects the actual data, the interpretation of the
data, and dependencies. The other meta-data, i.e. owner, access
tokens, and storage format can be read by everyone. The right to copy
also means that the storage format can be updated, since this is
needed to fulfill the protocol being used when copying. Only the owner
can change owner and access rights. Trust is determined by the user
referring to the information and is not part of a copy.

Tracking trust
--------------

Trust is a wrapping entity and not really a part of the
information. When the information is copied trust is not part of the
copy.

Each user has a database of what user they assign what trust to. Any
information from those users are assigned this trust level. Parts of
the application can then apply a trust constriction, i.e. require that
some information must have some level of trust to be accepted.

Tracking dependencies
---------------------



Tracking interpretation
-----------------------

Tracking storage format
-----------------------


The following types are the primary types for Sisdel:

* Thing
* Data
* Type
* Operator
* Message
* Constraint
* Transaction
* Module

![Hierarchy](type-hierarchy-graph "Sisdel type hierarchy")

![Relations](type-relation-graph "Sisdel type relations")

### Thing - Root type
This is the root type from which all other types are derived. Its
mainly used to state "any type".

### Data - Primary type
Data describes what is stored.

### Type- Primary type
A type describes how something is to be interpreted. It tells whether
Sisdel should treat an identifier as being data, operator, etc. It
also says whether one piece of data is compatible with another, and
also how to parse the data. A string might for example be stored as
pure ASCII string or as UTF-8 Unicode string. The type will tell which
it is so the code knows how to read the string.

If one type is expected but another is provided, a typecast can
occur. These can be either implicit or explicit. Only typecast being
defined can be implicit, all typecasts that Sisdel offers built-in
must be called explicitly.

### Operator - Primary type
Operator receives a message containing data and acts on it. It normally
produces data that is sent back again.

### Message - Primary type
When an operator is called with parameters, these parameters are
packed into a message by Sisdel and then sent asynchronously to the
operator. When the calling code needs the return code, which in simple
cases could be a simple success or fail, it will wait for a reply
signal from the called operator. This enables Sisdel to schedule other
activities in between to ensure better support for multiple processor
or nodes.

The message sent to the operator is called "request" and the message
received from the operator is called "reply". The name for "request"
and "reply" is the same as for the operator.

### Constraint - Primary type
Constraints can be put on anything to describe limitations of its
use. These limitations are typically not intended to describe what can
or cannot be done, but rather what is allowed. Constraints describe
security, privacy and optimization concerns.

Constraints can be given in two ways: Using "allow", "deny" and
require, or using "when". This gives slightly different
results. "allow" and "deny" will put restrictions on the use of the
associated data, while "when" will put restrictions on when the
statement will be executed.

For example, to specify what types are accepted for the current
operator:

    arg iterations require range ( 1 to 5 )

Or only use the "security-module" if it was authored by the correct
author:

    mod is use security-module when author "Mats Liljegren"

The difference here is that "allow", "deny" and "require" results in
compile or runtime error if constraint is not fulfilled, while "when"
only results in code not taking any effect if the constraint is not
fulfilled.

### Transaction - Primary type
When an operator is invoked, a transaction is created. This
transaction is a branch from the current executing
transaction. The transaction contains the current execution scope,
which includes arguments given to the operator.

When the called operator has finished successfully, the created
transaction has been fulfilled, and the transaction will return a
value. If the called operator fails, the created transaction
fails and no value is returned. Unless the current transaction handles
this failure, this will cause the current transaction to fail as well.

The code can add data to the current transaction, which is typically
used for security tokens. This way of supplying arguments to an
operator is useful since some operators might not care about security
tokens and will therefore not forward them, but the transaction itself
will.

A security check can then let a transaction fail due to failing
security check. Failing a transaction is similar to generating an
exception in object oriented languages.

### Module - Primary type
A Sisdel application is built out of one or more modules. The
application always starts with a module being selected as the main
module for the application. This module may then import or use other
modules.

A module is expected to be a versioned file or tree of files. Current
implementation has support for GIT integration, but other versioning
system should also be possible to use.

Importing a module means to include it during compile time, while
using a module means to refer to the module during compile time, and
include it during runtime.

A compile module will keep track of what version of imported or used
modules that was used. Only this version of the modules will be
accepted when running the application.

It is possible to specify upgrade path to allow the application to
automatically use a newer version of used or imported modules. This is
code specifying how to translate data from old version of the used or
imported module to the new version. In order for the upgrade to be
seamless all the old version of the public operators needs to be still
defined but now accepting the new version of the data. If this is not
true, then the application needs to be updated as well.

Derived types
-------------
The following types are derived from some other type.

### Scope - derived from data
A collection of data and operators which are used when resolving
identifier names while parsing the code. Scopes are
hierarchical.

The root scope has module parameters used when invoking the module and
the "use" operator. For root scope the "this" sub-scope will be empty.
By using the "use" operator the rest of the language can be
imported. The reason for this arrangement is to be able to version the
language support, since the version of the language is part of the
name supplied to the "use" operator. E.g.:

    use sisdel-v1

Above makes all exported identifiers in "sisdel-v1" available in current
scope as though they were defined natively. You can also have them in
a named scope:

    lang is use sisdel-v1

In this case all identifiers exported from "sisdel-v1" are accessed
through "lang" scope.

### Value - derived from data
A value is something made up of digits. It can be a float or an
integer.

### String - derived from data
String is a sequence of characters and digits, usually meant to be
read by a human at some stage. Identifier names in the code are themselves
strings. Sisdel uses UTF-8 to store the string.

### Unit - derived from type
Unit is used to declare type compatibility when there is no other
hints about it. It can be used to describe that a certain integer is
actually an age given in years, or that a string is actually a
person's name. This way other integers representing other things can't
be confused with ages, or other strings meant for other things can't
be confused with names.

It is possible to specify unit compatibility such that given a certain
unit there is a way to calculate what it would become in another
unit. This can for example be used to express relationship between
kilogram and gram, saying that 1 kilogram can be converted to 1000
gram.

### Storage - derived from type
Storage specifies how data is stored. This can express e.g. that data
is private, and therefore is expected to be encrypted. It can specify
storage size and alignment when stored in memory, and also endianess
for integers.

### Request - derived from message
The request message is created by sisdel when an operator is
invoked. This is the argument(s) given to the operator plus the data
being operated on, i.e. the right-hand side and left-hand side of the
operator expression. The operator implementation can then access these
arguments as read-only data using "arg" scope for the arguments and
"this" for the data being operated on.

### Reply - derived from message
The reply message is created by sisdel when entering operator
implementation scope. The implementation can assign data to this reply
message using "result" scope.

### Size - derived from storage
Specifies storage size, which can be specified in multiples of other
types, or in bits or bytes.

### Format - derived from storage
How the data is stored. For integers this might be float or two's
complement integer, and for strings this could be UTF-8, ASCII, UTF-16
and so on.

Several format might be combined, e.g. big-endian two's complement
integer.

### Alignment - derived from storage
The address alignment for the data when stored in memory. This is most
useful for device drivers.

### Private - derived from storage
Private is about protecting sensitive data. This storage directive will
guarantee that the information is not leaked outside current
scope. This guarantee is more than simple compile or runtime error if
code outside scope tries to peek into the information, it will also
use encryption or other means to make sure the information is
secure. Note that privacy restriction should only be used when
necessary, since it will limit debugability and might also have some
heavy performance costs. The current implementation would use GPG for
the encryption.

Note that private storage expects a user context, meaning that a
user context must first be created. This way many different privacy
data can be created using the same user, and this specific user may
still see all of this data. This user may therefore see all of this
when debugging as well.

### Valid-value - derived from constraint


### Author - derived from constraint
This is a restriction that can be used to assert that code is written
by specific author, or data belongs to a specific author. In the
latter case author means who entered the data.

The current implementation for the check is to use GPG and SKS
protocols, but could be done with other protocols as well. An
authenticity restriction also implies an integrity restriction, i.e. a
guarantee that the code has been unaltered by someone else than the
original author.

### Access - derived from constraint
Access restriction is a security token model to limit access to
primarily data, but could restrict other things as well like
scope and operators. The restriction can be lexically or
transactional, and can be limited by number of times of use.

### Token - derived from constraint


### License - derived from constraint
Sisdel requires that all code has a defined license. Each type of
license has a compatibility with other licenses, where two licenses
being combined might create a third license which then could be the
license for the application being built.

License in the language is a restriction that can be used
programmatically as well. It is possible to describe that certain data
or operator must have a certain license.

Food for thought
----------------
Molecular programming: Let the code organize itself. Code is
constructing code, which construct code, ad infinitum. In this process
data processing might take place as well, performing some function.

This should make adaptive programming possible, i.e. code that changes
itself as an answer to what problems that needs to be solved.

Types
-----
Types are data or operator without any actual data or implementation,
i.e. the meta-data part of Sisdel data or operator. This is useful for
not having to type all meta-data for all instances of data or operators
that have similar or identical meta-data descriptions. It also makes
it possible to talk about all the meta-data part of a data or operator.

A type can inherit other types to create a new type, even multiple
inheritance is allowed. But the implementation is required by Sisdel
to handle all possible types, and will generate a compile time error
if any of the types would be illegal to use.

Note that once data or operator has been declared, other namespace may
apply further restrictions on it when used from that namespace. Other
namespace may not lift any existing restriction.

Haskell type "maybe". Can be "Nothing" or "Just <x>", meaning "no
value" and "value <x>" respectively. Forces the programmer to always
consider both alternatives whenever there are alternatives. Is this
default type/sub-type?



This is the tree of types being predefined by Sisdel:

type
 |-- constraint
 |    |-- unit
 |    |-- licensed-as
 |    |-- when
 |    |-- requiring-token
 |    |-- authenticated-by
 |    |-- private
 |    |-- maybe
 |    |-- mutable
 |         |-- read-only
 |         |-- write-only
 |
 |-- stored-as
 |    |-- size (can be specified as multiples of other types, or in bytes)
 |    |-- bit
 |    |-- UTF32
 |    |-- UTF16
 |    |-- UTF8
 |    |-- big-endian
 |    |-- little-endian
 |    |-- address
 |    |-- elf-section
 |    |-- elf-segment
 |    |-- alignment
 |
 |-- operator
 |
 |-- nothing
 |
 |-- value
 |    |-- number
 |    |    |-- float
 |    |    |-- unsigned
 |    |    |-- signed
 |    |
 |    |-- boolean
 |    |
 |    |-- String
 |         |-- Documentation
 |
 |-- set
      |-- map
      |    |-- dependency
      |    |-- namespace
      |         |-- reference
      |
      |-- ordered-set
           |-- stream

The inheritance tree only show inheritance, and not namespace
relationship. This means that you do not specify a type as being
"stored-as nr-bits 4", but rather just "nr-bits 4". The "stored-as" is
used to denote a type that can be any of its sub-types. This can be
useful in determining whether a type has specified how it is stored.

Type             Description
---------------- --------------------------------------------------------------
*Thing*          The top-most type. It serves the purpose of saying "any
                 type".
*Constraint*     Puts restriction on a thing, limiting what operations
                 that are allowed. Constraints can only be added, never
                 be removed.
*Unit*           A tag placed on a type to describe compatibility. The unit is
                 unique per type, i.e. same unit name used on incompatible types
                 means different things. Unit does not affect what code is
                 compatible with the type, only what other type that can be
                 calculated together with this type.
*Security token* A token describing access priviledges, i.e. what
                 operations that are allowed on that thing.
*License*        Describes what code can be mixed with what other code. It
                 is also used for calculating the resulting license for the
                 application. Since some license combinations are disallowed,
                 license works like a constraint as seen from Sisdel's view.
*Operator*       Code which optionally accepts an argument, and when
                 executed produces a thing.
*Value*          Data that isn't natively interpreted as part of Sisdel
                 itself, but rather data that is part of the program itself.
*Number*         Any number including integers and floats.
*Mutable-number* Like number, but allowed to change value when the
                 application executes.
*String*         Unicode UTF-8 string.
*Mutable-string* Like string, but allowed to change value when the
                 application executes.
*Documentation*  String attached to code to describe the code.
*Set*            An unordered collection of things. A set can be iterated, but
                 there is no concept of "next" and "previous".
*Map*            A set of key value pairs, where key is used to find a certain
                 value.
*Dependency*     Describes from other things a certain thing was derived.
*Namespace*      A set of symbol name to thing associations. This can
                 give things names. If a thing is not referenced by any
                 namespaces, then it is said to be anonymous.
*Reference*      Similar to namespace, but is a single symbol name to
                 thing association.
*Stream*         An ordered collection which only has a "next" entry, no
                 "previous". Especially useful for describing cases where a read
                 alters a state, e.g. reading the next random number from a
                 random number device.
*Ordered-set*    An ordered collection of things, having both "next" and a
                 "previous" entry. Note that this includes linked list.

Table: Type pre-defined by sisdel-v1

Namespace
---------
Namespace in Sisdel works similar to other programming
languages. Symbol names are searched for in the current namespace
first, and then recursively in parent namespaces.

When Sisdel starts to execute it has a default namespace containing
the symbol "use". For the rest of the core language, there is a
namespace called "sisdel-v<version>". The version number is the
language version that the code was written for.

Any token given on the source line declares a new
namespace. Indentation declares namespace as well, where the last 
token given before the first indented line declares the namespace for
all indented lines.

Operators have two namespaces: One for the arguments and one for the
implementation. The implementation can reach the argument namespace by
using the name "arg".

Files and directories defines modules in Sisdel, and modules defines a
namespace. Modules can take arguments, just like operators. These
arguments are given on the "use" line. Unlike operators however,
modules can publish symbols.

Any sub-module being used inherits the parent module, which requires
the parent module to export applicable namespaces. This means that it
is possible for a parent module to change the language symbols for any
sub-module. Sisdel does however limit what can be modified, e.g. no
constraint may be removed and no type may be modified so the
sub-module code will begin to fail.

In the default namespace the following namespaces are defined:
*env* - This is the environment in which an application runs. It
	typically contains the environment variables stored by the
	operating system.
*arg* - This is the program arguments for the first code executed by
    Sisdel. For sub-modules this is the arguments given by the import
    statement.
*thread* - Namespace given for this thread of execution. Note that
	this is not thread as seen by an operating system. An execution
	thread can extend with messages being passed around. Operator
	invocations translates to asynchronous messages in case this helps
	parallellism, which means that execution threads can be branched
	as well.

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

Contraints
----------

Any line of code in Sisdel can have constraints.

Examples of constraints:

* Numbers
  - integer
  - unsigned
  - range, arguments: from <value>, to <value>, every <value>
  - odd | even
  - size <value>
  - precision <value>
  - accuracy <value>
* string
  - upper-case
  - lower-case
  - normalized
  - range [from <character>] [to <character>]
  - size <value>
  - length <value>
* Set
  - size <value>
  - size [from <value>] [to <value>]
* Ordered-set
  - size <value>
  - size [from <value>] [to <value>]
  - sort-ascending
  - sort-descending
* security-token
  - once-only

Default
-------

*Question*: Support defaults?

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
Sisdel is intended to be as simple to parse as possible, at the cost
of readability. The purpose of the source is to describe how the
program is designed, not to give programmer artistic freedom.

The editor should however be able to present the code in a readable
manner, since it is so easy to parse it should also be easy for the
editor to understand how to view the code.

Regarding comments: These are seen as part of application
documentation, and are therefore defined using the object "#", hash
character. In cases where the documentation is not supposed to be
exported, the tripple hash can be used, i.e. "###".

### Scanner syntax in extended Backus-Naur format:
    (* All names below ending with “-token” are entities returned by
       the scanner *)
    program = { { '\n' } , token } ;
	token =
        number-token
	  | string-token
	  | argname-token
	  | indent-token
	  | documentation-token
	  | comment-token
	  | char-token
	  | unit-token ;
    indent-token = '\t' , { '\t' } ;
    newln = '\n' | '\r' ;
    comment-token = '#' , { ? any-char ? } , newln ;
    documentation-token = '###' , { ? any-char ? } , newln ;
    number-token = [ '-' ] , ( dec-real )
	    | ( '0x' , hex-real )
	    | ( '0o' , oct-real )
	    | ( '0b' , bin-real ) ;
    bin-real = bin-unsigned , [ '.' , bin-unsigned ] , [ 'e' , [ '-' ]
        , bin-unsigned ] ;
    oct-real = oct-unsigned , [ '.' , oct-unsigned ] , [ 'e' , [ '-' ]
        , oct-unsigned ] ;
	dec-real = dec-unsigned , [ '.' , dec-unsigned ] , [ 'e' , [ '-' ]
        , dec-unsigned ] ;
    hex-real = hex-unsigned , [ '.' , hex-unsigned ] , [ 'e' , [ '-' ]
        , hex-unsigned ] ;
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
	    - 0 1 2 3 4 5 6 7 8 9 : \t \n ? ;
    valid-consecutive-char =
	    valid-first-char | ? - 0 1 2 3 4 5 6 7 8 9 ? ;

### Parser syntax:
    (* All names below ending with “-token” comes from the scanner *)
    program = { expression }
	expression = [ expression ? scope ? ] ,
            number-token
		| string-token
		| comment-token
		| documentation-token
		| operator-call
		| unit-token
		| '(' , { expression } , ')'
		| expression , indented-expression
		| set ;
    indented-expression =
	    indent , expression , { same-indent , expression } ;
    operator-call = operator-name , [ expression ] ;
    operator-name = string-token ;
    set = expression , ',' , expression , { ',' , expression } ;

### Example, hello world:
    use sisdel-v1
	stdout println "Hello world!"

### Example, calculate sum of arguments:
    use sisdel-v1
	stdout println "Sum: " , sum arg
	operator sum is
	    arg foreach val do ( retval add val )

### Example, same as above but make sure sum only accepts unsigned integers
    use sisdel-v1
	stdout println "Sum: " , sum arg
	operator sum is
	    arg is unsigned
	    arg foreach val do ( retval add val )

### Example, multiple parameters to operator:
    use sisdel-v1
	stdout println "Result: " , repeat-string "hej " nr-times: 2
	operator repeat-string is
	    repeat nr-times do ( retval add arg )

### Example, make sum single-threaded
    use sisdel-v1
        stdout println "Result: " , sum 1 , 2 , 3 , 4, 5
        operator sum is
	    arg foreach val do ( retval add val )
        message sum is queue-len-max 1

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
value. Example:

    myobj is mutable , integer
    myobj assign 1
    stdout print myobj snapshot
    myobj assign 2
    stdout print myobj snapshot

This will print "12" on the screen. Normally the order of the lines
are not important, but when "snapshot" and "assign" are used, the
order becomes important. Sisdel will adapt to this automatically.

Due to type inference, the first line of code above can actually be
omitted. The "assign" object expects a mutable object, which means
that this constraint will be inferred to "myobj".

Security model, v2
==================

The security model is based on licenses, which issue permissions to
users, where users can be applications as well as companies and
physical users.

A license consists of the following:

* Licensee User
* Issuing User
* Permission
* Limitations
* Permission inheritence
* Monitoring requirements
* Date issued
* Signature
* Description

License
-------

License is what connects everything together.

User, Licensee and Issuing
--------------------------

There are two user's for a license: The user that is allowed the
permission, the Licensee User, and the user that gave the permission,
the Issuing User.

User can be:

* Application
* Physical user
* Juridical person, e.g. company
* User group

Each user has associated public and private keys to authenticate
themselves.

Permission
----------

Name of permission given as an URI. What this permission means is
determined by implementing code and how and when it checks the
permission.

Each permission also has a description understandable to humans
describing what this permission is used for.

Limitations
-----------

Limitations can be:

* Time limit from first use
* Calendar time range
* Number of uses

Permission inheritence
----------------------

This describes how the licensee user can grant this permission to
other users:

* Can grant other user's this permission, recursively
* Can grant other user's this permission, but not recursively
* Cannot grant this permission to other users

Monitoring requirements
-----------------------

In case the issuing user wants to know how the permission is used. Can
be any of:

* Each use of the permission
* When permission is first used
* Each time licensee user grants this permission to a new user
* First time licensee user grants this permission to a new user
* Each time licensee user grants this permission to a new user, where
  the new user is allowed to grant further users.
* First time licensee user grants this permission to a new user, where
  the new user is allowed to grant further users.

There is also a flag indicating whether each use requires
acknowledgement. 

Signature
---------

Signature of the license done by issuing user.

Description
-----------

Describes the license in a way understandable to humans.

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
is why trust model is introduced. The idea is a rapid prototyping
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
objects.
dependency – Describes how one value depends on another.
root – This is the language scope, where all primitives resides.
arg – When a function is called, Sisdel will store all parameters
into a container called param. All arguments to the function can be
found there.
thread – Scope specific to a thread of execution. Note
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

- Strings in code only depicts default language. Translation files for
  other languages can be written afterwards without any special
  attention when first writing the program.

- ":" used as namespace operator.

- " " used as concatenation operator.

- "," used as next field operator.

- <tab> used as indentation operator.

- Identifier names can be used as strings, ensuring that names of
  variables printed are actually valid identifiers.

- Single int the same as an array of one int whose index 0 contains
  this int value. I.e., all single values are also arrays of size 1,
  and can therefore be used where arrays are expected.

- Arrays and maps are compatible types, as long as the key values in
  the map are compatible with array indexes. Arrays are actually maps
  with keys consisting of unsigned integer where no duplicates are
  allowed.

- Time is handled by Sisdel as sequences. Things might occur
  simultaneously and therefore have the same place in a
  sequence. Sisdel will make sure that whatever is said to occur
  simultaneously does not interact in such a way that race conditions
  occur.

- Even when a specific sequence has been specified, later code might
  inject steps by saying "before this but after that".

- Sequences can both apply to in which order data occurs as well as in
  which order execution occurs, or in which order events should occur,
  or states.

- Be able to express things as states. How generic can it be made?
  Applies to types, execution, data? How are events defined?

### Concurrency model

- Each input source, output destination and decision loop are given
  their own thread of execution.

- Threads of executions might be merged for performance reasons based
  on rules set in the source code. For example, threads having similar
  turn-around time for responding to events might be merged. Threads
  sharing a lot of data might also be merged.

- To support above, it must be possible to talk about threads having
  shorter or longer turn-around times, and how much so. This should be
  expressed in percentage of each other, and not in absolute elapsed
  time.

- Data sharing should also be possible to be expressed. It can be both
  quantitive as in number of bytes shared, and qualitive as in how
  often this happens, or any combinations thereof.
