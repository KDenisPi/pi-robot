cmake_minimum_required(VERSION 3.7)

#project name
project(test-http)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-http
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${CJSON}
    )

add_executable(test_http test_http.cpp)

set(TEST_HTTP_LIBS pi-utils pi-http pthread crypt rt)

message( STATUS "HTTP tests Libs: ${TEST_HTTP_LIBS}")

target_link_libraries(test_http ${TEST_HTTP_LIBS})

add_test(NAME Http_test COMMAND test_http)
