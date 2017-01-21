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

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(googletest)

# Syntax:
#   do_test (<test name> <source file> <source file> ...)

function (do_test test_name)
  add_executable (${test_name} ${ARGN} ${EXTRA_SRCFILES})
  target_link_libraries (${test_name} gtest_main ${LTTNG_UST_LIBRARIES})
  add_test (NAME ${test_name} COMMAND sh -c "(./${test_name}) ${LCOV_COLLECT}")
  set_tests_properties (${test_name} PROPERTIES TIMEOUT "20")
endfunction (do_test)

# sbucket unit test
do_test (check_sbucket check_sbucket.cc ../sbucket.cc)

# mmap unit test
do_test (check_mmap check_mmap_file.cc ../mmap_file.cc ../position.cc ../sbucket.cc ../file.cc)
add_custom_command(
        TARGET check_mmap POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/check_mmap_file.data
                ${CMAKE_CURRENT_BINARY_DIR}/check_mmap_file.data)
