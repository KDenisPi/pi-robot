cmake_minimum_required(VERSION 3.0)

#project name
project(pi-robot)

find_library(WIPI_LIB wiringPi PATH ${CMAKE_SOURCE_DIR}/../wiringPi/wiringPi)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_BIULD_TYPE Debug)

#C++ 11
#add_cxx_flags( "-std=c++11" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#cmake version 3.0 and above
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(libwiringPi STATIC IMPORTED)

add_subdirectory(${PROJECT_SOURCE_DIR}/pi-library)
add_subdirectory(${PROJECT_SOURCE_DIR}/version1)

set(EXTRA_LIBS ${EXTRA_LIBS} ${WIPI_LIB})

include_directories(${CMAKE_SOURCE_DIR}/../wiringPi/wiringPi}
   ${PROJECT_SOURCE_DIR}/pi-library/src
)



