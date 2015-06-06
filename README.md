-*- mode: text; fill-margin: 80 -*-

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

COPYING	      License, currently GPL3. The license model is not set in
	      stone, but will be some variant of GPL.
sisdel_design.odt A more detailed description of the
	      language. Currently very volatile and might be
	      completely rewritten anytime.
REDME.md      This file, contains a short introduction.
Makefile      Makefile to build everything. Invoke it from a separate
	      directory so that build files will not be intermixed
	      with source code. Use "-f <path>/Makefile" to point the
	      file out.
src/	      Directory where the source code is.
unit-test/    Directory where the unit test implementation is.

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
