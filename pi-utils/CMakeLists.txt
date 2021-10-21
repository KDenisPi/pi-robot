cmake_minimum_required(VERSION 3.0)

#project name
project(pi-utils)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_UTILS_SOURCES)

add_library(pi-utils SHARED ${PI_UTILS_SOURCES})
add_library(pi-utils-dev STATIC ${PI_UTILS_SOURCES})

add_library(PiRobot::pi-utils ALIAS pi-utils)
add_library(PiRobotStatic::pi-utils-dev ALIAS pi-utils-dev)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-t
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PROJECT_NAME}.so")
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PI_VER})
set_property(TARGET ${PROJECT_NAME} PROPERTY SVERSION ${PI_VER})
