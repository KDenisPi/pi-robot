cmake_minimum_required(VERSION 3.7)

#project name
project(test-timers)
enable_testing()

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/../../pi-utils
    ${PROJECT_SOURCE_DIR}/../../pi-smachine
)

add_executable(test_timers test_timer.cpp)

set(TEST_TIMERS_LIBS  pi-utils pi-smachine pi-library pthread atomic crypt rt ${EXTRA_LIBS})

target_link_libraries(test_timers ${TEST_TIMERS_LIBS})

add_test(NAME test-timers COMMAND test_timers)
