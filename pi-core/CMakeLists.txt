cmake_minimum_required(VERSION 3.0)

#project name
project(pi-core)

set(VER_MJR 0)
set(VER_MIN 9)

include_directories(
   ${PROJECT_SOURCE_DIR}/../pi-utils
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_CORE_SOURCES)
add_library(pi-core SHARED ${PI_CORE_SOURCES})
