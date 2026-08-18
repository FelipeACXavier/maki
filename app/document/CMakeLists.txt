# Define project name
project(libdocument VERSION 1.0)

include(GNUInstallDirs)

file(GLOB SOURCES
  "${PROJECT_SOURCE_DIR}/*.cpp"
)

add_library(${PROJECT_NAME}
  STATIC
    ${SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    libcpphelpers
    libcommon
    libmaki
    libkoda
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND BUILD_PLUGINS)
  target_compile_definitions(${PROJECT_NAME} PRIVATE DEV_BUILD=1)
endif()

target_include_directories(${PROJECT_NAME}
 PUBLIC
  ${PROJECT_SOURCE_DIR}
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
