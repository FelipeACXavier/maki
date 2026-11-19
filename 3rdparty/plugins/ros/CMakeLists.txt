# plugins/cppgenerator/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
set(PLUGIN_NAME ros_plugin)

project(${PLUGIN_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

find_package(Qt6 REQUIRED COMPONENTS
  Core
  Widgets
)

# ==================================================================
# Plugin generator
# Set the QT plugin file and the platform file
set(JSON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/ros.json)
set(PLUGIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}.json)

add_library(${PLUGIN_NAME} SHARED
  src/ros_plugin.cpp
  src/actions/ros_build.cpp
  src/actions/ros_copy.cpp
  src/actions/ros_launch.cpp

  ${JSON_FILE}
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
  Qt6::Core
  Qt6::Widgets
  libcpphelpers
  libmaki
  libkoda
)

target_include_directories(${PLUGIN_NAME}
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include(${CMAKE_SOURCE_DIR}/3rdparty/maki-sdk/cmake/maki-plugin.cmake)

maki_stage_plugin(
    PLUGIN_ID ${PLUGIN_NAME}
    TARGET ${PLUGIN_NAME}
    METADATA ${PLUGIN_FILE}
    ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets
  )
