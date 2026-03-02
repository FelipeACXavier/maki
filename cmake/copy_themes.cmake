# cmake/copy_themes.cmake

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
