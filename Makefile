# Current dependencies:
# - Google Test framework
# - Boost, multiprecision library (for mpfr_float etc)
# - GNU MPFR C library (used by boost)

ifeq ($(origin BUILDING),undefined)
# Determine source path from where this Makefile is
export SRCPATH := $(abspath $(dir $(shell readlink -e $(CURDIR)/$(lastword $(MAKEFILE_LIST)))))

MAKECMDGOALS ?= test

.PHONY: all $(MAKECMDGOALS)
$(MAKECMDGOALS): Makefile
	@$(MAKE) -rR --warn-undefined-variables --no-print-directory $@ BUILDING=y

# Make sure object directory gets a link to the makefile, so we don't need to
# specify "-f" for sub-sequent make invocations.
Makefile: $(SRCPATH)/Makefile
	ln -fs $<

else

.SILENT:

# Default verbosity is silent
V ?= 0

ifneq ($(filter-out 0 1 2,$(V)),)
$(info SRCPATH = "$(SRCPATH)")
endif

LTTNG := /home/mats/install-lttng-ust


# Make sure we use a bash shell
SHELL := $(shell which bash)

# If verbosity is 2 or higher, print all shell commands executed
ifneq ($(filter-out 0 1,$(V)),)
SHELL += -x
endif

# If verbosity is 1 or higher, print which rules that are executed and why
ifneq ($(filter-out 0,$(V)),)
OLD_SHELL := $(SHELL)
SHELL = $(warning Building $@$(if $<, (from $<))$(if $?, ($? newer)))$(OLD_SHELL) 
endif

vpath %.cc $(SRCPATH)/src
vpath %.md $(SRCPATH)
vpath %.dot $(SRCPATH)
vpath %.css $(SRCPATH)

#vpath %.tp $(SRCPATH)/src   # Replaced by the %.c vpath below
vpath %.c  $(SRCPATH)/src

vpath default.latex $(SRCPATH)/templates

# Needed when building unit tests
vpath %.cc $(SRCPATH)/unit-test   # Sisdel unit tests
vpath %.cc $(SRCPATH)/submodules/googletest/googletest/src # Google test framework

OPTIMIZE_LEVEL := 0

CFLAGS_COMMON := -I$(SRCPATH)/src
CFLAGS_COMMON += -I$(CURDIR)
CFLAGS_COMMON += -Wall
CFLAGS_COMMON += -Wshadow
#CFLAGS_COMMON += -Werror
CFLAGS_COMMON += -g
CFLAGS_COMMON += -pthread
CFLAGS_COMMON += -O$(OPTIMIZE_LEVEL)

# Include directories for Google Test
CFLAGS_COMMON += -isystem $(SRCPATH)/submodules/googletest/googletest/include
CFLAGS_COMMON += -I$(SRCPATH)/submodules/googletest/googletest

LDFLAGS :=

# If verbosity is 3 or higher, let tools print what programs they use
ifneq ($(filter-out 0 1 2,$(V)),)
override CFLAGS_COMMON += -v
override LDFLAGS += -v
override LDFLAGS += -Wl,-t
endif

override LDFLAGS += -pthread
#override LDFLAGS += -Wl,-rpath -Wl,$(LTTNG)/lib

CFLAGS :=
override CFLAGS += $(CFLAGS_COMMON)

CCFLAGS :=
override CCFLAGS += $(CFLAGS_COMMON)
override CCFLAGS += -Wextra
override CCFLAGS += -std=c++14

CC := clang
C++ := clang++

CCFILES := sbucket.cc
CCFILES += file.cc
CCFILES += mmap_file.cc
CCFILES += token.cc
CCFILES += error.cc
CCFILES += position.cc
CFILES += sisdel_tracepoints.c

UNIT_TESTS := check_sbucket check_mmap_file

SRCFILES_ALL := $(CCFILES) $(CFILES) $(addsuffix .cc,$(UNIT_TESTS))

LIBS :=

export LD_RUN_PATH += $(LTTNG)/lib

# LTTng tracer
override LIBS += -L$(LTTNG)/lib
override LIBS += -llttng-ust

override LIBS += -lrt
override LIBS += -ldl

# Multiprecision floats and ints
override LIBS += -lmpfr -lgmp

.PHONY: all clean lint doc html pdf clean-doc distclean-doc

all: Makefile $(CCFILES:.cc=.o)

# Make documents in all supported formats, currently html and pdf
doc: html pdf

# Remove everything related to documentation
distclean-doc: clean-doc
	rm -f sisdel.{html,pdf}

# Remove all intermediary files, saving only the final documentation
clean-doc:
	rm -f type-hierarchy-graph.{svg,pdf} type-relation-graph.{svg,pdf} sisdel.{tex,log,out,aux}

# "make html" is a short-hand for "make sisdel.html"
html: sisdel.html

# "make pdf" is a short-hand for "make sisdel.pdf"
pdf: sisdel.pdf

# sisdel.pdf is built from sisdel.tex, this is debug version of sisdel.pdf
# sisdel.pdf:  %.pdf : %.tex

# sisdel.pdf build, release version
sisdel.pdf: %.pdf: %.md %.css sisdel.md type-hierarchy-graph.pdf type-relation-graph.pdf default.latex CC-BY-license-88x31.png
	pandoc --toc -c $*.css --default-image-extension=$(suffix $@) --data-dir=$(<D) -o $@ $<


# How to build the document file, same template regardless of output format
sisdel.tex: %.tex: %.md %.css sisdel.md type-hierarchy-graph.pdf type-relation-graph.pdf default.latex CC-BY-license-88x31.png
	pandoc --toc -c $*.css --default-image-extension=pdf --data-dir=$(<D) -o $@ $<

sisdel.html: %.html: %.md %.css sisdel.md type-hierarchy-graph.svg type-relation-graph.svg CC-BY-license-88x31.png
	pandoc --toc -c $*.css --default-image-extension=$(suffix $@) --data-dir=$(<D) -o $@ $<

# Convert LaTex to pdf
%.pdf: %.tex
	pdflatex $<

# LaTex does not support svg format, so svg images are converted to pdf
# LaTex is used as an intermediate format by pandoc when producing pdf
# documents
%.pdf: %.svg
	rsvg-convert -f pdf -o $@ $<

# Compile directional graph text file to svg
%.svg: %.dot
	dot -Tsvg $< > $@

CC-BY-license-88x31.png: $(SRCPATH)/CC-BY-license-88x31.png
	ln -s $<

# Run unit test
test: Makefile $(UNIT_TESTS)

# How to create C (and header) source files from a tracepoint template file
#sisdel_tracepoints.c sisdel_tracepoints.h: sisdel_tracepoints.tp
#	lttng-gen-tp $< -o $(@)

# How to compile a C source file.
%.o: %.c
	@echo "   CC  $<"
	$(CC) -std=gnu99 -MMD -MF $(@:.o=.d) -c $(CFLAGS) -o $@ $<

# How to compile a C source file.
%.o: %.cc
	@echo "   CC  $<"
	$(CC) -MMD -MF $(@:.o=.d) -c $(CCFLAGS) -o $@ $<

#%.bc: %.cc sisdel_tracepoints.h
#	@echo "   C++ $<"
#	$(C++) -MMD -MF $(@:.o=.d) -c $(CCFLAGS) -o $@ $<

# Clean all temporary files
clean: clean-doc
	rm -f $(addsuffix .[odchis],$(basename $(SRCFILES_ALL))) $(addsuffix .ii,$(basename $(SRCFILES_ALL))) gtest-all.[od]

# Create html files describing potential problems with the code
lint: $(CCFILES)
	clang --analyze $(CCFLAGS) $(LINTFLAGS) $^

test_scanner: test_scanner.o $(CCFILES:.cc=.o) $(CFILES:.c=.o)
	@echo "   LD  $@"
	$(C++) $(CCFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# Google Test
gtest-all.a: gtest-all.cc

# Sbucket unit test
check_sbucket: check_sbucket.o sbucket.o gtest-all.o $(CFILES:.c=.o)
	@echo "   LD  $@"
	$(C++) $(CCFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

check_mmap_file.data:
	cp $(SRCPATH)/unit-test/$@ .

# Mmap_file unit test
check_mmap_file: check_mmap_file.o mmap_file.o position.o sbucket.o file.o gtest-all.o $(CFILES:.c=.o) check_mmap_file.data
	@echo "   LD  $@"
	$(C++) $(CCFLAGS) $(LDFLAGS) -o $@ $(filter %.o,$^) $(LIBS)

-include *.d

endif # ifeq ($(origin BUILDING),undefined)
