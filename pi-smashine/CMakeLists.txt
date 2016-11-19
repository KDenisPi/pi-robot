cmake_minimum_required(VERSION 3.0)

#project name
project(pi-smashine)

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/../../wiringPi/wiringPi
   ${PROJECT_SOURCE_DIR}/../pi-library
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_SMASHINE_SOURCES)

add_library(pi-smashine SHARED ${PI_SMASHINE_SOURCES})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
