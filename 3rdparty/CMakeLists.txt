cmake_minimum_required(VERSION 3.16)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CPPHELPERS_FILE_SYSTEM OFF)
set(CPPHELPERS_USE_SOURCE ON)

# Libraries
add_subdirectory(CppHelpers)
add_subdirectory(maki-sdk)

set(QLEMENTINE_SHOWCASE ON)
set(QLEMENTINE_SANDBOX ON)
add_subdirectory(qclementine)

# Build-in plugins
if (BUILD_PLUGINS)
add_subdirectory(plugins/koda)
endif()
