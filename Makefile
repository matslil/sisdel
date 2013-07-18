ifeq ($(origin BUILDING),undefined)
# Determine source path from where this Makefile is
export SRCPATH := $(abspath $(dir $(shell readlink -e $(CURDIR)/$(lastword $(MAKEFILE_LIST)))))

MAKECMDGOALS ?= test

.PHONY: all $(MAKECMDGOALS)
test $(MAKECMDGOALS): Makefile
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

#vpath %.tp $(SRCPATH)/src   # Replaced by the %.c vpath below
vpath %.c  $(SRCPATH)/src

# Needed when building unit tests
vpath %.cc $(SRCPATH)/unit-test   # Sisdel unit tests
vpath %.cc /usr/src/gtest/src     # GTest source, must be built with the same
				  # flags as Sisdel!

OPTIMIZE_LEVEL := 0

CFLAGS_COMMON := -I$(SRCPATH)/src
CFLAGS_COMMON += -I$(CURDIR)
CFLAGS_COMMON += -isystem $(LTTNG)/include
CFLAGS_COMMON += -I/usr/src/gtest
CFLAGS_COMMON += -Wall
CFLAGS_COMMON += -Wshadow
#CFLAGS_COMMON += -Werror
CFLAGS_COMMON += -g
CFLAGS_COMMON += -pthread
CFLAGS_COMMON += -O$(OPTIMIZE_LEVEL)

LDFLAGS :=

# If verbosity is 3 or higher, let tools print what programs they use
ifneq ($(filter-out 0 1 2,$(V)),)
override CFLAGS_COMMON += -v
override LDFLAGS += -v
override LDFLAGS += -Wl,-t
endif

override LDFLAGS += -pthread
override LDFLAGS += -Wl,-rpath -Wl,$(LTTNG)/lib

CFLAGS :=
override CFLAGS += $(CFLAGS_COMMON)

CCFLAGS :=
override CCFLAGS += $(CFLAGS_COMMON)
override CCFLAGS += -Wextra

CC := clang
C++ := clang++

CCFILES := sbucket.cc
CCFILES += error.cc
CCFILES += mmap.cc
CCFILES += file.cc
CCFILES += mmap_file.cc
CCFILES += token.cc
CFILES += sisdel_tracepoints.c

UNIT_TESTS := sbucket
UNIT_TESTS += token

SRCFILES_ALL := $(CCFILES) $(CFILES) $(addsuffix .cc,$(addprefix check_,$(UNIT_TESTS)))

LIBS :=

export LD_RUN_PATH += $(LTTNG)/lib

override LIBS += -L$(LTTNG)/lib
override LIBS += -llttng-ust
override LIBS += -lrt
override LIBS += -ldl

.PHONY: all clean lint

# all: Makefile sisdel.o

test: Makefile $(addprefix check_,$(UNIT_TESTS))

# How to create C (and header) source files from a tracepoint template file
#sisdel_tracepoints.c sisdel_tracepoints.h: sisdel_tracepoints.tp
#	lttng-gen-tp $< -o $(@)

# How to compile a C source file.
%.o: %.c
	@echo "   CC  $<"
	$(CC) -std=gnu99 -MMD -MF $(@:.o=.d) -c $(CFLAGS) -o $@ $<

#%.bc: %.cc sisdel_tracepoints.h
#	@echo "   C++ $<"
#	$(C++) -std=c++11 -MMD -MF $(@:.o=.d) -c $(CCFLAGS) -o $@ $<

# Clean all temporary files
clean:
	rm -f $(addsuffix .[odchis],$(basename $(SRCFILES_ALL))) $(addsuffix .ii,$(basename $(SRCFILES_ALL)))

# Create html files describing potential problems with the code
lint: $(CFILES)
	clang --analyze $(CPPFLAGS) $(LINTFLAGS) $^

doxygen.cfg: $(SRCPATH)/doxygen.cfg
	@echo "   GEN $<"
	sed -e 's!###INPUT###!$(SRCPATH)!g' $< > $@

version.sh: $(SRCPATH)/version.sh
	@echo "   GEN $<"
	sed -e 's!###INPUT###!$(SRCPATH)!g' $< > $@

doc: doxygen.cfg Makefile
	@echo "   DOC "
	doxygen $<

.SECONDEXPANSION:

$(addprefix check_,$(UNIT_TESTS)): $(CCFILES) $$@.cc gtest-all.cc $(CFILES:.c=.o)
	@echo "   LD  $@"
	$(C++) -std=c++11 $(CCFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

endif # ifeq ($(origin BUILDING),undefined)
