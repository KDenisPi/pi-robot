cmake_minimum_required(VERSION 3.0)

#project name
project(pi-utils)

include_directories(BEFORE
    "${SPDLOG_INCLUDE}/spdlog"
    ${SPDLOG_INCLUDE}
)


set(CMAKE_BIULD_TYPE Debug)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_UTILS_SOURCES)

add_library(pi-utils STATIC ${PI_UTILS_SOURCES})
