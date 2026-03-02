# cmake/doxygen.cmake
find_package(Doxygen)

function(maki_enable_doxygen_docs)
  find_package(Doxygen QUIET)

  if (NOT DOXYGEN_FOUND)
    message(STATUS "Doxygen not found; docs target will not be created.")
    return()
  endif()

  if (DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Not Release build; skipping Doxygen target.")
    return()
  endif()

  set(DOXYGEN_IN  ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

  add_custom_target(doc_doxygen
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
  )
endfunction()
