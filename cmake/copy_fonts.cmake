# cmake/copy_fonts.cmake

file(GLOB APP_FONT_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/dejavu/*.ttf
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/geist/*.ttf
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/jetbrains_mono/*.ttf
)

set(FONT_BUILD_DIR "${BUILD_ASSET_DIR}/fonts")

add_custom_target(copy_fonts ALL
  COMMENT "Copying fonts to ${FONT_BUILD_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${FONT_BUILD_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${APP_FONT_FILES} ${FONT_BUILD_DIR}
  DEPENDS ${APP_FONT_FILES}
)

install(FILES ${APP_FONT_FILES}
  DESTINATION "${RELEASE_ASSET_DIR}/fonts"
)
