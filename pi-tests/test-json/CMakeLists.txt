cmake_minimum_required(VERSION 3.0)

#project name
project(test-json)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${CJSON}
)

add_executable(test_cjson test_cjson.cpp)

set(TEST_JSON_LIBS  pi-utils pthread crypt rt)

target_link_libraries(test_cjson ${TEST_JSON_LIBS})

add_test(NAME test-cjson-lib COMMAND test_cjson)
