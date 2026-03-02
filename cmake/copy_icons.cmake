# cmake/copy_icons.cmake

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
  ${FONT_AWESOME_PATH}/rectangle-list.svg
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
