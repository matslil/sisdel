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

cmake_minimum_required ( VERSION 3.13 )

find_package( Catch2 REQUIRED )

add_executable( unittest unittest.cc check_mmap_file.cc )
target_link_libraries( unittest PRIVATE Catch2::Catch2 sisdel )

# Make sure data files are copied to directory where unittest binary is built
add_custom_command(
        TARGET unittest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/check_mmap_file.data
                ${CMAKE_CURRENT_BINARY_DIR}/check_mmap_file.data
)

include( CTest )
include( Catch )
catch_discover_tests( unittest )

