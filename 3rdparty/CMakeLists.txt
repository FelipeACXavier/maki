cmake_minimum_required(VERSION 3.16)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CPPHELPERS_FILE_SYSTEM OFF)
set(CPPHELPERS_USE_SOURCE ON)
set(CPPHELPERS_USE_QT ON)

# Libraries
add_subdirectory(CppHelpers)
add_subdirectory(maki-sdk)

add_subdirectory(qclementine)

find_package(ZLIB REQUIRED)
if(NOT WIN32)
  include_directories(${ZLIB_INCLUDE_DIRS})
endif()
add_subdirectory(quazip)

# ===================================
set(LIBAVOID_DIR ${CMAKE_CURRENT_SOURCE_DIR}/adaptagrams/cola/libavoid)
file(GLOB LIBAVOID_SOURCES
  ${LIBAVOID_DIR}/*.cpp
)
if(WIN32)
  add_library(libavoid SHARED ${LIBAVOID_SOURCES})
  target_compile_definitions(libavoid PRIVATE LIBAVOID_EXPORTS)
else()
  add_library(libavoid STATIC ${LIBAVOID_SOURCES})
endif()
target_include_directories(libavoid
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/adaptagrams/cola
)
# ===================================

# Build-in plugins
if (BUILD_PLUGINS)
  add_subdirectory(plugins/koda)
  add_subdirectory(plugins/platformio)
  add_subdirectory(plugins/arduino)
  add_subdirectory(plugins/ollama)
endif()
