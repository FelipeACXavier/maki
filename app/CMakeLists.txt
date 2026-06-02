cmake_minimum_required(VERSION 3.16)
project(${APPLICATION_NAME} LANGUAGES CXX)

# Compile libraries
add_subdirectory(common)
add_subdirectory(document)

qt_standard_project_setup()

# ------------------------------------------------------------------------------------------------------------
include(GNUInstallDirs)
include(${CMAKE_SOURCE_DIR}/cmake/copy_fonts.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/copy_icons.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/copy_themes.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/copy_libraries.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/doxygen.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/copy_translations.cmake)

# ------------------------------------------------------------------------------------------------------------
# Main application
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Get the source files in common and view
file(GLOB SYSTEM_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/system/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/system/undo_commands/*.cpp
)
file(GLOB ELEMENT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/elements/*.cpp)
file(GLOB WIDGET_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/structure/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/properties/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/dialogs/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/validators/*.cpp
)
file(GLOB COMPILER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/compiler/*.cpp)

qt_add_executable(${APPLICATION_NAME}
    main.cpp
    assets.qrc
    ${SYSTEM_FILES}
    ${WIDGET_FILES}
    ${ELEMENT_FILES}
    ${COMPILER_FILES}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND BUILD_PLUGINS)
  target_compile_definitions(${APPLICATION_NAME} PRIVATE DEV_BUILD=1)
endif()

if (MSVC)
  # Only set these on release so we can see the logs in the terminal
  set_target_properties(${APPLICATION_NAME} PROPERTIES
    WIN32_EXECUTABLE  $<CONFIG:Release>
    MACOSX_BUNDLE  $<CONFIG:Release>
  )
else()
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${APPLICATION_NAME} PRIVATE
      -fsanitize=address,undefined
      -fno-omit-frame-pointer
      -g
    )
    target_link_options(${APPLICATION_NAME} PRIVATE
      -fsanitize=address,undefined
    )
  endif()
endif()



target_link_libraries(${APPLICATION_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::Core5Compat
    QuaZip::QuaZip
    qlementine
    libavoid
    libcommon
    libdocument
    libcpphelpers
    libmaki
)

add_dependencies(${APPLICATION_NAME}
  copy_fonts
  copy_themes
  copy_icons
  copy_libraries
  copy_translations
)

# ------------------------------------------------------------------------------------------------------------
# Linux deployment
if (DEPLOY_TARGET STREQUAL "linux" AND UNIX AND NOT APPLE)
  # Where Qt is installed in the container
  set(QT_RUNTIME_PREFIX "${CMAKE_PREFIX_PATH}" CACHE PATH "Qt runtime prefix (Qt install dir)")

  install(TARGETS ${APPLICATION_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  add_custom_target(deploy-linux
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_INSTALL_PREFIX}"

    # 1. Regular install into CMAKE_INSTALL_PREFIX (e.g. /dist)
    COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install

    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_INSTALL_PREFIX}/plugins"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${QT_RUNTIME_PREFIX}/plugins"
            "${CMAKE_INSTALL_PREFIX}/plugins"

    # 4. Call CMake script to run ldd and copy only needed Qt libs
    COMMAND "${CMAKE_COMMAND}"
    -DAPP_PATH="${CMAKE_INSTALL_PREFIX}/bin/${APPLICATION_NAME}"
    -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
    -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}"
    -P "${CMAKE_SOURCE_DIR}/cmake/deploy-linux.cmake"

    COMMENT "Installing maki and bundling Qt runtime into ${CMAKE_INSTALL_PREFIX}"
  )

  add_dependencies(deploy-linux ${APPLICATION_NAME})
endif()

# TODO: Test this with a windows machine
# Windows deployment
if (DEPLOY_TARGET STREQUAL "windows" AND WIN32 AND NOT CMAKE_CROSSCOMPILING)
  # Make sure windeployqt is available
  find_program(QT_WINDEPLOYQT_EXECUTABLE NAMES windeployqt windeployqt.exe)

  install(TARGETS ${APPLICATION_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  if (QT_WINDEPLOYQT_EXECUTABLE)
    # Execute windeployqt as a postscript only  if running in debug mode
    # In release mode, the deploy-windows rule handles windeployqt
    message(STATUS "Found windeployqt: ${QT_WINDEPLOYQT_EXECUTABLE}")
    add_custom_command(TARGET ${APPLICATION_NAME} POST_BUILD
      # Make sure the target directory exists
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/bin"

      # Run windeployqt so the libraries are all in place
      COMMAND $<$<CONFIG:Debug>:${QT_WINDEPLOYQT_EXECUTABLE}>
              $<$<CONFIG:Debug>:--dir>
              $<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/bin>
              $<$<CONFIG:Debug>:$<TARGET_FILE:${APPLICATION_NAME}>>

      # Copy the executable to the output dir
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
              "$<TARGET_FILE:${APPLICATION_NAME}>"
              "${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:${APPLICATION_NAME}>"

      # Copy debug assets
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
              "${BUILD_ASSET_DIR}"
              "${CMAKE_BINARY_DIR}/bin/assets"

      COMMENT "Running ${QT_WINDEPLOYQT_EXECUTABLE} for $<TARGET_FILE_NAME:${APPLICATION_NAME}>"
      VERBATIM
    )
  else()
    message(WARNING "windeployqt not found; 'deploy-windows' target will be unavailable")
  endif()

  if (QT_WINDEPLOYQT_EXECUTABLE)
    add_custom_target(deploy-windows
      # Create final binary dir
      COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install --config Release

      # 1. Run windeployqt
      COMMAND "${QT_WINDEPLOYQT_EXECUTABLE}"
              --release
              --dir "${CMAKE_INSTALL_PREFIX}"
              --verbose 1
              "${CMAKE_INSTALL_PREFIX}/$<TARGET_FILE_NAME:${APPLICATION_NAME}>"

      COMMENT "Running windeployqt for ${APPLICATION_NAME}"
    )

    add_dependencies(deploy-windows ${APPLICATION_NAME})
  else()
    message(WARNING "windeployqt not found; cannot run the 'deploy-windows' target")
  endif()
endif()
