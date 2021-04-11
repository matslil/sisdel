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

option( BUILD_LIB "Build ${PROJECT_NAME} library" ON )
option( BUILD_PARSER "Build ${PROJECT_NAME} parser application" ON )
option( BUILD_DOC "Build ${PROJECT_NAME} documentation" ON )

if( BUILD_LIB )
    option( BUILD_TESTS "Build ${PROJECT_NAME} tests" OFF )
else()
    option( BUILD_TESTS "Build ${PROJECT_NAME} tests" NO )
endif()
    
if( BUILD_LIB OR BUILD_PARSER OR BUILD_TESTS )
    set( COMPILER_LANGUAGES C CXX )
    option( USE_CLANG_TIDY "Run clang-tidy static checked while compiling" ON )
    option( USE_IWYU "Run include-what-you-use checker" ON )
    option( USE_LWYU "Run link what you use checked" ON )
else()
    set( COMPILER_LANGUAGES NONE )
    option( USE_CLANG_TIDY "Run clang-tidy static checked while compiling" OFF )
    option( USE_IWYU "Run include-what-you-use checker" OFF )
    option( USE_LWYU "Run link what you use checked" OFF )
endif()

message( STATUS "BUILD_LIB: ${BUILD_LIB}" )
message( STATUS "BUILD_PARSER: ${BUILD_PARSER}" )
message( STATUS "BUILD_DOC: ${BUILD_DOC}" )
message( STATUS "BUILD_TESTS: ${BUILD_TESTS}" )
message( STATUS "USE_CLANG_TIDY: ${USE_CLANG_TIDY}" )
message( STATUS "USE_IWYU: ${USE_IWYU}" )
message( STATUS "USE_LWYU: ${USE_LWYU}" )

# Name the project
project (sisdel
	VERSION $ENV{GITVERSION_SEMVER}
	DESCRIPTION "Generic programming language"
	HOMEPAGE_URL https://github.com/matslil/sisdel
	LANGUAGES ${COMPILER_LANGUAGES}
)

if( DEFINED ENV{CONAN_USER_HOME} )
    list( APPEND CMAKE_MODULE_PATH $ENV{CONAN_USER_HOME} )
    list( APPEND CMAKE_PREFIX_PATH $ENV{CONAN_USER_HOME} )
endif()

include( GNUInstallDirs )
include( CMakePackageConfigHelpers )

if( USE_CLANG_TIDY )
    if( CMAKE_CXX_COMPILER_VERSION MATCHES "([0-9]*).*" )
        set( COMPILER_MAJOR_VERSION "${CMAKE_MATCH_0}" )
    endif()
    find_program( CLANG_TIDY_EXEC clang-tidy "clang-tidy-${COMPILER_MAJOR_VERSION}" )
    if( CLANG_TIDY_EXEC )
        set( CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXEC} )
    else()
        message( WARNING "clang-tidy not found, disabling static code checking using it" )
    endif()
endif()

if( USE_IWYU )
    find_program( IWYU_EXEC iwyu )
    if( IWYU_EXEC )
        set( CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXEC} )
    else()
        message( WARNING "iwyu not found, disabling staic code checking using it" )
    endif()
endif()

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
if( BUILD_LIB )
  add_subdirectory ( lib )
endif()

# Parser application
if( BUILD_PARSER )
  add_subdirectory( parser )
endif()

# Documentation
if( BUILD_DOC )
  add_subdirectory ( doc )
endif()

# Unit tests
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

