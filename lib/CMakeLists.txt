# SPDX-License-Identifier: MIT

cmake_minimum_required (VERSION 3.14)

option( DISABLE_WERROR "Do not make warnings fail the build" TRUE )
option( USE_COVERAGE "Build with code coverage support" FALSE )

set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "Build type selections" FORCE)

#
# Dependencies
#

find_package( Boost REQUIRED )
find_package( gmp REQUIRED )
find_package( mpfr REQUIRED )

#
# Library contents
#

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

target_compile_options( ${PROJECT_NAME} PRIVATE -Wall -Wextra -Wshadow -Wuninitialized -Winit-self -Wmissing-prototypes -Wformat-security -Wunused-parameter -Wundef -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wstrict-prototypes -Wmissing-declarations -Wredundant-decls -fstack-protector )

if( NOT DISABLE_WERROR )
  target_compile_options( ${PROJECT_NAME} PRIVATE -Werror )
endif()

target_compile_definitions( ${PROJECT_NAME} PUBLIC -D${PROJECT_NAME}_VERSION_MAJOR=${PROJECT_VERSION_MAJOR} -D${PROJECT_NAME}_VERSION_MINOR=${PROJECT_VERSION_MINOR} )

target_compile_features( ${PROJECT_NAME} PUBLIC cxx_std_20 )

configure_file (
  "${PROJECT_SOURCE_DIR}/config.hh.in"
  "${PROJECT_BINARY_DIR}/config/config.hh"
)

target_include_directories( ${PROJECT_NAME}
       	PUBLIC
        $<INSTALL_INTERFACE:include>
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${PROJECT_BINARY_DIR}/config>"
)

file( GLOB_RECURSE SISDEL_PUBLIC_INCLUDE_FILES "include/*" )
set_target_properties( ${PROJECT_NAME}
        PROPERTIES
	PUBLIC_HEADER "${SISDEL_PUBLIC_INCLUDE_FILES};${PROJECT_BINARY_DIR}/config/config.hh"
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	DEBUG_POSTFIX "-dbg"
)

if( USE_COVERAGE )
  find_program( LCOV lcov REQUIRED )
  message (STATUS "USE_COVERAGE: Collecting code coverage information")
  target_compile_options( ${PROJECT_NAME} PUBLIC -g --coverage )
  target_link_options( ${PROJECT_NAME} PUBLIC -fprofile-arcs )
  set (LCOV_COLLECT "&& ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/lcov_collect.sh")
else ()
	message (STATUS "USE_COVERAGE: Not collecting code coverage information")
  set (LCOV_COLLECT "")
endif ()

#
# Installation
#

# Create .pc file used by pkg-config
configure_file( lib.pc.in ${PROJECT_NAME}.pc @ONLY )

# Not specifying DESTINATION will use GNUInstallDirs or built-in defaults for libraries and headers
install( TARGETS ${PROJECT_NAME}
       	EXPORT lib-export
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        COMPONENT lib${PROJECT_NAME}
)

install( EXPORT lib-export
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        COMPONENT lib${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake
)

# CMake config files
write_basic_package_version_file( ${PROJECT_NAME}ConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
)

configure_package_config_file( config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake
)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake
        COMPONENT lib${PROJECT_NAME}
)

# Install .pc file
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig
        COMPONENT lib${PROJECT_NAME}
)

#
# Packaging
#

# set(CPACK_PACKAGE_NAME "SimpleApp")
#set(CPACK_PACKAGE_VENDOR "MyComp")
#set(CPACK_VERBATIM_VARIABLES true)
#set(CPACK_PACKAGE_VERSION_MAJOR 0)
#set(CPACK_PACKAGE_VERSION_MINOR 2)
#set(CPACK_PACKAGE_VERSION_PATCH 0)
#CPACK_PACKAGE_VENDOR
#CPACK_PACKAGE_DESCRIPTION_SUMMARY
#CPACK_PACKAGE_DESCRIPTION_FILE
#CPACK_RESOURCE_FILE_WELCOME
#CPACK_RESOURCE_FILE_LICENSE
#CPACK_RESOURCE_FILE_README
#include(CPack)


