cmake_minimum_required(VERSION 3.22)

project(libmaki VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
  libcpphelpers
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
