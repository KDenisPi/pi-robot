cmake_minimum_required(VERSION 3.0)

#project name
project(pi-tests)

include_directories(BEFORE ${WIRINGPI_INC}
    ${MOSQUITTO_HOME}/lib/cpp
    ${MOSQUITTO_HOME}/lib
    ${PROJECT_SOURCE_DIR}/../pi-utils
    ${PROJECT_SOURCE_DIR}/../pi-sql
    ${PROJECT_SOURCE_DIR}/../pi-library
    ${PROJECT_SOURCE_DIR}/../pi-smachine
    ${PROJECT_SOURCE_DIR}/../pi-mqqt
    ${PROJECT_SOURCE_DIR}/../pi-http
    ${MONGOOSE_HOME}
    ${JSONCONS}
    ${SQLITE3_HOME}
)

add_subdirectory(${PROJECT_SOURCE_DIR}/test-dma)
