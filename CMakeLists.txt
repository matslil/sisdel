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

# Name the project
project (sisdel
	VERSION 0.0.1
	DESCRIPTION "Generic programming language"
	HOMEPAGE_URL https://github.com/matslil/sisdel
	LANGUAGES CXX)

include( GNUInstallDirs )
include( CMakePackageConfigHelpers )

option( USE_CLANG_TIDY "Run clang-tidy static checked while compiling" ON )
if( USE_CLANG_TIDY )
    if( CMAKE_CXX_COMPILER_VERSION MATCHES "([0-9]*).*" )
        set( COMPILER_MAJOR_VERSION "${CMAKE_MATCH_0}" )
    endif()
    find_program( CLANG_TIDY_EXEC clang "clang-${COMPILER_MAJOR_VERSION}" )
    if( CLANG_TIDY_EXEC )
        set( CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXEC} -checks=-*,readability-*,performance-*,cert-* )
    else()
        message( WARNING "clang-tidy not found, disabling static code checking using it" )
    endif()
endif()

option( USE_IWYU "Run include-what-you-use checker" ON )
if( USE_IWYU )
    find_program( IWYU_EXEC iwyu )
    if( IWYU_EXEC )
        set( CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXEC} )
    else()
        message( WARNING "iwyu not found, disabling staic code checking using it" )
    endif()
endif()

option( USE_LWYU "Run link what you use checked" ON )
if( USE_LWYU )
    set( CMAKE_CXX_LINK_WHAT_YOU_USE TRUE )
endif()

# Create "test" build target, used by unit-test subdirectory
enable_testing()
set(CTEST_OUTPUT_ON_FAILURE ON)

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE ${CMAKE_PROJECT_NAME}_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE ${CMAKE_PROJECT_NAME}_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Library
option( BUILD_LIB "Build ${PROJECT_NAME} library" TRUE )
if( BUILD_LIB )
  add_subdirectory ( lib )
endif()

# Parser application
option( BUILD_PARSER "Build ${PROJECT_NAME} parser application" TRUE )
if( BUILD_PARSER )
  add_subdirectory( parser )
endif()

# Documentation
option( BUILD_DOC "Build ${PROJECT_NAME} documentation" TRUE )
if( BUILD_DOC )
  add_subdirectory ( doc )
endif()

# Unit tests
option( BUILD_TESTS "Build ${PROJECT_NAME} tests" TRUE )
if( BUILD_TESTS )  
  add_subdirectory ( unit-test )
endif()

#
# Packaging
#

if( CMAKE_SYSTEM_NAME STREQUAL Linux )
    set( CPACK_GENERATOR "DEB;RPM;TGZ" )
    set( CPACK_SOURCE_GENERATOR ${CPACK_GENERATOR} )
elseif( CMAKE_SYSTEM_NAME STREQUAL Darwin )
    set( CPACK_GENERATOR "DMG" )
    set( CPACK_SOURCE_GENERATOR ${CPACK_GENERATOR} )
elseif( CMAKE_SYSTEM_NAME MATCHES ".*BSD$" )
    set( CPACK_GENERATOR "PKG" )
    set( CPACK_SOURCE_GENERATOR ${CPACK_GENERATOR} )
elseif( CMAKE_SYSTEM_NAME STREQUAL Windows )
    set( CPACK_GENERATOR "NSIS" )
    set( CPACK_SOURCE_GENERATOR ${CPACK_GENERATOR} )
else()
  message( WARNING "${CMAKE_SYSTEM_NAME}: Don't know what package format to use, disabling packaging" )
  set( CPACK_GENERATOR "" )
  set( CPACK_SOURCE_GENERATOR "" )
endif()

if( CPACK_GENERATOR OR CPACK_SOURCE_GENERATOR )
    # Enable component packaging for package generators that do not have this as
    # default, but supports it.
    set( CPACK_RPM_COMPONENT_INSTALL ON )
    set( CPACK_DEB_COMPONENT_INSTALL ON )
    set( CPACK_TBZ2_COMPONENT_INSTALL ON )
    set( CPACK_TGZ_COMPONENT_INSTALL ON )
    set( CPACK_TXZ_COMPONENT_INSTALL ON )
    set( CPACK_TZ_COMPONENT_INSTALL ON )

    # One package per component
    set( CPACK_COMPONENTS_IGNORE_GROUPS 1 )

    set( CPACK_COMPONENT_lib${PROJECT_NAME}_DESCRIPTION "Libraries and headers for ${PROJECT_NAME}" )

    set( CPACK_PACKAGE_CONTACT "Mats G. Liljegren" )

    include( CPack )
endif()

