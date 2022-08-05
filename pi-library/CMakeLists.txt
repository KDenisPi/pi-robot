cmake_minimum_required(VERSION 3.0)

#project name
project(pi-library)

include_directories(BEFORE
   ${PROJECT_SOURCE_DIR}/../pi-utils
   ${PROJECT_SOURCE_DIR}/../pi-core
   ${CJSON}
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_LIBRARY_SOURCES)

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

add_library(pi-library SHARED ${PI_LIBRARY_SOURCES})
add_library(pi-library-dev STATIC ${PI_LIBRARY_SOURCES})

add_library(PiRobot::pi-library ALIAS pi-library)
add_library(PiRobotStatic::pi-library-dev ALIAS pi-library-dev)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-t
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PROJECT_NAME}.so")
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PI_VER})
set_property(TARGET ${PROJECT_NAME} PROPERTY SVERSION ${PI_VER})


