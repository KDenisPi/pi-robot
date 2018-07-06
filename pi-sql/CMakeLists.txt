cmake_minimum_required(VERSION 3.0)

#project name
project(pi-sql)

set(CMAKE_BIULD_TYPE Debug)

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../pi-utils
    ${SQLITE3_HOME}
    ${SPDLOG_INCLUDE}
)

aux_source_directory(${PROJECT_SOURCE_DIR} PI_SQL_SOURCES)
aux_source_directory(${SQLITE3_HOME} SQLITE_SOURCES)

add_library(pi-sql STATIC ${PI_UTILS_SOURCES} ${SQLITE_SOURCES})
