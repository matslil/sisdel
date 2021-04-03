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

cmake_minimum_required (VERSION 3.14)

option( DISABLE_WERROR "Do not make warnings fail the build" FALSE )

# Used to find lttng
find_package(PkgConfig)
find_package( Boost REQUIRED )
find_package( gmp REQUIRED )
find_package( mpfr REQUIRED )

pkg_check_modules( LTTNG_UST lttng-ust )

find_program(LCOV lcov)

add_library( sisdel
	config.cc
	sbucket.cc
       	file.cc
       	mmap_file.cc
       	token.cc
       	error.cc
       	position.cc
	)

target_link_libraries( sisdel PUBLIC Boost::Boost gmp::GMP mpfr::mpfr )

target_compile_options( sisdel PUBLIC -Wall -Wextra -Wshadow -Wuninitialized -Winit-self -Wmissing-prototypes -Wformat-security -Wunused-parameter -Wundef -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wstrict-prototypes -Wmissing-declarations -Wredundant-decls -fstack-protector )

if( NOT DISABLE_WERROR )
  target_compile_options( sisdel PUBLIC -Werror )
endif()

target_compile_definitions( sisdel PUBLIC -Dsisdel_VERSION_MAJOR=${sisdel_VERSION_MAJOR} -Dsisdel_VERSION_MINOR=${sisdel_VERSION_MINOR} )

target_compile_features( sisdel PUBLIC cxx_std_20 )

if (LTTNG_UST_FOUND)
  target_compile_definitions( sisdel PRIVATE HAVE_LTTNG )
  message(STATUS "lttng-ust detected, tracing enabled")

  # Handle include directories
  target_include_directories( sisdel PRIVATE ${LTTNG_UST_INCLUDE_DIRS} . )

  # Handle library paths
  target_link_directories( sisdel ${LTTNG_UST_LIBRARY_DIRS} )

  # Handle compiler flags
  target_compile_options( sisdel PRIVATE ${LTTNG_UST_CFLAGS} )

  # Handle linker flags
  target_link_options( sisdel ${LTTNG_UST_LDFLAGS} )

  target_sources( sisdel sisdel_tracepoints.c )
else()
  message( STATUS "lttng-ust NOT detected, tracing disabled" )
endif()

if (LCOV)
  # Add profile support
  message (STATUS "Collecting code coverage information")
  target_compile_options( sisdel PUBLIC -g --coverage )
  target_link_options( sisdel PUBLIC -fprofile-arcs )
  set (LCOV_COLLECT "&& ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/lcov_collect.sh")
else ()
  message (WARNING "lcov not found: Not collecting code coverage information")
  set (LCOV_COLLECT "")
endif ()

add_executable( test_scanner test_scanner.cc )
target_link_libraries( test_scanner PRIVATE sisdel )

# Unit tests
add_subdirectory (unit-test)

#
# Doxygen
#

#find_package(Doxygen)
#option(BUILD_API_DOCUMENTATION "Create and install the PDF and HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

#if(BUILD_API_DOCUMENTATION)
#    if(NOT DOXYGEN_FOUND)
#         message(FATAL_ERROR "Doxygen is needed to build the API documentation.")
#    endif()

#    add_custom_target(doc-api
#                      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/sisdel.doxygen
#                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#		      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/sisdel.doxygen
#                      COMMENT "Generating API documentation with Doxygen"
#                      VERBATIM)
#     configure_file ("sisdel.doxygen" "sisdel.doxygen" @ONLY)

#    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)
#endif()
