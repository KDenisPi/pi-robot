cmake_minimum_required(VERSION 3.0)

#project name
project(pi-robot)

set(THIRD_PARTY_HOME "${CMAKE_SOURCE_DIR}/../third-party")

set(WIRINGPI_HOME "${THIRD_PARTY_HOME}/wiringPi")
set(WIRINGPI_LIB "${WIRINGPI_HOME}/wiringPi")

set(MOSQUITTO_HOME "${THIRD_PARTY_HOME}/mosquitto")
set(MOSQUITTO_LIB "${MOSQUITTO_HOME}/build/lib")

set(SPDLOG_HOME "${THIRD_PARTY_HOME}/spdlog")
set(SPDLOG_INCLUDE "${SPDLOG_HOME}/include")

set(JSONCONS "${THIRD_PARTY_HOME}/jsoncons/include")

message( STATUS "THIRD_PARTY_HOME ${THIRD_PARTY_HOME}")
message( STATUS "WIRINGPI_HOME ${WIRINGPI_HOME}")
message( STATUS "WIRINGPI_LIB ${WIRINGPI_LIB}")
message( STATUS "MOSQUITTO_LIB ${MOSQUITTO_LIB}")
message( STATUS "SPDLOG_HOME ${SPDLOG_HOME}")
message( STATUS "JSONCONS ${JSONCONS}")


find_library(WIPI_LIB wiringPi PATH ${WIRINGPI_LIB})
find_library(MOSQT_LIB mosquitto PATH ${MOSQUITTO_LIB})
find_library(MOSQT_LIB_CPP mosquittopp PATH ${MOSQUITTO_LIB}/cpp)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_BIULD_TYPE Debug)

#cmake version 3.0 and above
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(BEFORE 
${SPDLOG_INCLUDE}
)

#C++ 11
#add_cxx_flags( "-std=c++11" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -std=c++11 -fexceptions")
message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

add_library(libwiringPi STATIC IMPORTED)
add_library(libmosquitto STATIC IMPORTED)
add_library(libmosquittopp STATIC IMPORTED)

set(EXTRA_LIBS ${EXTRA_LIBS} ${WIPI_LIB} ${MOSQT_LIB} ${MOSQT_LIB_CPP})

add_subdirectory(${PROJECT_SOURCE_DIR}/pi-utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-library)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-smachine)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-mqqt)

#add_subdirectory(${PROJECT_SOURCE_DIR}/version1)
#add_subdirectory(${PROJECT_SOURCE_DIR}/project1)
#add_subdirectory(${PROJECT_SOURCE_DIR}/spi_test)
add_subdirectory(${PROJECT_SOURCE_DIR}/weather)




