# cmake/copy_templates.cmake

file(GLOB APP_TEMPLATES_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/templates/*.maki
)

set(TEMPLATE_BUILD_DIR ${BUILD_ASSET_DIR}/templates)

add_custom_target(copy_templates ALL
  COMMENT "Copying templates to ${TEMPLATE_BUILD_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${TEMPLATE_BUILD_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${APP_TEMPLATES_FILES} ${TEMPLATE_BUILD_DIR}
  DEPENDS ${APP_TEMPLATES_FILES}
)

install(FILES ${APP_TEMPLATES_FILES}
  DESTINATION "${RELEASE_ASSET_DIR}/templates"
)
