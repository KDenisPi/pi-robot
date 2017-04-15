cmake_minimum_required(VERSION 3.0)

#project name
project(pi-library)

set(CMAKE_BIULD_TYPE Debug)
include_directories(BEFORE ${WIRINGPI_LIB}
   ${PROJECT_SOURCE_DIR}/../pi-utils
)

message( STATUS "WIRINGPI_LIB ${WIRINGPI_LIB}")

aux_source_directory(${PROJECT_SOURCE_DIR} PI_LIBRARY_SOURCES)

#add_library(pi-library SHARED ${PI_LIBRARY_SOURCES})
add_library(pi-library STATIC ${PI_LIBRARY_SOURCES})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
