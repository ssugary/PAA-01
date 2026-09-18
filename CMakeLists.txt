# To make the LSP work, also needed to:
# rm -rf build
# cmake -B build
# ln -s build/compile_commands.json .

cmake_minimum_required(VERSION 3.16)
project(OPTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug) # just for gdb

set(CMAKE_BUILD_TYPE Release) # for fastier math calcs
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -ffast-math")

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${LIB_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/include
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic 
)                          
