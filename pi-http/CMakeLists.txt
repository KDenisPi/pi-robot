cmake_minimum_required(VERSION 3.7)

#project name
project(pi-http)

include_directories(BEFORE ${MONGOOSE_HOME}
   ${PROJECT_SOURCE_DIR}/../weather
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_HTTP_SOURCES_MAIN)

set(PI_HTTP_SOURCES ${PI_HTTP_SOURCES_MAIN} ${MONGOOSE_SRC})

add_library(pi-http SHARED ${PI_HTTP_SOURCES})
add_library(PiRobot::pi-http ALIAS pi-http)

if(MAKE_STATIC_LIB)
add_library(pi-http-dev STATIC ${PI_HTTP_SOURCES})
endif(MAKE_STATIC_LIB)

if(MAKE_DEV_LIB)
add_library(PiRobotStatic::pi-http-dev ALIAS pi-http-dev)
endif(MAKE_DEV_LIB)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-t
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PROJECT_NAME}.so")
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PI_VER})
set_property(TARGET ${PROJECT_NAME} PROPERTY SVERSION ${PI_VER})


