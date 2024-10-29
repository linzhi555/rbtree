cmake_minimum_required(VERSION 3.5)

project(example VERSION 0.1 LANGUAGES C CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set( PROJECT_SOURCES
        ./main.cpp
)

set(CMAKE_BUILD_TYPE Debug)
message(${PROJECT_SOURCES})
add_executable(example ${PROJECT_SOURCES})
