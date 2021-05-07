# SPDX-License-Identifier: MIT

cmake_minimum_required( VERSION 3.9 )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Modules" )

set( SPHINX_TEMPLATEDIR "${CMAKE_CURRENT_SOURCE_DIR}/templates" )

find_package( Sphinx COMPONENTS breathe exhale sphinx_rtd_theme sphinxcontrib.plantuml REQUIRED )

sphinx_add_docs(
    doc
    SOURCE_DIRECTORY .
)

install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/build/
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc
        COMPONENT doc
)

