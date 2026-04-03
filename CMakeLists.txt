cmake_minimum_required(VERSION 3.22)

set(APPLICATION_NAME maki)

option(DEBUG "Build with debug symbols" OFF)
option(BUILD_DOC "Build documentation" ON)
option(BUILD_PLUGINS "Build plugins as part of the application" ON)

# Paths as configurable cache variables
set(LOCAL_QT_PATH "" CACHE PATH "Qt path on this machine")
set(LOCAL_PROJECT_PATH "" CACHE PATH "Project path on this machine")
set(BUILD_ASSET_DIR "${CMAKE_BINARY_DIR}/assets")
set(RELEASE_ASSET_DIR "${CMAKE_INSTALL_PREFIX}/assets")

set(DEPLOY_TARGET "auto" CACHE STRING "Deployment target (auto|linux|windows)")
set_property(CACHE DEPLOY_TARGET PROPERTY STRINGS auto linux windows)

# This is used to enable the resources
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Get QT version
file(READ "${CMAKE_SOURCE_DIR}/.qt-version" QT_VERSION_RAW)
string(STRIP "${QT_VERSION_RAW}" QT_VERSION)

message(STATUS "Using Qt version from .qt-version: ${QT_VERSION}")

project(${APPLICATION_NAME} LANGUAGES CXX)

set(QT_REQUIRED_PACKAGES
  Core
  Gui
  Svg
  Widgets
  SvgWidgets
  LinguistTools
)

# Comment out to allow casting from literal string to QString
# add_compile_definitions(QT_NO_CAST_FROM_ASCII)
if (MSVC)
  add_compile_options(/W4 /MP)

  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      add_compile_options(/Zi /Od)
  elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
      add_compile_options(/O2)
  endif()

  find_package(Qt6 ${QT_VERSION} REQUIRED COMPONENTS
    ${QT_REQUIRED_PACKAGES}
  )
else()
  add_compile_options(-Wall)

  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      message("Compiling with debug symbols")
      set(LINUX_BUILD_DIR "${CMAKE_SOURCE_DIR}/build/linux/debug")
      add_compile_options(-g -O0)
  elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
      message("Compiling in release version")
      set(LINUX_BUILD_DIR "${CMAKE_SOURCE_DIR}/build/linux/release")
      add_compile_options(-O2)
  endif()

  find_package(Qt6 REQUIRED COMPONENTS
    ${QT_REQUIRED_PACKAGES}
  )
endif()


# qt_standard_project_setup(REQUIRES ${QT_VERSION})

# -----------------------------------------
# Some variables
set(PLUGINS_FOLDER ${CMAKE_BINARY_DIR}/plugins)

# -----------------------------------------
# Get the curent build type
if (DEPLOY_TARGET STREQUAL "auto")
  if (WIN32)
    set(DEPLOY_TARGET windows)
  elseif (UNIX AND NOT APPLE)
    set(DEPLOY_TARGET linux)
  else()
    message(FATAL_ERROR "Unknown deployment target")
  endif()
endif()

message(STATUS "Deploy target: ${DEPLOY_TARGET}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif()

# -----------------------------------------
# Linux stuff
if (DEPLOY_TARGET STREQUAL "linux")
  if (UNIX AND NOT APPLE)
    # When installed, binaries will look for libs in ../lib relative to themselves
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  endif()
endif()

add_subdirectory(3rdparty)
add_subdirectory(app)

# Make sure the compile_commands works in the host and not in the docker
# TODO: We should make this also available on windows
if (DEPLOY_TARGET STREQUAL "linux" AND LOCAL_QT_PATH AND LOCAL_PROJECT_PATH)
  add_custom_target(patch_compile_commands ALL
    COMMAND ${CMAKE_COMMAND}
      -DINPUT="${LINUX_BUILD_DIR}/compile_commands.json"
      -DOLD="/home/ubuntu/maki"
      -DNEW="${LOCAL_PROJECT_PATH}"
      -P "${CMAKE_SOURCE_DIR}/cmake/patch_compile_commands.cmake"

    COMMAND ${CMAKE_COMMAND}
      -DINPUT="${LINUX_BUILD_DIR}/compile_commands.json"
      -DOLD="/home/ubuntu/Qt/${QT_VERSION}/gcc_64"
      -DNEW="${LOCAL_QT_PATH}"
      -P "${CMAKE_SOURCE_DIR}/cmake/patch_compile_commands.cmake"

    COMMENT "Patching compile_commands.json with local paths"

    DEPENDS ${APPLICATION_NAME}
  )
endif()
