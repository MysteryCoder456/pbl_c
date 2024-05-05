cmake_minimum_required(VERSION 3.10)
project(Snake VERSION 1.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

add_executable(Snake src/main.c)
target_include_directories(Snake PUBLIC src)
target_link_libraries(Snake ncurses)
