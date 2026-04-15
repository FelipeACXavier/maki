# cmake/copy_translations.cmake

file(GLOB TS_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/translations/*.ts")

set(QM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/translations")
set(QM_OUTPUT_COPY_DIR "${BUILD_ASSET_DIR}/translations")

# Set OUTPUT_LOCATION on each .ts file so lrelease writes .qm files to QM_OUTPUT_DIR.
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${QM_OUTPUT_DIR}")

# IMMEDIATE_CALL avoids the deferred cmake_language(EVAL CODE ...) path in Qt 6.8's
# Qt6LinguistToolsMacros.cmake which breaks on paths containing spaces.
qt_add_translations(${APPLICATION_NAME}
    IMMEDIATE_CALL
    TS_FILES ${TS_FILES}
    QM_FILES_OUTPUT_VARIABLE QM_FILES
)

add_custom_target(copy_translations ALL
  COMMENT "Copying translations to ${QM_OUTPUT_COPY_DIR}"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${QM_OUTPUT_COPY_DIR}"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${QM_FILES} "${QM_OUTPUT_COPY_DIR}"
  DEPENDS ${QM_FILES}
)

install(FILES ${QM_FILES}
  DESTINATION "${RELEASE_ASSET_DIR}/translations"
)
