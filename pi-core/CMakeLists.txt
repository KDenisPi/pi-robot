cmake_minimum_required(VERSION 3.0)

#project name
project(pi-core)

include_directories(
   ${PROJECT_SOURCE_DIR}/../pi-utils
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_CORE_SOURCES)
add_library(pi-core SHARED ${PI_CORE_SOURCES})
add_library(pi-core-dev STATIC ${PI_CORE_SOURCES})

add_library(PiRobot::pi-core ALIAS pi-core)
add_library(PiRobotStatic::pi-core-dev ALIAS pi-core-dev)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-t
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PROJECT_NAME}.so")
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PI_VER})
set_property(TARGET ${PROJECT_NAME} PROPERTY SVERSION ${PI_VER})

