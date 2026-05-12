cmake_minimum_required(VERSION 3.16)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CPPHELPERS_FILE_SYSTEM OFF)
set(CPPHELPERS_USE_SOURCE ON)

# Libraries
add_subdirectory(CppHelpers)
add_subdirectory(maki-sdk)

add_subdirectory(qclementine)

# Build-in plugins
if (BUILD_PLUGINS)
  add_subdirectory(plugins/koda)
  add_subdirectory(plugins/platformio)
  add_subdirectory(plugins/arduino)
  add_subdirectory(plugins/ollama)
endif()
