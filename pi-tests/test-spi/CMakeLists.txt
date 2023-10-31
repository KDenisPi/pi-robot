cmake_minimum_required(VERSION 3.12)

#project name
project(test-spi)
enable_testing()

include_directories(AFTER
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-library
    ${CJSON}
    )

get_directory_property (INC_DIRS INCLUDE_DIRECTORIES)
message( STATUS "INCLUDE_DIRECTORIES: ${PROJECT_NAME} ${INC_DIRS}")

message( STATUS "SPI GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "SPI CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

message( STATUS "SPI REAL_HARDWARE:  ${REAL_HARDWARE}" )

add_executable(test_spi_arduino arduino.cpp)

set(TEST_SPI_LIBS pi-utils pi-library pthread crypt rt)

if(REAL_HARDWARE)
    find_library(BCM_HOST_LIB bcm_host PATH "/opt/vc/lib")
    set(TEST_SPI_LIBS ${TEST_SPI_LIBS}  ${BCM_HOST_LIB})
endif(REAL_HARDWARE)

message( STATUS "SPI Libs: ${TEST_SPI_LIBS}")

target_link_libraries(test_spi_arduino ${TEST_SPI_LIBS})

add_test(NAME Arduino COMMAND sudo test_spi_arduino)
