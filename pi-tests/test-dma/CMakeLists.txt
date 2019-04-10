cmake_minimum_required(VERSION 3.0)

#project name
project(test-dma)
enable_testing()

include_directories(BEFORE ${WIRINGPI_INC}
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-core
    ${PROJECT_SOURCE_DIR}/../../pi-library
    ${JSONCONS}
)

message( STATUS "WIRINGPI_INC:  ${WIRINGPI_INC}" )

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

add_executable(testDma test_dma.cpp)

set(TEST_DMA_LIBS pi-utils pi-library pi-core pthread ${WIPI_LIB} ${WIPI_LIB_DEV} crypt rt)


if(REAL_HARDWARE)
find_library(BCM_HOST_LIB bcm_host PATH "/opt/vc/lib")
set(TEST_DMA_LIBS ${TEST_DMA_LIBS}  ${BCM_HOST_LIB})
endif(REAL_HARDWARE)

message( STATUS "Libs: ${TEST_DMA_LIBS}")

target_link_libraries(testDma ${TEST_DMA_LIBS})

add_test(NAME Dma1 COMMAND testDma)
