cmake_minimum_required(VERSION 3.12)

#project name
project(test-gpio)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-library
    ${CJSON}
    )

message( STATUS "GPIO GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "GPIO CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

message( STATUS "GPIO REAL_HARDWARE:  ${REAL_HARDWARE}" )

add_executable(test_gpio test_gpio_simple.cpp)
add_executable(test_gpio_pull test_gpio_simple_pull.cpp)
add_executable(test_gpio_edge test_gpio_simple_edge.cpp)

set(TEST_GPIO_LIBS pi-library pi-utils pthread crypt rt ${EXTRA_LIBS})

if(REAL_HARDWARE)
    find_library(BCM_HOST_LIB bcm_host PATH "/opt/vc/lib")
    set(TEST_GPIO_LIBS ${TEST_GPIO_LIBS}  ${BCM_HOST_LIB})
endif(REAL_HARDWARE)

message( STATUS "GPIO Libs: ${TEST_GPIO_LIBS}")

target_link_libraries(test_gpio ${TEST_GPIO_LIBS})
target_link_libraries(test_gpio_pull ${TEST_GPIO_LIBS})
target_link_libraries(test_gpio_edge ${TEST_GPIO_LIBS})

add_test(NAME Gpio_Simple COMMAND sudo test_gpio)
add_test(NAME Gpio_Simple_Pull COMMAND sudo test_gpio_pull)
add_test(NAME Gpio_Simple_Edge COMMAND sudo test_gpio_edge)

#add_test(NAME Dma_1 COMMAND sudo testDma "../pi-tests/test-dma/dma_test.json" )
