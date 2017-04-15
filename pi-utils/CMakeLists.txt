cmake_minimum_required(VERSION 3.0)

#project name
project(pi-utils)

set(CMAKE_BIULD_TYPE Debug)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_UTILS_SOURCES)

#add_library(pi-utils SHARED ${PI_UTILS_SOURCES})
add_library(pi-utils STATIC ${PI_UTILS_SOURCES})
