cmake_minimum_required(VERSION 3.0)

#project name
project(version1)

set(VER_MJR 0)
set(VER_MIN 1)

find_library(PI_LIB pi-library PATH ${CMAKE_BINARY_DIR})
find_library(PI_MQQT pi-mqqt PATH ${CMAKE_BINARY_DIR})

set(CMAKE_BIULD_TYPE Debug)
set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-mqqt pi-library)

include_directories(BEFORE ${MOSQUITTO_HOME}/lib/cpp
   ${MOSQUITTO_HOME}/lib
   ${WIRINGPI_LIB}
   ${PROJECT_SOURCE_DIR}/../pi-mqqt
   ${PROJECT_SOURCE_DIR}/../pi-library
   ${PROJECT_SOURCE_DIR}/../pi-utils
   ${JSONCONS}
)

aux_source_directory(${PROJECT_SOURCE_DIR} VERSION1_SOURCES)

add_executable(version1 ${VERSION1_SOURCES})

target_link_libraries(version1 ${EXTRA_LIBS})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
