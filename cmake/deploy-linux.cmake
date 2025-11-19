# cmake/deploy_linux.cmake
# Inputs:
#  - APP_PATH: full path to installed binary (e.g. /dist/bin/maki)
#  - CMAKE_INSTALL_PREFIX: install root (e.g. /dist)

if(NOT APP_PATH)
  message(FATAL_ERROR "APP_PATH not set")
endif()

# Run ldd on the binary
execute_process(
  COMMAND ldd "${APP_PATH}"
  OUTPUT_VARIABLE LDD_OUTPUT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Extract full paths to all libQt6*.so* from ldd output
# ldd line format example:
#   libQt6Widgets.so.6 => /home/ubuntu/Qt6-Linux/lib/libQt6Widgets.so.6 (0x...)
string(REGEX MATCHALL "/[^ ]*libQt6[^ ]*" QT_LIB_PATHS "${LDD_OUTPUT}")
list(REMOVE_DUPLICATES QT_LIB_PATHS)

# Copy libs
if(QT_LIB_PATHS)
  message(STATUS "Qt libs to copy from ldd(${APP_PATH}):")
  foreach(lib IN LISTS QT_LIB_PATHS)
    # message(STATUS "Using library: ${lib}")

    # Get the full path to the actual file
    get_filename_component(real_lib "${lib}" REALPATH)

    # Get the name of the desired library, no symlinks
    get_filename_component(basename "${lib}" NAME)

    # Copy file with the expected name
    file(COPY_FILE "${real_lib}" "${CMAKE_INSTALL_PREFIX}/lib/${basename}")
  endforeach()
else()
  message(WARNING "No Qt libraries detected in ldd output for ${APP_PATH}")
endif()

# TODO: Make this more robust
# cp ~/Qt6-Linux/lib/libQt6XcbQpa.so.6.8.3 ./dist/lib/libQt6XcbQpa.so.6
file(COPY_FILE "${CMAKE_PREFIX_PATH}/lib/libQt6XcbQpa.so.6.8.3" "${CMAKE_INSTALL_PREFIX}/lib/libQt6XcbQpa.so.6")
