cmake_minimum_required(VERSION 3.0)

#project name
project(pi-http)

set(VER_MJR 0)
set(VER_MIN 9)

include_directories(BEFORE ${MONGOOSE_HOME}
#   ${PROJECT_SOURCE_DIR}/../weather
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_HTTP_SOURCES_MAIN)

set(PI_HTTP_SOURCES ${PI_HTTP_SOURCES_MAIN} ${MONGOOSE_SRC})

IF(REAL_HARDWARE)
   add_library(pi-http SHARED ${PI_HTTP_SOURCES})
ELSE(REAL_HARDWARE)
   add_library(pi-http STATIC ${PI_HTTP_SOURCES})
ENDIF(REAL_HARDWARE)
