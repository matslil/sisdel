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

find_package( Boost REQUIRED )
find_package( gmp REQUIRED )
find_package( mpfr REQUIRED )

find_program(LCOV lcov)

add_library( ${PROJECT_NAME}
        config.cc
        sbucket.cc
       	file.cc
       	mmap_file.cc
       	token.cc
       	error.cc
       	position.cc
)

target_link_libraries( ${PROJECT_NAME} PUBLIC Boost::Boost gmp::GMP mpfr::mpfr )

target_compile_options( ${PROJECT_NAME} PUBLIC -Wall -Wextra -Wshadow -Wuninitialized -Winit-self -Wmissing-prototypes -Wformat-security -Wunused-parameter -Wundef -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wstrict-prototypes -Wmissing-declarations -Wredundant-decls -fstack-protector )

if( NOT DISABLE_WERROR )
    target_compile_options( ${PROJECT_NAME} PUBLIC -Werror )
endif()

target_compile_definitions( ${PROJECT_NAME} PUBLIC -D${PROJECT_NAME}_VERSION_MAJOR=${PROJECT_VERSION_MAJOR} -D${PROJECT_NAME}_VERSION_MINOR=${PROJECT_VERSION_MINOR} )

target_compile_features( ${PROJECT_NAME} PUBLIC cxx_std_20 )

target_include_directories( ${PROJECT_NAME}
       	PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

file(GLOB_RECURSE SISDEL_PUBLIC_INCLUDE_FILES "include/*")
set_target_properties( ${PROJECT_NAME}
        PROPERTIES
        PUBLIC_HEADER "${SISDEL_PUBLIC_INCLUDE_FILES}"
)

# Not specifying DESTINATION will use GNUInstallDirs or built-in defaults for libraries and headers
install( TARGETS ${PROJECT_NAME}
       	EXPORT lib-export
        PUBLIC_HEADER
        COMPONENT lib
)

install( EXPORT lib-export
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        COMPONENT lib
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake
)

if (LCOV)
  # Add profile support
  message (STATUS "Collecting code coverage information")
  target_compile_options( ${PROJECT_NAME} PUBLIC -g --coverage )
  target_link_options( ${PROJECT_NAME} PUBLIC -fprofile-arcs )
  set (LCOV_COLLECT "&& ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/lcov_collect.sh")
else ()
  message (WARNING "lcov not found: Not collecting code coverage information")
  set (LCOV_COLLECT "")
endif ()

add_executable( test_scanner test_scanner.cc )
target_link_libraries( test_scanner PRIVATE ${PROJECT_NAME} )

