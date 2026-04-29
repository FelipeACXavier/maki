# cmake/copy_icons.cmake

set(MISSION_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/mission")
set(SHAPES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/shapes")
set(FONT_AWESOME_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/fontawesome/svgs/solid")
set(ICON_BUILD_DIR "${BUILD_ASSET_DIR}/icons")

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
  ${FONT_AWESOME_PATH}/angle-right.svg
  ${FONT_AWESOME_PATH}/angles-right.svg
  ${MISSION_PATH}/icon_start.svg
  ${MISSION_PATH}/koda_async.svg
  ${MISSION_PATH}/koda_continue.svg
  ${MISSION_PATH}/koda_every.svg
  ${MISSION_PATH}/koda_failure.svg
  ${MISSION_PATH}/koda_join.svg
  ${MISSION_PATH}/koda_repeat.svg
  ${MISSION_PATH}/koda_start.svg
  ${MISSION_PATH}/koda_strategy.svg
  ${MISSION_PATH}/koda_success.svg
  ${MISSION_PATH}/koda_sync.svg
  ${MISSION_PATH}/koda_within.svg
  ${MISSION_PATH}/node_start.svg
  ${MISSION_PATH}/node_success.svg
  ${MISSION_PATH}/node_failure.svg
  ${MISSION_PATH}/node_join.svg
  ${MISSION_PATH}/node_strategy.svg
  ${MISSION_PATH}/node_within.svg
  ${MISSION_PATH}/node_repeat.svg
  ${MISSION_PATH}/node_every.svg
  ${MISSION_PATH}/node_continue.svg
  ${MISSION_PATH}/node_task.svg
  ${MISSION_PATH}/node_subtask_placeholder.svg
  ${MISSION_PATH}/port_in.svg
  ${MISSION_PATH}/port_out.svg
  ${SHAPES_PATH}/shape_start.svg
  ${SHAPES_PATH}/shape_middle.svg
  ${SHAPES_PATH}/shape_end.svg
)

add_custom_target(copy_icons ALL
  COMMENT "Copying icons to ${ICON_BUILD_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ICON_BUILD_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${APP_ICON_FILES} ${ICON_BUILD_DIR}
  DEPENDS ${APP_ICON_FILES}
)

install(FILES ${APP_ICON_FILES}
  DESTINATION "${RELEASE_ASSET_DIR}/icons"
)
