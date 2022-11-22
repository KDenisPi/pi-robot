cmake_minimum_required(VERSION 3.0)

#project name
project(pi-robot VERSION 1.2.4)
enable_testing()

set(PI_VER ${PROJECT_VERSION})

message( STATUS "REAL_HARDWARE:  ${REAL_HARDWARE}" )
IF(REAL_HARDWARE)
    set(THIRD_PARTY_HOME "${CMAKE_SOURCE_DIR}/../third-party")
ELSE(REAL_HARDWARE)
    set(THIRD_PARTY_HOME "$ENV{HOME}/third-party")
ENDIF(REAL_HARDWARE)

message( STATUS "THIRD_PARTY_HOME: ${THIRD_PARTY_HOME}")

#set(JSONCONS "${THIRD_PARTY_HOME}/jsoncons/include")
set(MONGOOSE_HOME "${THIRD_PARTY_HOME}/mongoose-5.1")
set(MONGOOSE_SRC "${MONGOOSE_HOME}/mongoose.c")
set(SQLITE3_HOME "${THIRD_PARTY_HOME}/sqlite-amalgamation-3240000")
set(CJSON "${THIRD_PARTY_HOME}/cJSON")

message( STATUS "CJSON: ${CJSON}")
message( STATUS "THIRD_PARTY_HOME ${THIRD_PARTY_HOME}")
message( STATUS "MONGOOSE_HOME ${MONGOOSE_HOME}")
message( STATUS "MONGOOSE_SRC ${MONGOOSE_SRC}")
message( STATUS "CMAKE_BIULD_TYPE is ${CMAKE_BIULD_TYPE}")

#cmake version 3.0 and above
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

message( STATUS "CMAKE_BUILD_TYPE:  ${CMAKE_BUILD_TYPE}" )

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# C++ 11
#
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -g -std=c++14 -fexceptions -Wno-psabi -Og")
ELSE(CMAKE_BUILD_TYPE NOT STREQUAL "Debug")
    message("release with debug info mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO} -std=c++14 -fexceptions -Wno-psabi")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message( STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO is ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message( STATUS "CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )
message( STATUS "CMAKE_C_FLAGS:  ${CMAKE_C_FLAGS}" )
 

#libspdlog-dev/focal,now 1:1.5.0-1 amd64
#libspdlog1/focal,now 1:1.5.0-1 amd64 [installed,automatic]

#libmosquitto-dev/focal 1.6.9-1 amd64
#libmosquitto1/focal 1.6.9-1 amd64

add_library(libmosquitto SHARED IMPORTED)

set(EXTRA_LIBS ${EXTRA_LIBS} mosquitto)
message( STATUS "EXTRA_LIBS:  ${EXTRA_LIBS}")

#include_directories(BEFORE
#    /usr/include
#)

#get_directory_property (INC_DIRS INCLUDE_DIRECTORIES)
#message( STATUS "INCLUDE_DIRECTORIES:  ${INC_DIRS}")

add_subdirectory(${PROJECT_SOURCE_DIR}/pi-utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-core)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-sql)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-mqtt)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-http)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-library)
add_subdirectory(${PROJECT_SOURCE_DIR}/pi-smachine)

add_subdirectory(${PROJECT_SOURCE_DIR}/pi-tests)

#
#Do not uncomment. Build it separately
#
#add_subdirectory(${PROJECT_SOURCE_DIR}/weather)


#
# Install section
#
set(PROJECT_VENDOR "Denis Kudia")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

# Generator name - Debian packages
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME lib${PROJECT_NAME})

#libspdlog
set(CPACK_DEBIAN_PACKAGE_DEPENDS "mosquitto (>= 1.6.8)")

# package information
set(CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR})
set(CPACK_PACKAGE_CONTACT ${PROJECT_VENDOR})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pi-Robot Debian package")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})

# package version
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

message( STATUS "CMAKE_SYSTEM_PROCESSOR:  ${CMAKE_SYSTEM_PROCESSOR}" )

IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
ELSE(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
ENDIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})

message( STATUS "CPACK_DEBIAN_PACKAGE_ARCHITECTURE:  ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}" )

install(EXPORT pi-utils-t NAMESPACE PiRobot DESTINATION lib)
install(EXPORT pi-core-t NAMESPACE PiRobot DESTINATION lib)
install(EXPORT pi-sql-t NAMESPACE PiRobot DESTINATION lib)
install(EXPORT pi-mqtt-t NAMESPACE PiRobot DESTINATION lib)
install(EXPORT pi-http-t NAMESPACE PiRobot DESTINATION lib)
install(EXPORT pi-library-t NAMESPACE PiRobot DESTINATION lib)
install(EXPORT pi-smachine-t NAMESPACE PiRobot DESTINATION lib)

include(CPack)



