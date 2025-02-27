cmake_minimum_required(VERSION 3.16)

add_subdirectory(CppHelpers)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)
