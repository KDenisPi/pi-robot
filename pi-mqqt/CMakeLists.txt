cmake_minimum_required(VERSION 3.0)

#project name
project(pi-mqqt)

include_directories(BEFORE ${MOSQUITTO_HOME}/lib/cpp
   ${MOSQUITTO_HOME}/lib
   ${PROJECT_SOURCE_DIR}/../pi-smashine
   ${PROJECT_SOURCE_DIR}/../pi-utils
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_MQQT_SOURCES)
message( STATUS "PI_MQQT_SOURCES:  ${PI_MQQT_SOURCES}" )


#add_library(pi-mqqt SHARED ${PI_MQQT_SOURCES})
add_library(pi-mqqt STATIC ${PI_MQQT_SOURCES})

target_link_libraries(pi-mqqt)
