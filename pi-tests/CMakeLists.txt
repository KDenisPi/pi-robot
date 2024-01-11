cmake_minimum_required(VERSION 3.12)

#project name
project(pi-tests)

set(TEST_LIBS  pi-utils pthread crypt rt)

IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    message("pi-utils 64")
    set(TEST_LIBS  ${TEST_LIBS} fmt)
ENDIF()

add_subdirectory(${PROJECT_SOURCE_DIR}/test-gpio)
add_subdirectory(${PROJECT_SOURCE_DIR}/test-i2c)
#add_subdirectory(${PROJECT_SOURCE_DIR}/test-dma)
add_subdirectory(${PROJECT_SOURCE_DIR}/test-main)
add_subdirectory(${PROJECT_SOURCE_DIR}/test-json)
add_subdirectory(${PROJECT_SOURCE_DIR}/test-spi)
