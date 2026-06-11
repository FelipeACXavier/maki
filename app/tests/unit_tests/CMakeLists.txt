set(EXECUTABLE_NAME ${APPLICATION_NAME}_unit_tests)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(${EXECUTABLE_NAME}
  ../../assets.qrc
  ../tests_main.cpp
  ${SOURCES}
)

target_link_libraries(${EXECUTABLE_NAME}
  PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    GTest::gtest
    maki_test_helpers
    libwidgets
    libcpphelpers
    libmaki
)

gtest_discover_tests(${EXECUTABLE_NAME})
