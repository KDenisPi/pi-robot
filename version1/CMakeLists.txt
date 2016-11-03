cmake_minimum_required(VERSION 3.0)

#project name
project(version1)

set(VER_MJR 0)
set(VER_MIN 1)

#find_library(WIPI_LIB wiringPi PATH ${CMAKE_SOURCE_DIR}/../wiringPi/wiringPi)

#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
#set(CMAKE_BIULD_TYPE Debug)

#C++ 11
#add_cxx_flags( "-std=c++11" )

#cmake version 3.0 and above
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#add_library(libwiringPi STATIC IMPORTED)

#set(EXTRA_LIBS ${EXTRA_LIBS} ${WIPI_LIB})

include_directories(${CMAKE_SOURCE_DIR}/../wiringPi/wiringPi}
   ${CMAKE_SOURCE_DIR}/pi-library/src
)

add_executable(version1 ${PROJECT_SOURCE_DIR}/main.cpp)

target_link_libraries(version1 ${EXTRA_LIBS})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
