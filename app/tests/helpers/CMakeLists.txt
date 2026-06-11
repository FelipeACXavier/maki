set(EXECUTABLE_NAME ${APPLICATION_NAME}_test_helpers)

add_library(${EXECUTABLE_NAME} STATIC
  widget_helpers.cpp
)

target_include_directories(${EXECUTABLE_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(${EXECUTABLE_NAME}
  PUBLIC
    Qt6::Core
    Qt6::Widgets
    GTest::gtest
)
