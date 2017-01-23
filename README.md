Welcome to Sisdel - Simple syntax dependency language
=====================================================

This is an experimental language that wants to explore the
consequences of having a language that lets dependencies of all kinds
to be a first class entity. This means that a programmer can make use
of the dependencies that the compiler/interpreter must keep track of
anyway.

The language is meant to be primarily functional like Haskell, but
still have the option of describing states whenever there is a need
for it.

The syntax is meant to be kept dead simple with few exceptions and few
restrictions. This means that the programmer will have a lot of
freedom, probably more than is healthy. The purpose is to find out
what programmers will do with that freedom before deciding what
freedom is good and what isn't.

The directory layout:

File              | Description
------------------|----------------------------------------------------
COPYING	          | License, which is Apache 2.0.
sisdel_design.odt | A more detailed description of the language. Currently very volatile and might be completely rewritten anytime.
REDME.md          | This file, contains a short introduction.
Makefile          | Makefile to build everything. Invoke it from a separate directory so that build files will not be intermixed with source code. Use "-f <path>/Makefile" to point the file out.
src/	          | Directory where the source code is.
src/unit-test/    | Directory where the unit test implementation is.

Ubuntu requirements for building documentation
----------------------------------------------
 * pandoc
 * dotty
 * graphviz
 * texlive-latex-base
 * texlive-latex-recommended
 * texlive-latex-extra
 * texlive-fonts-recommended
 * librsvg2-bin

Getting started
---------------

The below mentioned <build-dir> should be a path outside the source
directory tree.

Make sure the googletest GIT submodule has been cloned before starting

1.     mkdir -p <build-dir>
2.     cd <build-dir>
3.     cmake <source-dir>

Now you can run make. To make the default set of target, just run make:

    make

Right now this builds test applications including unit test binaries.

If you want user documentation, run doc target:

    make doc

To get source code documentation, use doc-api target:

    make doc-api

To run the unit tests, use:

    make test

If you want to do some static analyzing for finding bugs:

    make clean && scan-build make

License
-------

SPDX-License-Identifier: Apache-2.0

Licensed under the Apache License, Version 2.0 (the "License"); you may not
use this file except in compliance with the License. You may obtain a copy
of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations
under the License.
