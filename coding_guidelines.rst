When adding new code, the following changes are typically made:

- Source file changes
- Build file changes
- Example code unit test
- Other unit tests
- GitHub action changes

For source file changes, see following chapters:
- Coding best practices
- How to test locally
- Code comments

For build file changes, see following chapters:
- CMake best practices
- Conan best practices
- How to lint build files

Unit tests have the same requirements as source file changes. The difference between example code unit tests and other unit tests is that the example code unit tests becomes part of the API documentation, while other unit tests are only for verifying the functionality.

Code comments
=============

Use comments sparingly. For new files, there will be a copyright line telling who wrote the file from the beginning and therefore has copyright to the file, or who rewrote large portions of the file and therefore can claim copyright on it. This copyright is first.

Second is a Doxygen @file comment describing the context for the file. This comment has the following syntax:

    //! @file
    //! Implements the token parser.

Note that no file name is given after @file, to avoid having to update this when changing the name of the file. The comments are short and to the point, describing what is not apparent from the file name itself or its position in the source tree.

Apart from this, and Doxygen comments given below, comments are used only when needed. No comment blocks or comments about what the code does! Only comments that clarifies things that are not obvious from code, e.g. URL where the implemented algorithm is described, explanation why some code guideline is not followed, etc.

Use descriptive names for identifiers, but keep in mind what scope the identifier is in so this is not repeated in the identifier name. Use new types to describe what kind of value is being stored.

Doxygen comments
----------------

Doxygen comments use

    //! Doxygen syntax when used before the documented code

and if on the same line as the code:

    int index; //!< Doxygen comment on same line as code

Doxygen comments are only used for public interfaces, and only given when additional information is needed that is not already known from identifier names or types. Typically, things like when a class or method is typically needed or other scope information are documented. What the class or method does should be clear from their names!

Example usage is documented using unit tests, and is therefore not part of usual API documentation. This ensures that the examples are correct.

About todo and bugfix comments
------------------------------

Todo and bugfix comments are stored in the issue system, not in the code.

Commenting code
---------------

Code being commented out should not go into master branch. The software configuration manager (GIT) will remember how the code looked like. If you want to easily find in, make the removal of the code its own commit with proper comments.

Commit message
==============

Commit messages are used for the following purposes:

1. Show fellow coders what changes have been done and why
2. Build changelog for users
3. Determine when to change major, minor and fix version numbers

Commit message needs to be understandable to both other developers as well as users. The title is important when listing commits and try to find a certain change or certain type of changes. Description is important when trying to understand _why_ the change was made.

Keep commits small but relevant. The best way is imagine that sometime later, someone needs to revert your commit. Will this revert one change you did, or will it cause several unrelated changes? E.g., will it revert one single added feature, bug fix or style change?

This repository starts from the base version of conventional commits:
https://www.conventionalcommits.org/en/v1.0.0/#specification

The following changes from above specification are done:

1. _Both_ exclamation mark on subject line _as_well_as_ **BREAKING-CHANGE** trailer must be specified for breaking changes
2. **BREAKING-CHANGE** has an added hyphen to make it compatible with git trailers (https://git-scm.com/docs/git-interpret-trailers)

Commit message syntax
---------------------

The syntax for a commit message is:

    type[(scope)][!]: title

    [description]

    [trailers]

[] denotes optional parts.

Commit type
-----------

type is one of:

- **feat** if some new feature has been added
- **fix** if some bugfix has been made
- **refactor** code improvement that is neither a bug fix or added feature
- **docs** for documentation updates
- **test** for added, removed or changed test cases, unless the test cases are part of the documentation
- **style** for code style fixes that does not affect the meaning of the code, e.g. indentation changes
- **build** when externally visible build files are changed, typical scope cmake or conan
- **ci** if changes are to github actions

If several types matches your commit, this is an indication that you are doing too much in a single commit. In some cases this is relevant, e.g. when adding a new feature involves updating documentation, changing build files and introducing new ci steps. The main task here is to introduce a new feature, so the type would become "feat".

For **style** or **refactor** types these _must_ be separate commits, to make reviews possible!

Commit scope
------------

**scope** type is currently not well defined since it is too early in the project. Skip this if you are not sure what to specify.

If the change breaks backwards compatibility, an exclamation mark "!" must be added after scope.

Commit title
------------

- Use imperative mood, like you commanding something, since this is how auto-generated git commit messages are phrased
- Make it short, below 72 characters including type and scope since many tools will cut at this length when listing commits
- No references, those belong to description and/or trailers parts, to keep subjects as short and understandable as possible
- Start with captital letter, but the rest is lower case, to make it more readable
- No ending period character, this is a title

Commit description
------------------

- Focus on why the change is made, the diff is the most accurate how description you can get
- Keep lines below 100 characters to avoid having to scroll side-ways

If this commit fixes an issue in GitHub, add one or more lines formatted like this:

- **Fixes #1234** if this fixed issue 1234.

Markdown syntax is allowed in the description, but keep the following in mind when using it:

- Markdown is not a well-specified language
- Not all tools has markdown support

So use markdown sparingly, and in a way that still makes it readable as pure text.

Commit trailers
----------------

trailers is zero or more of:

- **see-also: fe3187489d69c4 Subject of this commit hash** to refer to some other commit
- **BREAKING-CHANGE: description** if the subject line had an exclamation mark denoting breakage of backwards compatibility
- **Co-authored-by: name <email>** to add another author to the commit, this is what GitHub desktop uses when adding co-authors

