cmake_minimum_required(VERSION 3.0)

#project name
project(pi-smachine)

include_directories(BEFORE
   ${PROJECT_SOURCE_DIR}/../pi-library
   ${PROJECT_SOURCE_DIR}/../pi-utils
   ${PROJECT_SOURCE_DIR}/../pi-mqtt
   ${CJSON}
)

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_SMASHINE_SOURCES)

add_library(pi-smachine SHARED ${PI_SMASHINE_SOURCES})
add_library(pi-smachine-dev STATIC ${PI_SMASHINE_SOURCES})

add_library(PiRobot::pi-smachine ALIAS pi-smachine)
add_library(PiRobotStatic::pi-smachine-dev ALIAS pi-smachine-dev)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-t
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PROJECT_NAME}.so")
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PI_VER})
set_property(TARGET ${PROJECT_NAME} PROPERTY SVERSION ${PI_VER})

