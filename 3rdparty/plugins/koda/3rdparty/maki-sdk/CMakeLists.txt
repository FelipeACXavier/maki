cmake_minimum_required(VERSION 3.16)

project(libmaki VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if (MSVC)
#   add_compile_options(/W4)
#   if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#       add_compile_options(/Zi /Od)
#   elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
#       add_compile_options(/O2)
#   endif()
# else()
#   add_compile_options(-Wall)
#   if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#       add_compile_options(-g -O0)
#   elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
#       add_compile_options(-O2)
#   endif()
# endif()

include(GNUInstallDirs)

# ------------------------------------------------------------------------------------------------------------
find_package(Qt6 ${QT_VERSION} REQUIRED COMPONENTS Core)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/source/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt6::Core
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
