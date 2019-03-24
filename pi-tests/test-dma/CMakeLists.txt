cmake_minimum_required(VERSION 3.0)

#project name
project(test-dma)
enable_testing()

include_directories(BEFORE ${WIRINGPI_INC}
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-core
)

message( STATUS "GCC is ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}" )

add_executable(testDma test_dma.cpp)
target_link_libraries(testDma ${EXTRA_LIBS} pi-utils pi-core pthread)

add_test(NAME Dma1 COMMAND testDma)
