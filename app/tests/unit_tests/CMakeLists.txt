set(EXECUTABLE_NAME ${APPLICATION_NAME}_unit_tests)

file(GLOB SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp
)

# Reuse the test rule
include(${CMAKE_SOURCE_DIR}/cmake/build_tests.cmake)

maki_add_test_executable(${EXECUTABLE_NAME}
  SOURCES
    ${SOURCES}
  LIBRARIES
    # maki_test_helpers
)
