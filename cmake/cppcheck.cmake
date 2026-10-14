
# cmake/code_analysis.cmake
include_guard(GLOBAL)

option(MAKI_ENABLE_CPPCHECK "Enable Cppcheck during compilation" OFF)

set(cppcheck_args
  --enable=warning,style,performance,portability
  --inline-suppr
  --template=gcc
  --library=qt
  --force
)

if(MAKI_WARNINGS_AS_ERRORS)
  list(APPEND cppcheck_args --error-exitcode=1)
endif()

# Configure warnings, sanitizers and static analysis for any MAKI target.
function(maki_enable_code_analysis target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Unknown analysis target: ${target}")
  endif()

  if(NOT MAKI_ENABLE_CPPCHECK)
    return()
  endif()

  find_program(CPPCHECK_EXECUTABLE NAMES cppcheck REQUIRED)
  set_property(TARGET ${target}
    PROPERTY CXX_CPPCHECK
      "${CPPCHECK_EXECUTABLE};${cppcheck_args}"
  )

  message(STATUS "Cppcheck enabled for ${target}")
endfunction()

function(maki_add_cppcheck_target target)
  if(NOT MAKI_ENABLE_CPPCHECK)
    return()
  endif()

  find_program(CPPCHECK_EXECUTABLE NAMES cppcheck REQUIRED)
  cmake_parse_arguments(
    ARG
    ""
    ""
    "SOURCES;INCLUDE_DIRS"
    ${ARGN}
  )

  if(NOT ARG_SOURCES)
    message(FATAL_ERROR "maki_add_cppcheck_target(${target}) requires SOURCES")
  endif()

  foreach(dir IN LISTS ARG_INCLUDE_DIRS)
    list(APPEND cppcheck_args "-I${dir}")
  endforeach()

  add_custom_target(cppcheck-${target}
    COMMAND "${CPPCHECK_EXECUTABLE}" ${cppcheck_args} ${ARG_SOURCES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Running Cppcheck on ${target}"
    COMMAND_EXPAND_LISTS
    VERBATIM
  )

  message(STATUS "Cppcheck target enabled: cppcheck-${target}")
endfunction()

