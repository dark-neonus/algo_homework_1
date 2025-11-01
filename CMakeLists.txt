cmake_minimum_required(VERSION 3.10)
project(algo_homework_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(algo_homework_1
    src/main.cpp
)
