# cmake/deploy-linux.cmake

if(NOT APP_PATH)
  message(FATAL_ERROR "APP_PATH not set")
endif()

if(NOT CMAKE_PREFIX_PATH)
  message(FATAL_ERROR "CMAKE_PREFIX_PATH (Qt prefix) not set")
endif()

# Use the first item in CMAKE_PREFIX_PATH as Qt runtime prefix
list(GET CMAKE_PREFIX_PATH 0 QT_RUNTIME_PREFIX)
message(STATUS "Qt runtime prefix: ${QT_RUNTIME_PREFIX}")

# Run ldd on the binary
execute_process(
  COMMAND ldd "${APP_PATH}"
  OUTPUT_VARIABLE LDD_OUTPUT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Split into lines
string(REPLACE "\n" ";" LDD_LINES "${LDD_OUTPUT}")

set(BUNDLE_LIBS "")

foreach(line IN LISTS LDD_LINES)
  if(line MATCHES "=> (/[^ ]+)")
    set(full_path "${CMAKE_MATCH_1}")

    # Only bundle libraries that live inside the Qt runtime prefix
    if(full_path MATCHES "^${QT_RUNTIME_PREFIX}")
      list(APPEND BUNDLE_LIBS "${full_path}")
    endif()
  endif()
endforeach()

list(REMOVE_DUPLICATES BUNDLE_LIBS)

if(BUNDLE_LIBS)
  message(STATUS "Qt-related libs to copy from ldd(${APP_PATH}):")
  foreach(lib IN LISTS BUNDLE_LIBS)
    message(STATUS "  ${lib}")
    get_filename_component(real_lib "${lib}" REALPATH)
    get_filename_component(basename "${lib}" NAME)
    file(COPY_FILE "${real_lib}" "${CMAKE_INSTALL_PREFIX}/lib/${basename}")
  endforeach()
else()
  message(WARNING "No Qt-related libraries detected in ldd output for ${APP_PATH}")
endif()

# TODO: Make this more robust
file(COPY_FILE "${CMAKE_PREFIX_PATH}/lib/libQt6XcbQpa.so.6.8.3" "${CMAKE_INSTALL_PREFIX}/lib/libQt6XcbQpa.so.6")
file(COPY_FILE "${CMAKE_PREFIX_PATH}/lib/libQt6Svg.so.6.8.3" "${CMAKE_INSTALL_PREFIX}/lib/libQt6Svg.so.6")

set(QT_RESOURCES_DIR "${CMAKE_PREFIX_PATH}/resources")
set(APP_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/resources")

file(GLOB QTWEBENGINE_PAKS
  ${CMAKE_PREFIX_PATH}/resources/qtwebengine_*.pak
)

set(QTWEBENGINE_FILES
  "${QTWEBENGINE_PAKS}"
  "${CMAKE_PREFIX_PATH}/resources/icudtl.dat"
  "${CMAKE_PREFIX_PATH}/resources/v8_context_snapshot.bin"
)

file(COPY
  ${QTWEBENGINE_FILES}
  DESTINATION "${APP_RESOURCES_DIR}"
)

file(COPY
  "${CMAKE_PREFIX_PATH}/translations/qtwebengine_locales"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/translations/"
)

file(COPY
  "${CMAKE_PREFIX_PATH}/libexec/QtWebEngineProcess"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/libexec/"
)

# Copy webengine stuff
# execute_process(
#   COMMAND mkdir -p "${CMAKE_INSTALL_PREFIX}/resources"
#   COMMAND cp "${CMAKE_PREFIX_PATH}/resources/qtwebengine*" "${CMAKE_INSTALL_PREFIX}/resources/"
#   COMMAND cp "${CMAKE_PREFIX_PATH}/resources/icudtl.dat" "${CMAKE_INSTALL_PREFIX}/resources/"

#   COMMAND mkdir -p "${CMAKE_INSTALL_PREFIX}/translations"
#   COMMAND cp -r "${CMAKE_PREFIX_PATH}/translations/qtwebengine_locales" "${CMAKE_INSTALL_PREFIX}/translations/"

#   COMMAND mkdir -p "${CMAKE_INSTALL_PREFIX}/libexec"
#   COMMAND cp -r "${CMAKE_PREFIX_PATH}/libexec/QtWebEngineProcess" "${CMAKE_INSTALL_PREFIX}/libexec/"
# )
