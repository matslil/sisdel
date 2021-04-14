# SPDX-License-Identifier: MIT

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

