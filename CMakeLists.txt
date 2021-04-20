cmake_minimum_required(VERSION 3.7)

project(
    "snake"
    LANGUAGES C
    VERSION 0.1.1
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")


set(C_FLAGS "-Wall")
set(CMAKE_C_FLAGS_RELEASE "${C_FLAGS} -I/usr/include/SDL2 -lSDL2 -lm -O2 -std=c11")
set(
    CMAKE_C_FLAGS
    "${C_FLAGS} -I/usr/include/SDL2 -lSDL2 -lm -O2 -std=c11 -O0 -std=c11"
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} src)

aux_source_directory("src" SNAKE_SOURCES)
add_executable("snake" ${SNAKE_SOURCES})

target_link_libraries(snake "${SDL2_LIBRARIES}")
