cmake_minimum_required(VERSION 3.22)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CPPHELPERS_FILE_SYSTEM OFF CACHE BOOL "Disable file system helpers in CppHelpers")

# Libraries
add_subdirectory(CppHelpers)
add_subdirectory(maki-sdk)
