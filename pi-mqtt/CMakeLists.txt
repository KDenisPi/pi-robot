cmake_minimum_required(VERSION 3.12)

#project name
project(pi-mqtt)

include_directories(AFTER
   ${PROJECT_SOURCE_DIR}/../pi-smashine
   ${PROJECT_SOURCE_DIR}/../pi-utils
   ${CJSON}
)

get_directory_property (INC_DIRS INCLUDE_DIRECTORIES)
message( STATUS "INCLUDE_DIRECTORIES: ${PROJECT_NAME} ${INC_DIRS}")

aux_source_directory(${PROJECT_SOURCE_DIR} PI_MQTT_SOURCES)

add_library(pi-mqtt SHARED ${PI_MQTT_SOURCES})
#add_library(pi-mqtt-dev STATIC ${PI_MQTT_SOURCES})

add_library(PiRobot::pi-mqtt ALIAS pi-mqtt)
#add_library(PiRobotStatic::pi-mqtt-dev ALIAS pi-mqtt-dev)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-t
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PROJECT_NAME}.so")
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PI_VER})
set_property(TARGET ${PROJECT_NAME} PROPERTY SVERSION ${PI_VER})
