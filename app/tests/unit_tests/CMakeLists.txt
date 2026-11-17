set(EXECUTABLE_NAME ${APPLICATION_NAME}_unit_tests)

file(GLOB SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp
  # unit_tests/../../
  ${CMAKE_CURRENT_SOURCE_DIR}/../../system/undo_commands/*.cpp
)

# Reuse the test rule
include(${CMAKE_SOURCE_DIR}/cmake/build_tests.cmake)

maki_add_test_executable(${EXECUTABLE_NAME}
  SOURCES
    ${SOURCES}
  LIBRARIES
    libwidgets
    libcanvas
    libpipeline
)