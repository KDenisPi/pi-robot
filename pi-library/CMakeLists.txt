cmake_minimum_required(VERSION 3.0)

#project name
project(pi-library)

set(VER_MJR 0)
set(VER_MIN 9)

include_directories(BEFORE
   ${PROJECT_SOURCE_DIR}/../pi-utils
   ${PROJECT_SOURCE_DIR}/../pi-core
   ${JSONCONS}
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_LIBRARY_SOURCES)

#IF(REAL_HARDWARE)
#   add_library(pi-library SHARED ${PI_LIBRARY_SOURCES})
#ELSE(REAL_HARDWARE)
   add_library(pi-library STATIC ${PI_LIBRARY_SOURCES})
#ENDIF(REAL_HARDWARE)


configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
