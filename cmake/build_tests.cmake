function(maki_add_test_executable TARGET_NAME)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs SOURCES LIBRARIES)

  cmake_parse_arguments(MAKI_TEST
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
  )

  add_executable(${TARGET_NAME}
    ../../assets.qrc
    ../tests_main.cpp
    ${MAKI_TEST_SOURCES}
  )

  target_link_libraries(${TARGET_NAME}
    PRIVATE
      Qt6::Core
      Qt6::Widgets
      Qt6::Test
      GTest::gtest
      maki_test_helpers
      qlementine
      libwidgets
      libcanvas
      libcpphelpers
      libmaki
      ${MAKI_TEST_LIBRARIES}
  )

  gtest_discover_tests(${TARGET_NAME})
endfunction()
