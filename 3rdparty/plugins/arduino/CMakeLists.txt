# plugins/cppgenerator/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
set(PLUGIN_NAME arduino_plugin)

project(${PLUGIN_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
  set(antlr4-runtime_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows/share/antlr4-runtime")
  set(ANTLR4_RUNTIME_LIBRARY antlr4_shared)
else()
  find_path(ANTLR4_RUNTIME_INCLUDE_DIR
    NAMES antlr4-runtime.h
    PATH_SUFFIXES antlr4-runtime
    PATHS /usr/include /usr/local/include
  )

  find_library(ANTLR4_RUNTIME_LIBRARY
    NAMES antlr4-runtime
    PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib
  )
endif()

# If we are building with MAKI (with BUILD_PLUGINS), then just use MAKI's settings
if (NOT BUILD_PLUGINS)
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
      message(STATUS "Compiling with debug symbols")
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
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(antlr4-runtime CONFIG REQUIRED)

# ==================================================================
# Plugin generator
# Set the QT plugin file and the platform file
set(JSON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/arduino.json)
set(PLUGIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arduino_plugin.json)

add_library(${PLUGIN_NAME} SHARED
  src/arduino_plugin.cpp
  src/arduino_plugin.h
  src/actions/generate_cpp.cpp
  src/compiler/koda_arduino_plugin.cpp
  ${JSON_FILE}
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  libcpphelpers
  libmaki
  libkoda
  ${ANTLR4_RUNTIME_LIBRARY}
)

target_include_directories(${PLUGIN_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${ANTLR4_RUNTIME_INCLUDE_DIR}
)

include(${CMAKE_SOURCE_DIR}/3rdparty/maki-sdk/cmake/maki-plugin.cmake)

# Make sure the antlr libraries are available in the target
add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:${ANTLR4_RUNTIME_LIBRARY}>
    $<TARGET_FILE_DIR:${PLUGIN_NAME}>
  COMMAND_EXPAND_LISTS
)

maki_stage_plugin(
    PLUGIN_ID ${PLUGIN_NAME}
    TARGET ${PLUGIN_NAME}
    METADATA ${PLUGIN_FILE}
    ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets
  )
