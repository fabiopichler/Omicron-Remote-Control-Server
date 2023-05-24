cmake_minimum_required(VERSION 3.5)

project(Omicron-Remote-Control LANGUAGES CXX)

set(PROJECT_APP omicron-remote-control)
set(PROJECT_CORE Omicron-Remote-Control_core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

add_definitions(-DQT_DEPRECATED_WARNINGS)

find_package(Qt5 5.7 COMPONENTS Core Gui Widgets Network Bluetooth REQUIRED)

include_directories(include)
add_subdirectory(src)

#include(${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt)
