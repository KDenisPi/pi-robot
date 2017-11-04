cmake_minimum_required(VERSION 3.0)

#project name
project(pi-smachine)

#find_library(RT_LIB rt PATH)

include_directories(BEFORE ${WIRINGPI_LIB}
   ${PROJECT_SOURCE_DIR}/../pi-library
   ${PROJECT_SOURCE_DIR}/../pi-utils
)

#add_library(librt STATIC IMPORTED)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_SMASHINE_SOURCES)

#add_library(pi-smachine SHARED ${PI_SMASHINE_SOURCES})
add_library(pi-smachine STATIC ${PI_SMASHINE_SOURCES})

target_link_libraries(pi-smachine pi-utils rt)

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
