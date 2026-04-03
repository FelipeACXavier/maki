# cmake/maki-plugin.cmake

include(GNUInstallDirs)
include(CMakeParseArguments)

function(maki_stage_plugin)
  set(options)
  set(oneValueArgs
    PLUGIN_ID
    TARGET
    METADATA
    ASSET_DIR
  )
  set(multiValueArgs
    BIN_TARGETS
    FILES
  )

  cmake_parse_arguments(MAKI "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT MAKI_PLUGIN_ID)
    message(FATAL_ERROR "maki_stage_plugin: PLUGIN_ID is required")
  endif()

  if(NOT MAKI_TARGET)
    message(FATAL_ERROR "maki_stage_plugin: TARGET is required")
  endif()

  if(NOT TARGET ${MAKI_TARGET})
    message(FATAL_ERROR "maki_stage_plugin: target '${MAKI_TARGET}' does not exist")
  endif()

  message(INFO "Staging plugin: ${MAKI_PLUGIN_ID}")
  message(INFO "Target: ${MAKI_TARGET}")

  if (BUILD_PLUGINS)
    set(_plugin_root_build "${CMAKE_BINARY_DIR}/plugins/${MAKI_PLUGIN_ID}")
    set(_plugin_root_install "${CMAKE_INSTALL_PREFIX}/maki/plugins/${MAKI_PLUGIN_ID}")
  else ()
    set(_plugin_root_build "${CMAKE_BINARY_DIR}/${MAKI_TARGET}")
    set(_plugin_root_install "${CMAKE_INSTALL_PREFIX}/plugins/${MAKI_TARGET}")
  endif()

  set(_plugin_bin_build "${_plugin_root_build}/bin")
  set(_plugin_assets_build "${_plugin_root_build}/assets")

  set(_plugin_bin_install "${_plugin_root_install}/bin")
  set(_plugin_assets_install "${_plugin_root_install}/assets")

  # Main plugin library goes into plugin/bin in the build tree
  set_target_properties(${MAKI_TARGET} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${_plugin_bin_build}"
    RUNTIME_OUTPUT_DIRECTORY "${_plugin_bin_build}"
    ARCHIVE_OUTPUT_DIRECTORY "${_plugin_bin_build}"
  )

  foreach(cfg DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(${MAKI_TARGET} PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY_${cfg} "${_plugin_bin_build}"
      RUNTIME_OUTPUT_DIRECTORY_${cfg} "${_plugin_bin_build}"
      ARCHIVE_OUTPUT_DIRECTORY_${cfg} "${_plugin_bin_build}"
    )
  endforeach()

  # Stage helper/runtime executables into the same plugin/bin folder
  foreach(_bin_target IN LISTS MAKI_BIN_TARGETS)
    if(NOT TARGET ${_bin_target})
      message(FATAL_ERROR "maki_stage_plugin: BIN_TARGET '${_bin_target}' does not exist")
    endif()

    set_target_properties(${_bin_target} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${_plugin_bin_build}"
      LIBRARY_OUTPUT_DIRECTORY "${_plugin_bin_build}"
      ARCHIVE_OUTPUT_DIRECTORY "${_plugin_bin_build}"
    )

    foreach(cfg DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
      set_target_properties(${_bin_target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${cfg} "${_plugin_bin_build}"
        LIBRARY_OUTPUT_DIRECTORY_${cfg} "${_plugin_bin_build}"
        ARCHIVE_OUTPUT_DIRECTORY_${cfg} "${_plugin_bin_build}"
      )
    endforeach()

    install(TARGETS ${_bin_target}
      RUNTIME DESTINATION "${_plugin_bin_install}"
      LIBRARY DESTINATION "${_plugin_bin_install}"
      ARCHIVE DESTINATION "${_plugin_bin_install}"
    )
  endforeach()

  # Install main plugin target
  install(TARGETS ${MAKI_TARGET}
    RUNTIME DESTINATION "${_plugin_bin_install}"
    LIBRARY DESTINATION "${_plugin_bin_install}"
    ARCHIVE DESTINATION "${_plugin_bin_install}"
  )

  # Metadata
  if(MAKI_METADATA)
    add_custom_command(TARGET ${MAKI_TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${_plugin_root_build}"
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              "${MAKI_METADATA}"
              "${_plugin_root_build}/plugin.json"
      COMMENT "Staging plugin metadata for ${MAKI_PLUGIN_ID}"
    )

    install(FILES "${MAKI_METADATA}"
      DESTINATION "${_plugin_root_install}"
      RENAME "plugin.json"
    )
  endif()

  # Asset directory
  if(MAKI_ASSET_DIR)
    add_custom_command(TARGET ${MAKI_TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${_plugin_assets_build}"
      COMMAND ${CMAKE_COMMAND} -E copy_directory
              "${MAKI_ASSET_DIR}"
              "${_plugin_assets_build}"
      COMMENT "Staging plugin assets for ${MAKI_PLUGIN_ID}"
    )

    install(DIRECTORY "${MAKI_ASSET_DIR}/"
      DESTINATION "${_plugin_assets_install}"
    )
  endif()

  # Arbitrary files, such as .dzn libraries
  if(MAKI_FILES)
    # add_custom_command(TARGET ${MAKI_TARGET} POST_BUILD
    #   COMMAND ${CMAKE_COMMAND} -E make_directory "${_plugin_bin_build}"
    #   COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #           ${MAKI_FILES}
    #           "${_plugin_bin_build}"
    #   COMMENT "Staging extra plugin files for ${MAKI_PLUGIN_ID}"
    # )

    install(FILES ${MAKI_FILES}
      DESTINATION "${_plugin_bin_install}"
    )
  endif()

  # Convenience compile definition for code that wants to know its plugin id
  target_compile_definitions(${MAKI_TARGET} PRIVATE
    MAKI_PLUGIN_ID="${MAKI_PLUGIN_ID}"
  )
endfunction()
