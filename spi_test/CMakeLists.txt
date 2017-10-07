cmake_minimum_required(VERSION 3.0)

#project name
project(spi_test)

set(VER_MJR 0)
set(VER_MIN 1)

set(CMAKE_BIULD_TYPE Debug)
set(EXTRA_LIBS ${EXTRA_LIBS} pi-library pi-smachine pi-utils pthread)

include_directories(BEFORE ${WIRINGPI_LIB}
   ${PROJECT_SOURCE_DIR}/../pi-library
   ${PROJECT_SOURCE_DIR}/../pi-utils
   ${PROJECT_SOURCE_DIR}/../pi-smachine
)

message( STATUS "EXTRA_LIBS (proj):  ${EXTRA_LIBS}" )

aux_source_directory(${PROJECT_SOURCE_DIR} SPI_TEST_SOURCES)

add_executable(spi_test ${SPI_TEST_SOURCES})

target_link_libraries(spi_test ${EXTRA_LIBS})

#configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)
