cmake_minimum_required(VERSION 3.0)

#project name
project(version1)

set(VER_MJR 0)
set(VER_MIN 1)

#C++ 11
#add_cxx_flags( "-std=c++11" )

#cmake version 3.0 and above
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(EXTRA_LIBS ${EXTRA_LIBS} ${WIPI_LIB})

include_directories(${PROJECT_SOURCE_DIR}/../../wiringPi/wiringPi
   ${PROJECT_SOURCE_DIR}/../pi-library
)

add_executable(version1 ${PROJECT_SOURCE_DIR}/main.cpp)

target_link_libraries(version1 ${EXTRA_LIBS})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
