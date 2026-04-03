# cmake/copy_icons.cmake

set(SRC_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")
set(BUILD_LIB_DIR "${BUILD_ASSET_DIR}/libraries")

file(GLOB APP_LIB_FILES
  ${SRC_LIB_DIR}/koda.json
)

add_custom_target(copy_libraries ALL
  COMMENT "Copying libraries to ${BUILD_LIB_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_LIB_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${APP_LIB_FILES} ${BUILD_LIB_DIR}
  DEPENDS ${APP_LIB_FILES}
)

install(FILES ${APP_LIB_FILES}
  DESTINATION "${RELEASE_ASSET_DIR}/libraries"
)
