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

cmake_minimum_required( VERSION 3.9 )

set( SPHINX_TEMPLATEDIR "${CMAKE_CURRENT_SOURCE_DIR}/templates" )

find_package( Doxygen )
find_package( Sphinx COMPONENTS breathe exhale sphinx_rtd_theme )

if ( Sphinx_FOUND AND Doxygen_FOUND )
    set( CAN_BUILD_USER_DOC 1 )
else()
    set( CAN_BUILD_USER_DOC 0 )
    message( STATUS "Not all tools found to build documentation" )
endif()

if( CAN_BUILD_USER_DOC )
    set( DOXYGEN_GENERATE_HTML NO )
    set( DOXYGEN_GENERATE_XML YES )
    doxygen_add_docs( sisdel_doxygen "../src" )

    sphinx_add_docs(
	    doc
	    BREATHE_PROJECTS sisdel_doxygen
	    BUILDER html
	    SOURCE_DIRECTORY .
	    )
    
#    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)
endif()
