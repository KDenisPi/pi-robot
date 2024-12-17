cmake_minimum_required(VERSION 3.7)

#project name
project(test-general)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-mqtt
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${CJSON}
    )

add_executable(test_cbuff test_cbuff.cpp)

set(TEST_GENERAL_LIBS ${EXTRA_LIBS} pi-utils pi-mqtt pthread crypt rt)

message( STATUS "General tests Libs: ${TEST_GENERAL_LIBS}")

target_link_libraries(test_cbuff ${TEST_GENERAL_LIBS})

add_test(NAME Gpio_CBuff COMMAND sudo test_cbuff)
