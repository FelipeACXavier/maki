# Define project name
project(libcommon VERSION 1.0)

add_compile_options(-O3)

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
)
# target_include_directories(${PROJECT_NAME}
#  PUBLIC
#   result
#   string
#   logging
#   safe_types
#   file_system
# )

target_include_directories(${PROJECT_NAME}
 PUBLIC
  ${PROJECT_SOURCE_DIR}
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
