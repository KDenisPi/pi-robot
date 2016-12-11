cmake_minimum_required(VERSION 3.0)

#project name
project(pi-smachine)

#find_library(RT_LIB rt PATH)

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/../../wiringPi/wiringPi
   ${PROJECT_SOURCE_DIR}/../pi-library
)

#add_library(librt STATIC IMPORTED)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_SMASHINE_SOURCES)

add_library(pi-smachine SHARED ${PI_SMASHINE_SOURCES})

target_link_libraries(pi-smachine rt)

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
