=====================
Types
=====================

A type describes structure, and can be used to:

- Enforce data format
- Describe a template
- Describe a pattern

The more obvious usage is to describe what kind of data that a function accepts, and what kind of data the function returns. But types can also be used to describe valid format of a configuration file. It is even possible to translate a CSS file into a type which then can be used to validate an HTML file.

For the case with configuration file, some places in the file would have some value expected. These values can in the type be described by variables, whose type describes which values are valid for the specific configuration. When this type is then successfully applied to data, those values will be written to the type's variables.

Types of types
--------------

- Type
- Data
- Function
- Variable
- Identifier
- Public
- Collection
  - Set
  Can be ordered or not, sorted or not, supports pop front, supports pop back
- Composition
  - A or B
  - A and B
  - A exclusive or B
- Sequence (next, previous, split)
  - A follows B
  - split to A and B
  Join is done through connection.
- Connection
- Repetition
  - Minimum number of repetitions
  - Maximum number of repetitions
  - Greedy/non-greedy

A map is for example built by set of key to data connections. Both key and data are objects of any type. If key object type is unsigned integer, then the map would become an array. If key can have different types within a set, this would become a struct.
