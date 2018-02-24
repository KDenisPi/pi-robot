cmake_minimum_required(VERSION 3.0)

#project name
project(project1)

set(VER_MJR 0)
set(VER_MIN 1)

set(CMAKE_BIULD_TYPE Debug)
set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-library pi-smachine pi-mqqt pthread)

include_directories(BEFORE ${WIRINGPI_LIB}
    ${MOSQUITTO_HOME}/lib/cpp
    ${MOSQUITTO_HOME}/lib
    ${PROJECT_SOURCE_DIR}/../pi-utils
    ${PROJECT_SOURCE_DIR}/../pi-library
    ${PROJECT_SOURCE_DIR}/../pi-smachine
    ${PROJECT_SOURCE_DIR}/../pi-mqqt
    ${JSONCONS}
)

message( STATUS "EXTRA_LIBS (proj):  ${EXTRA_LIBS}" )

aux_source_directory(${PROJECT_SOURCE_DIR} PROJECT1_SOURCES)

add_executable(project1 ${PROJECT1_SOURCES})

target_link_libraries(project1 ${EXTRA_LIBS})

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
