# Copyright (C) 2017 Mats G. Liljegren
# SPDX-License-Identifier: Apache-2.0

# This file is part of Sisdel.

# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy
# of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

cmake_minimum_required (VERSION 3.1)

set(EXTRA_SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/config.cc")

# Find pkg-config tool to be used when searching for check library
# See http://www.cmake.org/cmake/help/v2.8.2/cmake.html#module:FindPkgConfig
find_package(PkgConfig)

# Macro for finding an optional module (library), and add it to the build.
# Use <var>_FOUND to determine whether this succeeded or not.
macro (check_optional_module var name)
  pkg_check_modules(${var} ${name})

  # If module is found
  if (${var}_FOUND)
    # Handle ${var}_INCLUDE_DIRS
    include_directories(${${var}_INCLUDE_DIRS} .)

    # Handle ${var}_LIBRARY_DIRS
    link_directories(${${var}_LIBRARY_DIRS})

    # Handle ${var}_CFLAGS
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${${var}_CFLAGS})

    # Handle ${var}_LDFLAGS, translate ; separated list into CMake list
    foreach(OPT ${${var}_LDFLAGS})
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OPT}")
    endforeach()

    set(EXTRA_SRCFILES ${EXTRA_SRCFILES} ${CMAKE_CURRENT_SOURCE_DIR}/sisdel_tracepoints.c)

  endif (${var}_FOUND)
endmacro (check_optional_module)

# Check if we have LTTNG tracer
check_optional_module(LTTNG_UST lttng-ust)

if (LTTNG_UST_FOUND)
  add_definitions(-DHAVE_LTTNG)
  message(STATUS "lttng-ust detected, tracing enabled")
else ()
  message(STATUS "lttng-ust NOT detected, tracing disabled")
endif (LTTNG_UST_FOUND)

# We need the Boost headers
find_package(Boost)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(ERROR "Boost include files not found!")
endif()

find_program(LCOV lcov)
if (LCOV)
  # Add profile support
  message (STATUS "Collecting code coverage information")
  add_definitions(-g --coverage)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs")
  set (LCOV_COLLECT "&& ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/lcov_collect.sh")
else ()
  message (WARNING "lcov not found: Not collecting code coverage information")
  set (LCOV_COLLECT "")
endif ()

# Common flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wshadow -Wuninitialized -Winit-self -Wmissing-prototypes -Wformat-security -Wunused-parameter -Wundef -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wstrict-prototypes -Wmissing-declarations -Wredundant-decls -fstack-protector ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Dsisdel_VERSION_MAJOR=${sisdel_VERSION_MAJOR} -Dsisdel_VERSION_MINOR=${sisdel_VERSION_MINOR}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Dsisdel_VERSION_MAJOR=${sisdel_VERSION_MAJOR} -Dsisdel_VERSION_MINOR=${sisdel_VERSION_MINOR}")

# GMP and MPFR
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lmpfr -lgmp")

# Unit tests
add_subdirectory (unit-test)

add_executable (test_scanner test_scanner.cc sbucket.cc file.cc mmap_file.cc token.cc error.cc position.cc ${EXTRA_SRCFILES})

set_target_properties(test_scanner PROPERTIES LINKER_LANGUAGE CXX)

#
# Doxygen
#

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API        
documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
         message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    add_custom_target(doc-api
                      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/sisdel.doxygen
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		      DEPENDS sisdel.doxygen
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
     configure_file ("sisdel.doxygen" "sisdel.doxygen" @ONLY)

#    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)
endif()
