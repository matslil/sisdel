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

find_program (PANDOC pandoc)
find_program (DOT dot)
find_program (RSVG_CONVERT rsvg-convert)

if (PANDOC AND DOT AND RSVG_CONVERT)
    set (CAN_BUILD_USER_DOC 1)
else()
    set (CAN_BUILD_USER_DOC 0)
endif()

option(BUILD_USER_DOCUMENTATION "Create and install the PDF and HTML based user documentation (requires Doxygen)" ${CAN_BUILD_USER_DOC})

if(BUILD_USER_DOCUMENTATION)
    if (NOT PANDOC)
         message (FATAL_ERROR "Pandoc is needed to build the user documentation.")
    endif()

    if (NOT DOT)
         message (FATAL_ERROR "Dot is needed to build the user documentation.")
    endif()

    if (NOT RSVG_CONVERT)
         message (FATAL_ERROR "rsvg-convert is needed to build the user documentaiton.")
    endif()

    add_custom_target (doc
    		      DEPENDS html pdf)

    add_custom_target (html
    		      DEPENDS sisdel.html
		      VERBATIM)

    add_custom_target (pdf
    		      DEPENDS sisdel.pdf
		      VERBATIM)

    add_custom_command (OUTPUT sisdel.html
                      COMMAND ${PANDOC} --toc -c ${CMAKE_CURRENT_SOURCE_DIR}/templates/sisdel.css --default-image-extension=svg --data-dir=${CMAKE_CURRENT_SOURCE_DIR} -o sisdel.html ${CMAKE_CURRENT_SOURCE_DIR}/sisdel.md
		      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sisdel.md type-hierarchy-graph.svg type-relation-graph.svg CC-BY-license-88x31.png
                      COMMENT "Generating HTML user documentation with Pandoc"
                      VERBATIM)

     configure_file ("CC-BY-license-88x31.png" "CC-BY-license-88x31.png" COPYONLY)

     add_custom_command (OUTPUT sisdel.pdf
                      COMMAND ${PANDOC} --toc -c ${CMAKE_CURRENT_SOURCE_DIR}/templates/sisdel.css --default-image-extension=pdf --data-dir=${CMAKE_CURRENT_SOURCE_DIR} -o sisdel.pdf ${CMAKE_CURRENT_SOURCE_DIR}/sisdel.md
		      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sisdel.md type-hierarchy-graph.pdf type-relation-graph.pdf CC-BY-license-88x31.png ${CMAKE_CURRENT_SOURCE_DIR}/templates/default.latex
                      COMMENT "Generating PDF user documentation with Pandoc"
                      VERBATIM)

     add_custom_command (OUTPUT type-hierarchy-graph.pdf
     			COMMAND ${RSVG_CONVERT} -f pdf -o type-hierarchy-graph.pdf type-hierarchy-graph.svg
			DEPENDS type-hierarchy-graph.svg
			COMMENT "Converting type hierarchy graph from .svg to .pdf"
			VERBATIM)
			
     add_custom_command (OUTPUT type-relation-graph.pdf
     			COMMAND ${RSVG_CONVERT} -f pdf -o type-relation-graph.pdf type-relation-graph.svg
			DEPENDS type-relation-graph.svg
			COMMENT "Converting type relation graph from .svg to .pdf"
			VERBATIM)

     add_custom_command (OUTPUT type-hierarchy-graph.svg
     			COMMAND ${DOT} -Tsvg -o type-hierarchy-graph.svg ${CMAKE_CURRENT_SOURCE_DIR}/type-hierarchy-graph.dot
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/type-hierarchy-graph.dot
			COMMENT "Generating type hierarchy graph image"
			VERBATIM)

     add_custom_command (OUTPUT type-relation-graph.svg
     			COMMAND ${DOT} -Tsvg -o type-relation-graph.svg ${CMAKE_CURRENT_SOURCE_DIR}/type-relation-graph.dot
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/type-relation-graph.dot
			COMMENT "Generating type relation graph image"
			VERBATIM)

#    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)
endif()
