cmake_minimum_required(VERSION 3.0)

#project name
project(pi-robot)
enable_testing()

set(THIRD_PARTY_HOME "${CMAKE_SOURCE_DIR}/../third-party")

set(MOSQUITTO_HOME "${THIRD_PARTY_HOME}/mosquitto")
set(MOSQUITTO_LIB "${MOSQUITTO_HOME}/build/lib")

set(JSONCONS "${THIRD_PARTY_HOME}/jsoncons/include")

set(MONGOOSE_HOME "${THIRD_PARTY_HOME}/mongoose-5.1")
set(MONGOOSE_SRC "${MONGOOSE_HOME}/mongoose.c")

set(SQLITE3_HOME "${THIRD_PARTY_HOME}/sqlite-amalgamation-3240000")

message( STATUS "THIRD_PARTY_HOME ${THIRD_PARTY_HOME}")
message( STATUS "MOSQUITTO_LIB ${MOSQUITTO_LIB}")
message( STATUS "SPDLOG_HOME ${SPDLOG_HOME}")
message( STATUS "JSONCONS ${JSONCONS}")
message( STATUS "MONGOOSE_HOME ${MONGOOSE_HOME}")
message( STATUS "MONGOOSE_SRC ${MONGOOSE_SRC}")

find_library(MOSQT_LIB mosquitto PATH ${MOSQUITTO_LIB})
find_library(MOSQT_LIB_CPP mosquittopp PATH ${MOSQUITTO_LIB}/cpp)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

#set(CMAKE_BIULD_TYPE Debug)
#set(CMAKE_BIULD_TYPE RelWithDebInfo)
message( STATUS "CMAKE_BIULD_TYPE is ${CMAKE_BIULD_TYPE}")

#cmake version 3.0 and above
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#
# C++ 11
#
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -g -std=c++11 -fexceptions -Wno-psabi -Og")
ELSE(CMAKE_BUILD_TYPE NOT STREQUAL "Debug")
    message("release with debug info mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO} -std=c++11 -fexceptions -Wno-psabi")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message( STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO is ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message( STATUS "CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

add_library(libmosquitto STATIC IMPORTED)
add_library(libmosquittopp STATIC IMPORTED)

#set(EXTRA_LIBS ${EXTRA_LIBS} ${MOSQT_LIB} ${MOSQT_LIB_CPP})
set(EXTRA_LIBS ${EXTRA_LIBS} ${MOSQT_LIB} ${MOSQT_LIB_CPP})
message( STATUS "EXTRA_LIBS:  ${EXTRA_LIBS}")

add_subdirectory(${PROJECT_SOURCE_DIR}/pi-utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-core)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-sql)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-mqqt)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-http)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-library)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-smachine)

add_subdirectory(${PROJECT_SOURCE_DIR}/pi-tests)

#add_subdirectory(${PROJECT_SOURCE_DIR}/version1)
#add_subdirectory(${PROJECT_SOURCE_DIR}/project1)
#add_subdirectory(${PROJECT_SOURCE_DIR}/spi_test)
#add_subdirectory(${PROJECT_SOURCE_DIR}/storage)
#add_subdirectory(${PROJECT_SOURCE_DIR}/weather)
