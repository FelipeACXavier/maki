# cmake/copy_translations.cmake

file(GLOB TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.ts)

set(QM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/translations")
set(QM_OUTPUT_COPY_DIR "${BUILD_ASSET_DIR}/translations")

qt_add_translations(${APPLICATION_NAME}
    TS_FILES ${TS_FILES}
    QM_FILES_OUTPUT_VARIABLE QM_FILES
    OUTPUT_LOCATION ${QM_OUTPUT_DIR}
)

add_custom_target(copy_translations ALL
  COMMENT "Copying translations to ${QM_OUTPUT_COPY_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${QM_OUTPUT_COPY_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QM_FILES} ${QM_OUTPUT_COPY_DIR}
  DEPENDS ${QM_FILES}
)

install(FILES ${QM_FILES}
  DESTINATION "${RELEASE_ASSET_DIR}/translations"
)
