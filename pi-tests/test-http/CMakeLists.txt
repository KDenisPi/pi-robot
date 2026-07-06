cmake_minimum_required(VERSION 3.7)

#project name
project(test-http)
enable_testing()

include_directories(BEFORE
    ${MONGOOSE_HOME}
    ${PROJECT_SOURCE_DIR}/../../pi-http
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-mqtt
    ${PROJECT_SOURCE_DIR}/../../pi-env
    ${CJSON}
    )

add_executable(test_http test_http.cpp web_settings_test.cpp)

set(TEST_HTTP_LIBS pi-utils pi-http pi-env ${EXTRA_LIBS} pthread crypt rt)

message( STATUS "HTTP tests Libs: ${TEST_HTTP_LIBS}")

target_link_libraries(test_http ${TEST_HTTP_LIBS})

# Copy runtime resources next to the binary so the relative paths in test_http.cpp
# resolve after the process chdir()s to its own executable directory at startup.
add_custom_command(TARGET test_http POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/default.html
        $<TARGET_FILE_DIR:test_http>/default.html
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../../conf
        $<TARGET_FILE_DIR:test_http>/conf
    COMMENT "Copying test-http runtime resources (default.html, conf) next to the binary"
)

add_test(NAME Http_test COMMAND test_http)
