project(libpipeline VERSION 1.0)

include(GNUInstallDirs)

file(GLOB SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
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
    libwidgets
    libcommon
    libmaki
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND BUILD_PLUGINS)
  target_compile_definitions(${PROJECT_NAME} PRIVATE DEV_BUILD=1)
endif()

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)