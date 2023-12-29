cmake_minimum_required(VERSION 3.12)

#
# Genaral test for testing application structire and service components
#
#
#

#project name
project(test-main)
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-sql pi-library pi-smachine pi-mqtt pi-http mosquitto pthread dl curl rt)

IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    message("test-main 64")
    set(EXTRA_LIBS  ${EXTRA_LIBS} fmt)
ENDIF()

link_directories("${PROJECT_SOURCE_DIR}/../../build")

message( STATUS "EXTRA_LIBS ${EXTRA_LIBS}")
message( STATUS "Link ${PROJECT_SOURCE_DIR}/../../build")

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-main
    ${PROJECT_SOURCE_DIR}/../../pi-sql
    ${PROJECT_SOURCE_DIR}/../../pi-library
    ${PROJECT_SOURCE_DIR}/../../pi-smachine
    ${PROJECT_SOURCE_DIR}/../../pi-mqtt
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
