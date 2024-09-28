cmake_minimum_required(VERSION 3.0)

#project name
project(test-timers)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
)

add_executable(test_timers test_timer.cpp Timer.cpp)

set(TEST_TIMERS_LIBS  pi-utils pthread crypt rt ${EXTRA_LIBS})

target_link_libraries(test_timers ${TEST_TIMERS_LIBS})

add_test(NAME test-timers COMMAND test_timers)
