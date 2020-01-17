cmake_minimum_required(VERSION 3.0)

#project name
project(test-mqtt)
enable_testing()

include_directories(BEFORE
    ${JSONCONS}
    ${MONGOOSE_HOME}
    ${MOSQUITTO_HOME}/lib/cpp
    ${MOSQUITTO_HOME}/lib
    ${PROJECT_SOURCE_DIR}/../../pi-main
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-mqtt
    ${PROJECT_SOURCE_DIR}/../../pi-library
    ${PROJECT_SOURCE_DIR}/../../pi-smachine
    ${PROJECT_SOURCE_DIR}/../../pi-http
)

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

add_executable(testMqtt test_mqtt.cpp ${MONGOOSE_SRC})

set(TEST_MQTT_LIBS pi-utils  pi-smachine pi-mqtt pi-library pi-core pthread crypt rt)

message( STATUS "MQTT Libs: ${TEST_MQTT_LIBS}")

target_link_libraries(testMqtt ${TEST_MQTT_LIBS})

add_test(NAME Mqtt COMMAND sudo testMqtt)
#add_test(NAME Dma_1 COMMAND sudo testDma "../pi-tests/test-dma/dma_test.json" )
