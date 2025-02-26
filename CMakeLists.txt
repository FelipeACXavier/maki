
cmake_minimum_required(VERSION 3.16)

set(APPLICATION_NAME editor)

# This is used to enable the resources
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_compile_options(-Wall)
add_compile_options(-O2)
add_compile_options(-std=c++17)

project(${APPLICATION_NAME} LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets
             OPTIONAL_COMPONENTS PrintSupport Pdf PdfWidgets Quick3D)

qt_standard_project_setup(REQUIRES 6.8)

# add_compile_definitions(QT_NO_CAST_FROM_ASCII)

# add_subdirectory(plugins)
add_subdirectory(3rdparty/CppHelpers)
add_subdirectory(app)
