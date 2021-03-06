cmake_minimum_required(VERSION 3.0)

#project name
project(weather)

set(DAEMON ${PROJECT_NAME}d)

set(VER_MJR 0)
set(VER_MIN 9)

set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-sql pi-library pi-smachine pi-mqqt pi-http pthread dl curl)

include_directories(BEFORE
    ${MOSQUITTO_HOME}/lib/cpp
    ${MOSQUITTO_HOME}/lib
    ${PROJECT_SOURCE_DIR}/../pi-utils
    ${PROJECT_SOURCE_DIR}/../pi-sql
    ${PROJECT_SOURCE_DIR}/../pi-library
    ${PROJECT_SOURCE_DIR}/../pi-smachine
    ${PROJECT_SOURCE_DIR}/../pi-mqqt
    ${PROJECT_SOURCE_DIR}/../pi-http
    ${MONGOOSE_HOME}
    ${JSONCONS}
    ${SQLITE3_HOME}
)

message( STATUS "EXTRA_LIBS (proj):  ${EXTRA_LIBS}" )

aux_source_directory(${PROJECT_SOURCE_DIR} WEATHER_SOURCES)

add_executable(${DAEMON} ${WEATHER_SOURCES})
target_link_libraries(${DAEMON} ${EXTRA_LIBS} crypt)

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

#
# Installation
#

#main executable
install(TARGETS ${DAEMON} RUNTIME DESTINATION bin)

#
# Mosquitto library
#
install(DIRECTORY ${MOSQUITTO_LIB}/
        DESTINATION lib
        FILES_MATCHING
        PATTERN "libmosquitto.*"
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN "cpp" EXCLUDE
        PERMISSIONS
            OWNER_EXECUTE OWNER_WRITE OWNER_READ
            GROUP_EXECUTE GROUP_READ
            WORLD_EXECUTE WORLD_READ
    )

install(DIRECTORY ${MOSQT_LIB_CPP}/
        DESTINATION lib
        FILES_MATCHING
        PATTERN "libmosquittopp.*"
        PATTERN "CMakeFiles" EXCLUDE
        PERMISSIONS
            OWNER_EXECUTE OWNER_WRITE OWNER_READ
            GROUP_EXECUTE GROUP_READ
            WORLD_EXECUTE WORLD_READ
    )

#configuration file
install(FILES ${PROJECT_SOURCE_DIR}/../conf/Weather.json
    DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/weather
    RENAME Weather.json
    PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_READ
        WORLD_EXECUTE WORLD_READ
    )

#daemon configuration file
install(FILES ${PROJECT_SOURCE_DIR}/daemon/weather
    DESTINATION /etc/init.d/
    PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_READ
        WORLD_EXECUTE WORLD_READ
    )

#create log folder
install(DIRECTORY DESTINATION /var/log/pi-robot
    DIRECTORY_PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_WRITE GROUP_READ
        WORLD_EXECUTE WORLD_WRITE WORLD_READ
    )

#create data folder
install(DIRECTORY DESTINATION /var/data/pi-robot/data
    DIRECTORY_PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_WRITE GROUP_READ
        WORLD_EXECUTE WORLD_WRITE WORLD_READ
    )
