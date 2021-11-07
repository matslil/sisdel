============================================================
Sisdel
============================================================

Why Sisdel?
===========

Problem statement
-----------------

Many programming languages today have shortcomings in one or more of the following areas:

- Language version handling, be able to have some code dependent on earlier versions of the language, other code on later versions.
- Implementation should not care whether the algorithm is intended to be wrapped into a shell command, GUI application, application plugin, REST API or something else.
- Implementation should not care how many threads that will be spawned, only about any restrictions it may have on parallellism.
- Distinct separation of data and algorithm, to make code re-use easier.
- Being able to follow execution thread across e.g. network protocols.
- Full dependency tracking, so you can e.g. tell what configuration that affects a certain error condition.
- Distinguish between bounded and unbounded data in types.

Programming language paradigms
------------------------------

Structured programming paradigm
  Discipline enforced on direct transfer of execution. This removes possibility to directly specify what instruction to execute next. This instead uses conditional statements, loop statements, functions etc to do language assisted indirect transfer of execution.

Object-oriented programming paradigm
  Discipline enforced on indirect transfer of execution. This removes possibility to call functions using pointers, and instead using virtual object methods and function overloads to do language assisted indirect transfer of execution.

Functional programming paradigm
  Displine enforced on data initialization and assignment. No assignments are allowed, only initializations. Each initialization creates a new instance of the data, which removes issues with mutable data.

Best practice
-------------

RAII - Resource Allocation Is Initialization
  Objects cannot be in uninitialized state if allocation and initialization is always done together. This way you cannot end up with an object having some memory allocated to it, but which is not fully initialized yet.

  This also works well with the functional programming paradigm where you never assign a new value to data already fully initialized, i.e. you do not have mutable data.

SOLID
  Single responsibility - Each data construct, interface or algorithm solves one specific problem, and does not try to be a universal tool to solve any type of problem you can think of.

  Open for extensions, closed for changes - Interfaces can be extended with new functionality as long as the extension is backwards compatible, but it cannot be changed. This ensures that clients to the original version of the interface will still experience correct behavior when interfacing an implementation for a new version of the interface.

  Liskov substitution - When an entity declares it is compatible with a base class, operating on it using base class interface should not cause unexpected behaviors. This means that the entity can be accessed by either its own interface or the base class interface. The only difference should be in level of functionaly being offered.

  Interface segregation - Each interface should be self contained, and not dependent on external interfaces. It can however use objects or handles retrieved using other interfaces.

  Depend on abstraction - Distinguish between interface and code/data, to make it easy to for alternative implementations.

Language goals
--------------

- Each language element has an associated version when it was introduced or when it was last changed in a non-backwards compatible way
- Compiler will keep track of which version of the language that were used for its result, so it can be correctly integrated into code using newer versions of the language
- Be able to add meta-data into existing libraries written in other programming languages so that they can be correctly integrated into Sisdel code

Object
======

An object is the most fundamental entity the a Sisdel developer can use. It is built up from the following parts:

- Stored value
- Interface, type, unit, constraints, license and relations to other objects
- Description, documentation, copyright holder

Stored value
  This is normally handled by the language itself, and is not seen by the developer. Awareness of it can be important though since it consumes resources, typically memory or network bandwidth. In some cases hardware considerations are needed which can affect the stored value.

Interface
  This is how the code interacts with the stored value, it describes the conditions for accessing the stored value.

Description
  All information about the object which does not affect its usage.

Stored value
------------

The only time stored value is being referenced from code is when there are hardware considerations. The following is a list of examples:

Alignment
  The alignment when storing value in memory. Can be specified in bits or bytes, with some special values: Packed for no alignment, cache-line for cache line alignment, natural for what is the machines minimal data size it can access in one data transfer, which is typically the size of the data bus.

Memory region
  If the stored value refers to a specific memory address region.

Interface - Type
----------------

The following types are seen as fundamental, i.e. all other types build on them:

String
  Unicode characters representing text.

Number
  Denotes a mathematical number, which code be a real number, fractional number or integer.

Identifier
  Name known only within the program, unless made explicitly visible by the code.

Set
  Groups together a number of entities. These entities can be of the same type or of different types.

Stream
  This indicates a type which produces a new instance each time it is read, i.e. a stream of entities. This is an infinite set, and tells the compiler/interpreter that pre-calculation is not possible. Note that the only way to convert a stream into a set, is to read a defined number of entries from the stream and return them as a set. Also note that ordering is not possible with stream, you can only read one entry at a time in the order returned by the function producing the stream, although you can skip a number of entries.

Function
  Function operates in a scope accepting a single argument (right-hand side) to produce a result which is its return value.

Type
  Type of entity rather than its value.

These types are defined by the language, but builds from the fundamental types:

Reference
  Operator which when given one entity, returns another. Typically used to make an identifier to refer to some entity.

List
  Set of ordered entities. The order is loose, meaning some entities might have the same ordering.

Sequence
  Set of strictly ordered entities. The order is strict, meaning that no entity has the same order as any other entity.

Map
  Set of references.

Constraint
  Expression which when evaluated within a context determines whether that context is applicable or not. If the expression returns false, the context is not applicable. The consequence of a context not being applicable can result in context being ignored, or compile or runtime error.

Pattern
  Given a pattern and one or more types, matches any number of them. Example patterns are "any" and "none", which refers to any type or no type at all respectively.

Unitcast
  Function which translates the unit given by the scope to the unit given by the argument.

Typecast
  Function which translates the type given by the scope to the type given by the argument.

Interface - Unit
----------------

An artifical addition to the type whose sole purpose is to make the type unique even for value that could have been compatible otherwise. Units can also be combined to produce new units, where functions are used to determine what is possible and what the outcome is.

Unitcast functions are used when unit conversions are needed, e.g. to convert watt * seconds to joule.

Can also define prefixes, e.g. SI-prefix like "kilo" for 1000, "mega" for 1000000 etc.

Interface - Constraints
-----------------------

The starting point for determining the type for the object is the entity type for its stored value. Constraints determines when a type is applicable. It consists of an array of functions returning a boolean. If this boolean is true in a given context, the type is applicable, otherwise it is not. Whether a false results in another type being chosen instead, or if a compiler or runtime error is generated, depends on the context.

Interface - License
-------------------

String holding an SPDX tag naming what license the object is under. Note that this can change when code is compiled, since some licenses can affect other objects that it is compiled together with.

One code using different licenses are including in the same context, the licenses are combined into a new license. In some cases, licenses are not compatible, and if so, compiler/interpreter will fail.

The license determines the following:

- Right to access source code
- Requirement for providing source code to whoemver have received the binary
- Requirement for attributing the copyright holder(s)
- Whether using the code means you are not allowed to restrict patents you (copyright holder) owns and that is used by the code, for other users of the code
- Whether license is weakly copyleft, i.e. its license affects all code included in the same link scope
- Whether license is stronly copyleft, i.e. its license affects all code sharing same API

More can be added as needed to support all common licenses.

There are conversion functions which determines what happens when you combine licenses within same link scope and/or under same API.

Interface - Relations
---------------------

Describes relations to other objects, apart from containment which is implicitly given by set entity. This could be which objects that affected the value of this object, or if this object should be sorted before or after another object in a set.

It can also indicate an object which is changed when current object is changed. This is could be used to describe the case when reading an object representing the interrupt status register might affect an object indicating whether there is a pending interrupt.

Description - Documentation
---------------------------

Similar to how Python documents code, where the documentation becomes part of the object. This makes it possible to send objects over network and have documentation with it.

Description - Copyright holder
------------------------------

Set of strings with an URI to the copyright holder(s). For companies this could be a web address, for private person a mailto address.

It is the copyright holder(s) that decides the license. All copyright holders might give their acceptance if license need to change.

Syntax
======

Comments start with **#** (hash) character and ends at new-line, and are ignored by the compiler/interpreter.

All tokens are separated by at least one white space: Space, tab, new-line and/or carriage-return characters.

A token which contains one of the following characters denote a starting block: **(**, **{**, **[**. The block ends when the same character sequence in reverse order is seen, with **(** replace with **)**, **{** replaced with **}** and **[** replace with **]**. Examples:

{ block }

( block )

(" block ")

{{(abc block cba)}}

There is a special case when having string immediates in the code:

If starting the string with **("** then all white spaces will be trimmed from beginning and end of string. If string starts with **('** then only the first and last white space character will be trimmed from the beginning and end of string. Examples:

("   Hello   ")
  Only the word **Hello** will be included in the string.

('   Hello   ')
  The word **Hello** with two spaces before and after the word.

When a line is indented, then the previous line is used as scope for all lines with the same indentation. Examples:

Line of code giving the context
  First line of code using this context
  Second line of code using this context, creating a new context for the following line
    Line of code using the context defined by previous line

When indentation cannot be used to determine context, then file hierarchy will be used. For the top file in the file hierarchy, the language defined start context will be used.

Everything is built up using functions, some functions are defined by the language, some are in the standard library for the language, some are exported by parent source files in the same project and some are imported externally.

Scanner
-------

Scanner is the preparation step before the parser. The scanner will perform the following steps:

- Remove comments (starts with hash, ends with new-line)
- Mark tokens that work as block start, i.e. tokens containing one or more of (, { and/or [ and match it with a corresponding block end marker token
- Generate one indent token if current line is more indented than previous line
- Generate one unindent token for each less indentation current line has compared to previous line
- Generate new-line character for a sequence of at least one newline or carriage return characters in any combination
- Generate a number token for a sequence of characters matching numbers
- Generate a token for a sequence of non-white-space characters not handled above

Parser
------

.. code-block:: ebnf
   (* Numbers. Binary, octal and hexadecimal numbers has ' separators for every four digits,
      while decimal numbers has it for every three digits. *)

   bin = "0" | "1" ;
   bins = bin, [ bin ], [ bin ], [ bin ]
          | bin, bin, bin, bin, "'", { bin, bin, bin, bin, "'" }, bin, [ bin ], [ bin ] ;
   oct = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" ;
   octs = oct, [ oct ], [ oct ], [ oct ]
          | oct, oct, oct, oct, "'", { oct, oct, oct, oct, "'" }, oct, [ oct ], [ oct ] ;
   dec = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
   decs = dec, [ dec ], [ dec ],
          | dec, dec, dec, "'", { dec, dec, dec, "'" }, dec, [ dec ] ;
   hex = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "a" | "b" | "c" | "d" | "e" | "f" ;
   hexs = hex, [ hex ], [ hex ], [ hex ]
          | hex, hex, hex, hex, "'", { hex, hex, hex, hex, "'" }, hex, [ hex ], [ hex ] ;

   (* Binary, octal and hexadecimal can only be unsigned, while decimal can be signed too *)

   integer = ( [ "+" | "-" ], decs )
         | ( "bin:", bins, )
         | ( "oct:", octs, )
         | ( "hex:", hexs, ) ;

   (* Float and fractions can only used decimals *)

   float = [ "+" | "-" ] ( [ decs ], ".", decs )
           | ( decs, [ ".", [ decs ] ] ) ;
   fraction = [ "+" | "-" ], [ decs ], " ", decs, "/", decs ;

   number = integer | float | fraction ;

   (* Special characters *)

   nl = ? new-line character ? | ? carriage return character ? ;
   nls = nl, { nl } ;
   space = ? space character ? ;
   spaces = space, { space } ;
   sep = spaces | ( { space }, nl, { nl | space }, same-indent ) ;
   tab = ? tab character ? ;
   indent = ? one more tab character than what previous line starts with ? ;
   unindent = ? one less tab character than what previous line starts with ? ;
   same-indent = ? same number of tab characters as what previous line starts with ? ;
   double-quote = ? " character ? ;
   white-space = nl | space | tab ;
   identifier-char = ? any UTF-8 character except white-space, [, {, (, ], }, or ) ? ;
   block-start-char = identifier-char | "[" | "{" | "(" ;
   block-start = { block-start-char }, "[" | "{" | "(", { block-start-char } ;
   block-end-char = identifier-char | "]" | "{" | "(" ;
   block-end = { block-end-char }, "]" | "}" | ")", { block-end-char } (* matches block-start with reversed character sequence and with ( translated to ), { translated to } and [ translated to ] *) ;

   (* string *)

   string-char-double-quote = ? any UTF-8 character except ") sequence ? ;
   string-char-single-quote = ? any UTF-8 character except ') sequence ? ;
   string = ( "(", double-quote, space | nl, { white-space }, { string-char-double-quote }, {white-space}, double-quote, ")" )
          | ( "('", space | nl, { string-char-single-quote }, space | nl, "')" ;

   (* expression *)

   identifier = identifier-char, { identifier-char } ;
   function = identifier, expression ;
   list = expression, ",", expression, { ",", expression } ;
   block = block-start, expression, block-end ;
   expression = ( sep, number
                   | string
                   | identifier
                   | function
                   | list
                   | block
                   , sep )
                | ( expression, indent, expression, unindent ) ;

   program = expression
