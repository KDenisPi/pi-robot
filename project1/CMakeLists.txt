cmake_minimum_required(VERSION 3.0)

#project name
project(project1)

set(VER_MJR 0)
set(VER_MIN 1)

#find_library(PI_LIB pi-library PATH ${CMAKE_BINARY_DIR})
#find_library(PI_MASHINE pi-mashine PATH ${CMAKE_BINARY_DIR})

set(CMAKE_BIULD_TYPE Debug)
set(EXTRA_LIBS ${EXTRA_LIBS} ${WIPI_LIB} pi-library pi-smashine)

include_directories(${PROJECT_SOURCE_DIR}/../../wiringPi/wiringPi
   ${PROJECT_SOURCE_DIR}/../pi-library
   ${PROJECT_SOURCE_DIR}/../pi-smashine
)

message( STATUS "EXTRA_LIBS:  ${EXTRA_LIBS}" )

aux_source_directory(${PROJECT_SOURCE_DIR} PROJECT1_SOURCES)

add_executable(project1 ${PROJECT1_SOURCES})

target_link_libraries(project1 ${EXTRA_LIBS})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
