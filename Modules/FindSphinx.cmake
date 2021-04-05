# Source: https://github.com/k0ekk0ek/cmake-sphinx/blob/master/cmake/Modules/FindSphinx.cmake
include(FindPackageHandleStandardArgs)

find_package( Python )

macro(_Sphinx_find_executable _exe)
  string(TOUPPER "${_exe}" _uc)
  # sphinx-(build|quickstart)-3 x.x.x
  # FIXME: This works on Fedora (and probably most other UNIX like targets).
  #        Windows targets and PIP installs might need some work.
  find_program(
    SPHINX_${_uc}_EXECUTABLE
    NAMES "sphinx-${_exe}-3" "sphinx-${_exe}" "sphinx-${_exe}.exe")

  if( Python_FOUND AND SPHINX_${_uc}_EXECUTABLE )
    execute_process(
      COMMAND "${SPHINX_${_uc}_EXECUTABLE}" --version
      RESULT_VARIABLE _result
      OUTPUT_VARIABLE _output
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_result EQUAL 0 AND _output MATCHES "([0-9]+\\.[0-9]+\\.[0-9]+)$")
      set(SPHINX_${_uc}_VERSION "${CMAKE_MATCH_1}")
    endif()

    add_executable(Sphinx::${_exe} IMPORTED GLOBAL)
    set_target_properties(Sphinx::${_exe} PROPERTIES
      IMPORTED_LOCATION "${SPHINX_${_uc}_EXECUTABLE}")
    set(Sphinx_${_exe}_FOUND TRUE)
  else()
    set(Sphinx_${_exe}_FOUND FALSE)
  endif()
  unset(_uc)
endmacro()

macro(_Sphinx_find_extension _ext)
  execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import ${_ext}"
    RESULT_VARIABLE _result)
  if(_result EQUAL 0)
    set(Sphinx_${_ext}_FOUND TRUE)
    list( APPEND SPHINX_EXTENSIONS "${_ext}" )
  else()
    set(Sphinx_${_ext}_FOUND FALSE)
  endif()
endmacro()

#
# Find sphinx-build and sphinx-quickstart.
#
_Sphinx_find_executable(build)
_Sphinx_find_executable(quickstart)

#
# Verify both executables are part of the Sphinx distribution.
#
if(SPHINX_BUILD_EXECUTABLE AND SPHINX_QUICKSTART_EXECUTABLE)
  if(NOT SPHINX_BUILD_VERSION STREQUAL SPHINX_QUICKSTART_VERSION)
    message(FATAL_ERROR "Versions for sphinx-build (${SPHINX_BUILD_VERSION})"
                        "and sphinx-quickstart (${SPHINX_QUICKSTART_VERSION})"
                        "do not match")
  endif()
endif()

foreach( _comp IN LISTS Sphinx_FIND_COMPONENTS )
  if( _comp STREQUAL "build" )
    # Do nothing, sphinx-build is always required.
  elseif( _comp STREQUAL "quickstart" )
    # Do nothing, sphinx-quickstart is optional, but looked up by default.
  else()
    _Sphinx_find_extension( ${_comp} )
  endif()
endforeach()

find_package_handle_standard_args(
  Sphinx
  VERSION_VAR SPHINX_VERSION
  REQUIRED_VARS SPHINX_BUILD_EXECUTABLE SPHINX_BUILD_VERSION
  HANDLE_COMPONENTS)


# Generate a conf.py template file using sphinx-quickstart.
#
# sphinx-quickstart allows for quiet operation and a lot of settings can be
# specified as command line arguments, therefore its not required to parse the
# generated conf.py.
function(_Sphinx_generate_confpy _target _cachedir)
  if(NOT TARGET Sphinx::quickstart)
    message(FATAL_ERROR "sphinx-quickstart is not available, needed by"
                        "sphinx_add_docs for target ${_target}")
  endif()

  if(NOT DEFINED SPHINX_AUTHOR)
    set(SPHINX_AUTHOR "${SPHINX_PROJECT} committers")
  endif()

  if(NOT DEFINED SPHINX_COPYRIGHT)
    string(TIMESTAMP "%Y, ${SPHINX_AUTHOR}" SPHINX_COPYRIGHT)
  endif()

  # Extensions known to quickstart, needs another parameter to enable
  set( _known_exts autodoc doctest intersphinx todo coverage imgmath mathjax ifconfig viewcode githubpages )

  set( _opts )
  list( APPEND _opts -q )
  list( APPEND _opts --no-makefile )
  list( APPEND _opts --no-batchfile )
  list( APPEND _opts -p "${PROJECT_NAME}" )
  list( APPEND _opts -a "${SPHINX_AUTHOR}" )
  list( APPEND _opts -v "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" )
  list( APPEND _opts -r "${PROJECT_VERSION}" )
  list( APPEND _opts -l "en" )
  list( APPEND _opts -d "reporoot=${CMAKE_SOURCE_DIR}" )
  list( APPEND _opts -d "sourcedir=${_outputdir}" )
  list( APPEND _opts -d "project_name=${PROJECT_NAME}" )

  if( DEFINED SPHINX_EXTENSIONS )
    foreach( _ext ${SPHINX_EXTENSIONS} )
      if( _known_exsts MATCHES "[ ^]${_ext}[ $]" )
        list( APPEND _opts "${opts} --ext-${_ext}" )
      else()
        if( _exts )
          set( _exts "${_exts},${_ext}" )
        else()
          set( _exts "${_ext}" )
        endif()
      endif()
    endforeach()
  endif()

  if( _exts )
    list( APPEND _opts "--extensions=${_exts}" )
  endif()

  # If user supplied own templates, use them
  if( SPHINX_TEMPLATEDIR )
    list( APPEND _opts -t "${SPHINX_TEMPLATEDIR}" )
  endif()

  # If rtd theme extension has been enabled, use it
  if( Sphinx_sphinx_rtd_theme_FOUND )
    list( APPEND _opts -d html_theme=sphinx_rtd_theme )
  else()
    list( APPEND _opts -d html_theme=alabaster )
  endif()

  set(_templatedir "${CMAKE_CURRENT_BINARY_DIR}/${_target}.template")
  file(MAKE_DIRECTORY "${_templatedir}")
  execute_process(
    COMMAND "${SPHINX_QUICKSTART_EXECUTABLE}" ${_opts} "${_templatedir}"
    RESULT_VARIABLE _result
    OUTPUT_QUIET)

  if(_result EQUAL 0 AND EXISTS "${_templatedir}/conf.py")
    file(COPY "${_templatedir}/conf.py" DESTINATION "${_cachedir}")
  endif()

  file(REMOVE_RECURSE "${_templatedir}")

  if(NOT _result EQUAL 0 OR NOT EXISTS "${_cachedir}/conf.py")
    message(FATAL_ERROR "Sphinx configuration file not generated for "
                        "target ${_target}")
  endif()
endfunction()

function(sphinx_add_docs _target)
  set(_opts)
  set(_single_opts OUTPUT_DIRECTORY SOURCE_DIRECTORY)
  cmake_parse_arguments(_args "${_opts}" "${_single_opts}" "${_multi_opts}" ${ARGN})

  unset(SPHINX_BREATHE_PROJECTS)

  if(NOT _args_SOURCE_DIRECTORY)
    message(FATAL_ERROR "Sphinx source directory not specified for target ${_target}")
  else()
    if(NOT IS_ABSOLUTE "${_args_SOURCE_DIRECTORY}")
      get_filename_component(_sourcedir "${_args_SOURCE_DIRECTORY}" ABSOLUTE)
    else()
      set(_sourcedir "${_args_SOURCE_DIRECTORY}")
    endif()
    if(NOT IS_DIRECTORY "${_sourcedir}")
      message(FATAL_ERROR "Sphinx source directory '${_sourcedir}' for"
                          "target ${_target} does not exist")
    endif()
  endif()

  if(_args_OUTPUT_DIRECTORY)
    set(_outputdir "${_args_OUTPUT_DIRECTORY}")
  else()
    set(_outputdir "${CMAKE_CURRENT_BINARY_DIR}/${_target}")
  endif()

  set(_cachedir "${CMAKE_CURRENT_BINARY_DIR}/${_target}.cache")
  file(MAKE_DIRECTORY "${_cachedir}")
  file(MAKE_DIRECTORY "${_cachedir}/_static")

  _Sphinx_generate_confpy(${_target} "${_cachedir}")

  add_custom_target(
    ${_target}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${_sourcedir} ${_outputdir}
    COMMAND ${SPHINX_BUILD_EXECUTABLE}
              -b html
              -d "${CMAKE_CURRENT_BINARY_DIR}/${_target}.cache/_doctrees"
              -c "${CMAKE_CURRENT_BINARY_DIR}/${_target}.cache"
              "${_outputdir}"
              "${_outputdir}/build"
    DEPENDS ${_depends})

endfunction()

