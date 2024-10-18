cmake_minimum_required(VERSION 3.0)

#project name
project(pi-sql)

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../pi-utils
    ${SQLITE3_HOME}
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_SQL_SOURCES)
aux_source_directory(${SQLITE3_HOME} SQLITE_SOURCES)

add_library(pi-sql SHARED ${PI_SQL_SOURCES} ${SQLITE_SOURCES})
add_library(PiRobot::pi-sql ALIAS pi-sql)

if(MAKE_STATIC_LIB)
add_library(pi-sql-dev STATIC ${PI_SQL_SOURCES} ${SQLITE_SOURCES})
endif(MAKE_STATIC_LIB)

if(MAKE_DEV_LIB)
add_library(PiRobotStatic::pi-sql-dev ALIAS pi-sql-dev)
endif(MAKE_DEV_LIB)


install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-t
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PROJECT_NAME}.so")
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PI_VER})
set_property(TARGET ${PROJECT_NAME} PROPERTY SVERSION ${PI_VER})
