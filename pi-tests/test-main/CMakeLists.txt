cmake_minimum_required(VERSION 3.12)

#
# Genaral test for testing application structire and service components
#
#
#

#project name
project(test-main)
enable_testing()

message( STATUS "REAL_HARDWARE:  ${REAL_HARDWARE}" )
IF(REAL_HARDWARE)
    set(THIRD_PARTY_HOME "${CMAKE_SOURCE_DIR}/../third-party")
ELSE(REAL_HARDWARE)
    set(THIRD_PARTY_HOME "$ENV{HOME}/third-party")
ENDIF(REAL_HARDWARE)

message( STATUS "THIRD_PARTY_HOME: ${THIRD_PARTY_HOME}")

set(JSONCONS "${THIRD_PARTY_HOME}/jsoncons/include")
set(MONGOOSE_HOME "${THIRD_PARTY_HOME}/mongoose")

message( STATUS "THIRD_PARTY_HOME ${THIRD_PARTY_HOME}")
message( STATUS "MONGOOSE_HOME ${MONGOOSE_HOME}")
message( STATUS "CMAKE_BIULD_TYPE is ${CMAKE_BIULD_TYPE}")

#add_library(libmosquitto SHARED IMPORTED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO} -std=c++17 -fexceptions -Wno-psabi")

message( STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO is ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")

IF(USE_SQL_STORAGE)
    set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-sql pi-library pi-smachine pi-mqtt pi-env pi-http mosquitto pthread atomic dl curl rt)
ELSE(USE_SQL_STORAGE)
    set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-library pi-smachine pi-mqtt pi-env pi-http mosquitto pthread atomic dl curl rt)
ENDIF(USE_SQL_STORAGE)

link_directories("${PROJECT_SOURCE_DIR}/../../build")

message( STATUS "EXTRA_LIBS ${EXTRA_LIBS}")
message( STATUS "Link ${PROJECT_SOURCE_DIR}/../../build")

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-main
IF(USE_SQL_STORAGE)
    ${PROJECT_SOURCE_DIR}/../../pi-sql
ENDIF(USE_SQL_STORAGE)
    ${PROJECT_SOURCE_DIR}/../../pi-mqtt
    ${PROJECT_SOURCE_DIR}/../../pi-env
    ${PROJECT_SOURCE_DIR}/../../pi-library
    ${PROJECT_SOURCE_DIR}/../../pi-smachine
    ${PROJECT_SOURCE_DIR}/../../pi-http
    ${PROJECT_SOURCE_DIR}/../../pi-data
    ${MONGOOSE_HOME}
    ${CJSON}
    ${SQLITE3_HOME}
)

message( STATUS "EXTRA_LIBS (proj):  ${EXTRA_LIBS}" )

aux_source_directory(${PROJECT_SOURCE_DIR} MTEST_SOURCES)
add_executable( ${PROJECT_NAME} ${MTEST_SOURCES})
target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS})
