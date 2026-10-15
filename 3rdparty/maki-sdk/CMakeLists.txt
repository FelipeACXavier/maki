cmake_minimum_required(VERSION 3.22)

project(libmaki VERSION 0.0.1)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

option(MAKI_WITH_ROS "Enable ROS package discovery" ON)

# ------------------------------------------------------------------------------------------------------------
find_package(Qt6 ${QT_VERSION} REQUIRED COMPONENTS Core)

file(GLOB SOURCES
  "${PROJECT_SOURCE_DIR}/source/*.cpp"
  "${PROJECT_SOURCE_DIR}/source/typing/*.cpp"
)

file(GLOB HEADERS
  "${PROJECT_SOURCE_DIR}/include/*.h"
  "${PROJECT_SOURCE_DIR}/include/typing/*.h"
)

add_library(${PROJECT_NAME} STATIC
  ${SOURCES}
  ${HEADERS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    include
)

target_link_libraries(${PROJECT_NAME}
 PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  libcpphelpers
)

if(MAKI_WITH_ROS)
  find_package(ament_index_cpp QUIET)
endif()
if(ament_index_cpp_FOUND)
  message(STATUS "ROS package discovery enabled")
  target_link_libraries(${PROJECT_NAME}
    PUBLIC ament_index_cpp::ament_index_cpp
  )
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE MAKI_HAS_AMENT=1
  )
else()
  message(STATUS "ROS package discovery disabled")
endif()

install(TARGETS ${PROJECT_NAME}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
