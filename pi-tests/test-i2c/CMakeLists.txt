cmake_minimum_required(VERSION 3.0)

#project name
project(test-i2c)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-library
    ${CJSON}
    )

message( STATUS "I2C GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "I2C CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

message( STATUS "I2C REAL_HARDWARE:  ${REAL_HARDWARE}" )

add_executable(test_i2c_tsl2561 test_i2c_tsl2561.cpp)
add_executable(test_i2c_sgp30 test_i2c_sgp30.cpp)
add_executable(test_i2c_bmp280 test_i2c_bmp280.cpp)

set(TEST_I2C_LIBS pi-library pi-utils pthread crypt rt ${EXTRA_LIBS})

if(REAL_HARDWARE)
    find_library(BCM_HOST_LIB bcm_host PATH "/opt/vc/lib")
    set(TEST_I2C_LIBS ${TEST_I2C_LIBS}  ${BCM_HOST_LIB})
endif(REAL_HARDWARE)

message( STATUS "I2C Libs: ${TEST_I2C_LIBS}")

target_link_libraries(test_i2c_tsl2561 ${TEST_I2C_LIBS})
target_link_libraries(test_i2c_sgp30 ${TEST_I2C_LIBS})
target_link_libraries(test_i2c_bmp280 ${TEST_I2C_LIBS})

add_test(NAME Gpio_i2c_tsk2561 COMMAND sudo test_i2c_tsl2561)
add_test(NAME Gpio_i2c_sgp30 COMMAND sudo test_i2c_sgp30)
add_test(NAME Gpio_i2c_bmp280 COMMAND sudo test_i2c_bmp280)
