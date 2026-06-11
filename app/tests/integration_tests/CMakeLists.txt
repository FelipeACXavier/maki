set(EXECUTABLE_NAME ${APPLICATION_NAME}_integration_tests)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(${EXECUTABLE_NAME}
  ../../assets.qrc
  ../tests_main.cpp
  ${SOURCES}
)

target_link_libraries(${EXECUTABLE_NAME}
  PRIVATE
    # maki_test_helpers
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    GTest::gtest
    libcpphelpers
    libmaki
)

gtest_discover_tests(${EXECUTABLE_NAME})
