# Define project name
project(libwidgets VERSION 1.0)

include(GNUInstallDirs)

file(GLOB SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/menus/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/properties/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/validators/*.cpp
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
    Qt6::Svg
    Qt6::SvgWidgets
    qlementine
    libcpphelpers
    libdocument
    libcanvas
    libcommon
    libmaki
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
