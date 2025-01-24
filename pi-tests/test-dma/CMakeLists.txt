cmake_minimum_required(VERSION 3.12)

#project name
project(test-dma)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-core
    ${PROJECT_SOURCE_DIR}/../../pi-library
    ${CJSON}
    )

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

add_executable(testPMem test_phys_mem.cpp)
add_executable(testDmaInfo test_dma_info.cpp)
add_executable(testDma test_dma.cpp)
add_executable(testPwm test_pwm.cpp)
add_executable(testMBox test_mbox.cpp)

set(TEST_DMA_LIBS pi-utils pi-library pi-core pthread crypt rt ${EXTRA_LIBS})

if(REAL_HARDWARE)
    find_library(BCM_HOST_LIB bcm_host PATH "/opt/vc/lib")
    set(TEST_DMA_LIBS ${TEST_DMA_LIBS}  ${BCM_HOST_LIB})
endif(REAL_HARDWARE)

message( STATUS "Libs: ${TEST_DMA_LIBS}")

target_link_libraries(testPMem ${TEST_DMA_LIBS})
target_link_libraries(testDmaInfo ${TEST_DMA_LIBS})
target_link_libraries(testDma ${TEST_DMA_LIBS})
target_link_libraries(testPwm ${TEST_DMA_LIBS})
target_link_libraries(testMBox ${TEST_DMA_LIBS})

add_test(NAME PMem COMMAND sudo testPMem)
add_test(NAME MBox COMMAND sudo testMBox)
add_test(NAME Pwm_1 COMMAND sudo testPwm)
add_test(NAME Dma_1 COMMAND sudo testDma "../pi-tests/test-dma/dma_test.json" )
