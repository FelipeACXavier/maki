cmake_minimum_required(VERSION 3.16)
project(${APPLICATION_NAME} LANGUAGES CXX)

# Compile libraries
add_subdirectory(common)
add_subdirectory(document)

qt_standard_project_setup()

# ------------------------------------------------------------------------------------------------------------
# Fonts
file(GLOB APP_FONT_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/dejavu/*.ttf
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/geist/*.ttf
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/jetbrains_mono/*.ttf
)

set(FONT_BUILD_DIR "${CMAKE_BINARY_DIR}/share/fonts")

add_custom_target(copy_fonts ALL
  COMMENT "Copying fonts to ${FONT_BUILD_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${FONT_BUILD_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${APP_FONT_FILES} ${FONT_BUILD_DIR}
  DEPENDS ${APP_FONT_FILES}
)

install(FILES ${APP_FONT_FILES}
  DESTINATION "${CMAKE_INSTALL_DATADIR}/fonts"
)

# ------------------------------------------------------------------------------------------------------------
# Icons
set(FONT_AWESOME_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/fontawesome/svgs/solid")
set(ICON_BUILD_DIR "${CMAKE_BINARY_DIR}/share/icons")
file(GLOB APP_ICON_FILES
  ${FONT_AWESOME_PATH}/circle-nodes.svg
  ${FONT_AWESOME_PATH}/cubes.svg
  ${FONT_AWESOME_PATH}/vector-square.svg
  ${FONT_AWESOME_PATH}/xmark.svg
  ${FONT_AWESOME_PATH}/circle-xmark.svg
  ${FONT_AWESOME_PATH}/equals.svg
  ${FONT_AWESOME_PATH}/plus.svg
  ${FONT_AWESOME_PATH}/circle-play.svg
  ${FONT_AWESOME_PATH}/circle-pause.svg
  ${FONT_AWESOME_PATH}/bullseye.svg
  ${FONT_AWESOME_PATH}/rocket.svg
  ${FONT_AWESOME_PATH}/terminal.svg
  ${FONT_AWESOME_PATH}/display.svg
  ${FONT_AWESOME_PATH}/diagram-next.svg
  ${FONT_AWESOME_PATH}/newspaper.svg
  ${FONT_AWESOME_PATH}/folder-tree.svg
  ${FONT_AWESOME_PATH}/code-branch.svg
  ${FONT_AWESOME_PATH}/sliders.svg
  ${FONT_AWESOME_PATH}/rectangle-list.svg
  ${FONT_AWESOME_PATH}/gear.svg
  ${FONT_AWESOME_PATH}/paint-roller.svg
  ${FONT_AWESOME_PATH}/trash.svg
  ${FONT_AWESOME_PATH}/temperature-half.svg
  ${FONT_AWESOME_PATH}/circle-info.svg
  ${FONT_AWESOME_PATH}/triangle-exclamation.svg
  ${FONT_AWESOME_PATH}/circle-xmark.svg
  ${FONT_AWESOME_PATH}/arrow-rotate-left.svg
  ${FONT_AWESOME_PATH}/circle-check.svg
  ${FONT_AWESOME_PATH}/circle-xmark.svg
  ${FONT_AWESOME_PATH}/caret-right.svg
  ${FONT_AWESOME_PATH}/chevron-up.svg
  ${FONT_AWESOME_PATH}/chevron-down.svg
  ${FONT_AWESOME_PATH}/chevron-right.svg
  ${FONT_AWESOME_PATH}/toggle-on.svg
  ${FONT_AWESOME_PATH}/toggle-off.svg
  ${FONT_AWESOME_PATH}/minus.svg
  ${FONT_AWESOME_PATH}/chevron-up-dark.svg
  ${FONT_AWESOME_PATH}/chevron-down-dark.svg
  ${FONT_AWESOME_PATH}/toggle-on-dark.svg
  ${FONT_AWESOME_PATH}/toggle-off-dark.svg
  ${FONT_AWESOME_PATH}/minus-dark.svg
  ${FONT_AWESOME_PATH}/shield-cat.svg
  ${FONT_AWESOME_PATH}/eye.svg
  ${FONT_AWESOME_PATH}/eye-slash.svg
  ${FONT_AWESOME_PATH}/plug.svg
)

add_custom_target(copy_icons ALL
  COMMENT "Copying fonts to ${ICON_BUILD_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ICON_BUILD_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${APP_ICON_FILES} ${ICON_BUILD_DIR}
  DEPENDS ${APP_ICON_FILES}
)

install(FILES ${APP_ICON_FILES}
  DESTINATION "${CMAKE_INSTALL_DATADIR}/icons"
)

# ------------------------------------------------------------------------------------------------------------
# Themes
file(GLOB APP_THEME_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/themes/*.theme
)

set(THEME_BUILD_DIR ${CMAKE_BINARY_DIR}/share/themes)

add_custom_target(copy_themes ALL
  COMMENT "Copying themes to ${THEME_BUILD_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${THEME_BUILD_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${APP_THEME_FILES} ${THEME_BUILD_DIR}
  DEPENDS ${APP_THEME_FILES}
)

install(FILES ${APP_THEME_FILES}
  DESTINATION "${CMAKE_INSTALL_DATADIR}/themes"
)

# ------------------------------------------------------------------------------------------------------------
# Main application
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Get the source files in common and view
file(GLOB SYSTEM_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/system/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/system/undo_commands/*.cpp
)
file(GLOB ELEMENT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/elements/*.cpp)
file(GLOB WIDGET_FILES ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp)
file(GLOB STRUCTURAL_WIDGET_FILES ${CMAKE_CURRENT_SOURCE_DIR}/widgets/structure/*.cpp)
file(GLOB PROPERTY_WIDGET_FILES ${CMAKE_CURRENT_SOURCE_DIR}/widgets/properties/*.cpp)
file(GLOB COMPILER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/compiler/*.cpp)

# message("view files: ${VIEW_FILES}")
qt_add_executable(${APPLICATION_NAME}
    main.cpp
    assets.qrc
    ${SYSTEM_FILES}
    ${WIDGET_FILES}
    ${STRUCTURAL_WIDGET_FILES}
    ${PROPERTY_WIDGET_FILES}
    ${ELEMENT_FILES}
    ${COMPILER_FILES}
)

if (MSVC)
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


set_target_properties(${APPLICATION_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(${APPLICATION_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::WebEngineCore
    Qt6::WebEngineWidgets
    libcommon
    libdocument
    libcpphelpers
    libmaki
)

add_dependencies(${APPLICATION_NAME} copy_fonts copy_themes copy_icons)

install(TARGETS ${APPLICATION_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# ------------------------------------------------------------------------------------------------------------
# Linux deployment
if (DEPLOY_TARGET STREQUAL "linux" AND UNIX AND NOT APPLE)
  # Where Qt is installed in the container
  set(QT_RUNTIME_PREFIX "${CMAKE_PREFIX_PATH}" CACHE PATH "Qt runtime prefix (Qt install dir)")

  add_custom_target(deploy-linux
    # 1. Regular install into CMAKE_INSTALL_PREFIX (e.g. /dist)
    COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install

    # 2. Copy Qt plugins (platforms, imageformats, etc.)
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_INSTALL_PREFIX}/plugins"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${QT_RUNTIME_PREFIX}/plugins"
            "${CMAKE_INSTALL_PREFIX}/plugins"

    # 3. Copy our language plugins
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${PLUGINS_FOLDER}"
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
  if (QT_WINDEPLOYQT_EXECUTABLE)
    add_custom_target(deploy-windows
      # 1. Run windeployqt
      COMMAND "${QT_WINDEPLOYQT_EXECUTABLE}" --release
              --dir "${CMAKE_INSTALL_PREFIX}"
              "${CMAKE_BINARY_DIR}/Release/${APPLICATION_NAME}.exe"

      # 2. Copy our language plugins
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
              "${PLUGINS_FOLDER}/Release"
              "${CMAKE_INSTALL_PREFIX}/plugins"

      # 3. Copy the themes
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/share/themes
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
              "${THEME_BUILD_DIR}"
              "${CMAKE_INSTALL_PREFIX}/share/themes"

      # 4. Copy the fonts
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/share/fonts
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
              "${FONT_BUILD_DIR}"
              "${CMAKE_INSTALL_PREFIX}/share/fonts"

      COMMENT "Running windeployqt for ${APPLICATION_NAME}"
    )

    add_dependencies(deploy-windows ${APPLICATION_NAME})
  else()
    message(WARNING "windeployqt not found; cannot run the 'deploy-windows' target")
  endif()
endif()
