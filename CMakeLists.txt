cmake_minimum_required(VERSION 3.0)

#project name
project(pi-robot)

set(THIRD_PARTY_HOME "${CMAKE_SOURCE_DIR}/../third-party")

set(WIRINGPI_HOME "${THIRD_PARTY_HOME}/wiringPi")
set(WIRINGPI_INC "${WIRINGPI_HOME}/wiringPi")
set(WIRINGPI_LIB "/usr/local/lib")

set(MOSQUITTO_HOME "${THIRD_PARTY_HOME}/mosquitto")
set(MOSQUITTO_LIB "${MOSQUITTO_HOME}/build/lib")

#set(SPDLOG_HOME "${THIRD_PARTY_HOME}/spdlog")
#set(SPDLOG_INCLUDE "${SPDLOG_HOME}/include")

set(JSONCONS "${THIRD_PARTY_HOME}/jsoncons/include")

set(MONGOOSE_HOME "${THIRD_PARTY_HOME}/mongoose-5.1")
set(MONGOOSE_SRC "${MONGOOSE_HOME}/mongoose.c")

set(SQLITE3_HOME "${THIRD_PARTY_HOME}/sqlite-amalgamation-3240000")

message( STATUS "THIRD_PARTY_HOME ${THIRD_PARTY_HOME}")
message( STATUS "WIRINGPI_HOME ${WIRINGPI_HOME}")
message( STATUS "WIRINGPI_LIB ${WIRINGPI_LIB}")
message( STATUS "MOSQUITTO_LIB ${MOSQUITTO_LIB}")
message( STATUS "SPDLOG_HOME ${SPDLOG_HOME}")
message( STATUS "JSONCONS ${JSONCONS}")
message( STATUS "MONGOOSE_HOME ${MONGOOSE_HOME}")
message( STATUS "MONGOOSE_SRC ${MONGOOSE_SRC}")


find_library(WIPI_LIB wiringPi PATH ${WIRINGPI_LIB})
find_library(WIPI_LIB_DEV wiringPiDev PATH ${WIRINGPI_LIB})
find_library(MOSQT_LIB mosquitto PATH ${MOSQUITTO_LIB})
find_library(MOSQT_LIB_CPP mosquittopp PATH ${MOSQUITTO_LIB}/cpp)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_BIULD_TYPE Debug)

#cmake version 3.0 and above
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#include_directories(BEFORE
#${SPDLOG_INCLUDE}
#)

#C++ 11
#add_cxx_flags( "-std=c++11" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -std=c++11 -fexceptions")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

add_library(libwiringPi SHARED IMPORTED)
add_library(libwiringPiDev SHARED IMPORTED)
add_library(libmosquitto STATIC IMPORTED)
add_library(libmosquittopp STATIC IMPORTED)

#set(EXTRA_LIBS ${EXTRA_LIBS} ${MOSQT_LIB} ${MOSQT_LIB_CPP} ${WIPI_LIB} ${WIPI_LIB_DEV})
set(EXTRA_LIBS ${EXTRA_LIBS} ${MOSQT_LIB} ${MOSQT_LIB_CPP})
message( STATUS "EXTRA_LIBS:  ${EXTRA_LIBS}")

add_subdirectory(${PROJECT_SOURCE_DIR}/pi-utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-sql)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-mqqt)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-http)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-library)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-smachine)


#add_subdirectory(${PROJECT_SOURCE_DIR}/version1)
#add_subdirectory(${PROJECT_SOURCE_DIR}/project1)
#add_subdirectory(${PROJECT_SOURCE_DIR}/spi_test)
#add_subdirectory(${PROJECT_SOURCE_DIR}/storage)
add_subdirectory(${PROJECT_SOURCE_DIR}/weather)




