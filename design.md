Sisdel design description {#design}
=========================

This document is intended as an introduction to Sisdel language design. It is intended as a high-level description, the source code documentation show serve as the low-level description.

## Token syntax

This token scanner adheres to the syntax as described below.
The file being scanned consists of tokens separated by either blank,
tab, or carriage return.

Line feed has a special meaning for the parser, so it is returned as an
identifier token. There is an exception however: if the first token on
the line following the line feed is three dots, then this measns that
the line feed should be ignored.

For example, the following produces a line feed token between foo and bar:

	foo
	bar

But in this example, only foo and bar will be tokens:

	foo
	... bar

This therefore becomes equivalent with:

	foo bar

Tokens can have either of following types:

Token type  | Description
----------  | ---------------
End of file | Token representing end of file.
End of line | Token representing new line character.
Identifier  | This can be line-feed or any word consisting of any valid character.
Integer     | An integer value with optinally a unit name.
Float       | A floating point value with optionally a unit name.
String      | A string with optionally a unit name.

The token might also be an error token. The above will still be filled in,
but a check for error will indicate that the token wasn't parsed correctly.

## Parser syntax

The following is an extended Bachus-Naur description of the parser syntax:

	program ::= value
	value ::= identifier
	        | value identifier value*
	        | string-constant
	        | integer-constant
	        | float-constant
	        | '(' value* ')'

Examples:

	times-two is function arg a do (
		return a * 2
	)
