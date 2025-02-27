cmake_minimum_required(VERSION 3.16)
project(${APPLICATION_NAME} LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Compile libraries
add_subdirectory(common)

qt_standard_project_setup()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Get the source files in common and view
file(GLOB ELEMENT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/elements/*.cpp)
file(GLOB VIEW_FILES ${CMAKE_CURRENT_SOURCE_DIR}/view/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/view/*.ui)

qt_add_executable(${APPLICATION_NAME}
    main.cpp
    assets.qrc
    ${VIEW_FILES}
    ${ELEMENT_FILES}
)

set_target_properties(${APPLICATION_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(${APPLICATION_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    libcommon
    libcpphelpers
    nlohmann_json::nlohmann_json
)

# set(plugin_targets
#     jsonviewer
#     txtviewer
# )

# if(QT6_IS_SHARED_LIBS_BUILD)
#     add_dependencies(${APPLICATION_NAME} ${plugin_targets})
# else()
#     target_link_libraries(${APPLICATION_NAME} PRIVATE ${plugin_targets})
# endif()

install(TARGETS ${APPLICATION_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# install(TARGETS abstractviewer
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )
