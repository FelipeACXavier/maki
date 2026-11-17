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
    tests_main.cpp
    ${MAKI_TEST_SOURCES}
  )

  target_link_libraries(${TARGET_NAME}
    PRIVATE
      GTest::gtest
      GTest::gmock
      Qt6::Core
      Qt6::Widgets
      Qt6::Test
      maki_test_helpers
      qlementine
      libcommon
      libcpphelpers
      libmaki
      ${MAKI_TEST_LIBRARIES}
  )

  gtest_discover_tests(${TARGET_NAME}
    PROPERTIES
      ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
  )
endfunction()