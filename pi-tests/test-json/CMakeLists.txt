cmake_minimum_required(VERSION 3.0)

#project name
project(test-json)
enable_testing()

set(CJSON "${THIRD_PARTY_HOME}/cJSON")

include_directories(BEFORE
    ${CJSON}
)

add_executable(test_cjson test_cjson.cpp ${CJSON}/cJSON.c)

set(TEST_JSON_LIBS  crypt rt)

target_link_libraries(test_cjson ${TEST_JSON_LIBS})

add_test(NAME test-cjson-lib COMMAND test_cjson)
