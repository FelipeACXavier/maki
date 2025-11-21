cmake_minimum_required(VERSION 3.16)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CPPHELPERS_FILE_SYSTEM OFF CACHE BOOL "Disable file system helpers in CppHelpers")
add_subdirectory(CppHelpers)

# Plugins
# add_subdirectory(GeneratorPlugin)
# add_subdirectory(DezyneGenerator)
