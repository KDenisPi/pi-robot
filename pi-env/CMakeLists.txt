cmake_minimum_required(VERSION 3.7)

#project name
project(pi-env)

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../pi-utils
    ${PROJECT_SOURCE_DIR}/../pi-mqtt
    ${CJSON}
)

#configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_ENV_SOURCES)

add_library(pi-env SHARED ${PI_ENV_SOURCES})
add_library(PiRobot::pi-env ALIAS pi-env)

if(MAKE_STATIC_LIB)
add_library(pi-env-dev STATIC ${PI_ENV_SOURCES})
endif(MAKE_STATIC_LIB)

if(MAKE_DEV_LIB)
add_library(PiRobotStatic::pi-env-dev ALIAS pi-env-dev)
endif(MAKE_DEV_LIB)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-t
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PROJECT_NAME}.so")
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PI_VER})
set_property(TARGET ${PROJECT_NAME} PROPERTY SVERSION ${PI_VER})

