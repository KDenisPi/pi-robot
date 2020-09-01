cmake_minimum_required(VERSION 3.0)

#project name
project(pi-utils)

set(VER_MJR 0)
set(VER_MIN 9)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_UTILS_SOURCES)

#IF(REAL_HARDWARE)
#    add_library(pi-utils SHARED ${PI_UTILS_SOURCES})
#ELSE(REAL_HARDWARE)
    add_library(pi-utils STATIC ${PI_UTILS_SOURCES})
#ENDIF(REAL_HARDWARE)

