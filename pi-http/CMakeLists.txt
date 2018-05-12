cmake_minimum_required(VERSION 3.0)

#project name
project(pi-http)

set(CMAKE_BIULD_TYPE Debug)

include_directories(BEFORE ${MONGOOSE_HOME}
   ${PROJECT_SOURCE_DIR}/../weather
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_HTTP_SOURCES_MAIN)

set(PI_HTTP_SOURCES ${PI_HTTP_SOURCES_MAIN} ${MONGOOSE_SRC})

#add_library(pi-http SHARED ${PI_HTTP_SOURCES})
add_library(pi-http STATIC ${PI_HTTP_SOURCES})
