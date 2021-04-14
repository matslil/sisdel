# SPDX-License-Identifier: MIT

cmake_minimum_required (VERSION 3.14)

project( ${CMAKE_PROJECT_NAME}-parser
	VERSION ${CMAKE_PROJECT_VERSION}
	HOMEPAGE_URL ${CMAKE_PROJECT_HOMEPAGE_URL}
	DESCRIPTION "${CMAKE_PROJECT_NAME} parser application"
)

add_executable( ${PROJECT_NAME} test_scanner.cc )
target_link_libraries( ${PROJECT_NAME} PRIVATE ${CMAKE_PROJECT_NAME} )

install( TARGETS ${PROJECT_NAME}
	COMPONENT parser
)

