cmake_minimum_required(VERSION 3.0)

#project name
project(pi-mqtt)

include_directories(BEFORE ${MOSQUITTO_HOME}/lib/cpp
   ${MOSQUITTO_HOME}/lib
   ${PROJECT_SOURCE_DIR}/../pi-smashine
   ${PROJECT_SOURCE_DIR}/../pi-utils
   ${JSONCONS}
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_MQTT_SOURCES)
message( STATUS "PI_MQTT_SOURCES:  ${PI_MQTT_SOURCES}" )


#add_library(pi-mqtt SHARED ${PI_MQTT_SOURCES})
add_library(pi-mqtt STATIC ${PI_MQTT_SOURCES})

target_link_libraries(pi-mqtt)
