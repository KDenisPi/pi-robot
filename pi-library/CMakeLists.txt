cmake_minimum_required(VERSION 3.0)

#project name
project(pi-library)

include_directories(BEFORE ${WIRINGPI_INC}
   ${PROJECT_SOURCE_DIR}/../pi-utils
   ${PROJECT_SOURCE_DIR}/../pi-core
   ${JSONCONS}
)

message( STATUS "WIRINGPI_LIB ${WIRINGPI_LIB}")

aux_source_directory(${PROJECT_SOURCE_DIR} PI_LIBRARY_SOURCES)

#add_library(pi-library SHARED ${PI_LIBRARY_SOURCES})
add_library(pi-library STATIC ${PI_LIBRARY_SOURCES})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
