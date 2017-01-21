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

# Only supports Clang, GCC/G++ will fail
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

set (CMAKE_CXX_STANDARD 14)

# Name the project
project (Sisdel)

# Create "test" build target, used by unit-test subdirectory
enable_testing()
set(CTEST_OUTPUT_ON_FAILURE ON)

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE sisdel_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE sisdel_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set version
set (sisdel_VERSION_MAJOR 1)
set (sisdel_VERSION_MINOR 0)
configure_file (
  "${PROJECT_SOURCE_DIR}/config.hh.in"
  "${PROJECT_BINARY_DIR}/config/config.hh"
  )
include_directories("${PROJECT_BINARY_DIR}/config")

# Source code
add_subdirectory (src)

# Documentation
add_subdirectory (doc)
