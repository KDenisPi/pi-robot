cmake_minimum_required(VERSION 3.0)

#project name
project(weather)

set(DAEMON ${PROJECT_NAME}d)

set(VER_MJR 1)
set(VER_MIN 0)

set(EXTRA_LIBS ${EXTRA_LIBS} pi-utils pi-sql pi-library pi-smachine pi-mqtt pi-http ${MOSQT_LIB} pthread dl curl)

include_directories(BEFORE
    ${MOSQUITTO_HOME}/lib
    ${PROJECT_SOURCE_DIR}/../pi-utils
    ${PROJECT_SOURCE_DIR}/../pi-data
    ${PROJECT_SOURCE_DIR}/../pi-sql
    ${PROJECT_SOURCE_DIR}/../pi-library
    ${PROJECT_SOURCE_DIR}/../pi-smachine
    ${PROJECT_SOURCE_DIR}/../pi-mqtt
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
# DEB folder structure
#
set(DEB_FOLDER ${DAEMON}_${VER_MJR}.${VER_MIN})
set(DEB_FOLDER_BIN ${DEB_FOLDER}/usr/local/bin)
set(DEB_FOLDER_LIB ${DEB_FOLDER}/usr/local/lib)
set(DEB_FOLDER_ETC ${DEB_FOLDER}/etc)

#
# Installation
#


install(DIRECTORY DESTINATION ${DEB_FOLDER_BIN})
install(DIRECTORY DESTINATION ${DEB_FOLDER_LIB})
install(DIRECTORY DESTINATION ${DEB_FOLDER_ETC}/weather)
install(DIRECTORY DESTINATION ${DEB_FOLDER_ETC}/init.d)


#main executable
install(TARGETS ${DAEMON} RUNTIME DESTINATION ${DEB_FOLDER}/usr/local/bin)


#
# Mosquitto library
#
install(DIRECTORY ${MOSQUITTO_LIB}/
        DESTINATION ${DEB_FOLDER_LIB}
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
        DESTINATION ${DEB_FOLDER_LIB}
        FILES_MATCHING
        PATTERN "libmosquittopp.so.1.5"
        PATTERN "CMakeFiles" EXCLUDE
        PERMISSIONS
            OWNER_EXECUTE OWNER_WRITE OWNER_READ
            GROUP_EXECUTE GROUP_READ
            WORLD_EXECUTE WORLD_READ
    )

#install(DIRECTORY ${MOSQT_LIB_CPP}/
#        DESTINATION ${DEB_FOLDER_LIB}
#        FILES_MATCHING
#        PATTERN "libmosquittopp.so.1"
#        PATTERN "CMakeFiles" EXCLUDE
#        PERMISSIONS
#            OWNER_EXECUTE OWNER_WRITE OWNER_READ
#            GROUP_EXECUTE GROUP_READ
#            WORLD_EXECUTE WORLD_READ
#    )

#install(DIRECTORY ${MOSQT_LIB_CPP}/
#        DESTINATION ${DEB_FOLDER_LIB}
#        FILES_MATCHING
#        PATTERN "libmosquittopp.so"
#        PATTERN "CMakeFiles" EXCLUDE
#        PERMISSIONS
#            OWNER_EXECUTE OWNER_WRITE OWNER_READ
#            GROUP_EXECUTE GROUP_READ
#            WORLD_EXECUTE WORLD_READ
#    )

#configuration file
install(FILES ${PROJECT_SOURCE_DIR}/../conf/Weather.json
    DESTINATION ${DEB_FOLDER_ETC}/weather
    RENAME Weather.json
    PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_READ
        WORLD_EXECUTE WORLD_READ
    )

#daemon configuration file
#install(FILES ${PROJECT_SOURCE_DIR}/daemon/weather
#    DESTINATION ${DEB_FOLDER_ETC}/init.d/
#    PERMISSIONS
#        OWNER_EXECUTE OWNER_WRITE OWNER_READ
#        GROUP_EXECUTE GROUP_READ
#        WORLD_EXECUTE WORLD_READ
#    )

#create log folder
#install(DIRECTORY DESTINATION ${DEB_FOLDER}/var/log/pi-robot
#    DIRECTORY_PERMISSIONS
#        OWNER_EXECUTE OWNER_WRITE OWNER_READ
#        GROUP_EXECUTE GROUP_WRITE GROUP_READ
#        WORLD_EXECUTE WORLD_WRITE WORLD_READ
#    )

#create data folder
#install(DIRECTORY DESTINATION ${DEB_FOLDER}/var/data/pi-robot/data
#    DIRECTORY_PERMISSIONS
#        OWNER_EXECUTE OWNER_WRITE OWNER_READ
#        GROUP_EXECUTE GROUP_WRITE GROUP_READ
#        WORLD_EXECUTE WORLD_WRITE WORLD_READ
#    )
