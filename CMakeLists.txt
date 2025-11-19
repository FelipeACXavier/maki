cmake_minimum_required(VERSION 3.16)

set(APPLICATION_NAME maki)

option(DEBUG "Build with debug symbols" OFF)

set(DEPLOY_TARGET "auto" CACHE STRING "Deployment target (auto|linux|windows)")
set_property(CACHE DEPLOY_TARGET PROPERTY STRINGS auto linux windows)

# This is used to enable the resources
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Comment out to allow casting from literal string to QString
# add_compile_definitions(QT_NO_CAST_FROM_ASCII)
add_compile_options(-Wall)

if (DEBUG)
  add_compile_options(-g -O0)
else()
  add_compile_options(-O2)
endif()

project(${APPLICATION_NAME} LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS
  Core
  Gui
  Widgets
)

qt_standard_project_setup(REQUIRES 6.8)

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

# Windows stuff
if (DEPLOY_TARGET STREQUAL "windows" AND WIN32 AND NOT CMAKE_CROSSCOMPILING)
  # You can refine this hint based on your Qt install location
  find_program(QT_WINDEPLOYQT_EXECUTABLE
    NAMES windeployqt windeployqt.exe
    HINTS "/home/ubuntu/Qt-Windows/bin"
  )

  if (QT_WINDEPLOYQT_EXECUTABLE)
    message(STATUS "Found windeployqt: ${QT_WINDEPLOYQT_EXECUTABLE}")
  else()
    message(WARNING "windeployqt not found; 'deploy-windows' target will be unavailable")
  endif()

  # We'll hook this up to a target in app/CMakeLists (see below)
endif()

# add_subdirectory(plugins)
add_subdirectory(3rdparty)
add_subdirectory(app)
