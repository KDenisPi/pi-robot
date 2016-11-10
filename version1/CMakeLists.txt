cmake_minimum_required(VERSION 3.0)

#project name
project(version1)

set(VER_MJR 0)
set(VER_MIN 1)

find_library(PI_LIB pi-library PATH ${CMAKE_BINARY_DIR})

set(EXTRA_LIBS ${EXTRA_LIBS} ${WIPI_LIB} pi-library)

include_directories(${PROJECT_SOURCE_DIR}/../../wiringPi/wiringPi
   ${PROJECT_SOURCE_DIR}/../pi-library
)

message( STATUS "EXTRA_LIBS:  ${EXTRA_LIBS}" )

add_executable(version1 ${PROJECT_SOURCE_DIR}/main.cpp)

target_link_libraries(version1 ${EXTRA_LIBS})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
