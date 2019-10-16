cmake_minimum_required(VERSION 3.15)
project(Bands C)

set(CMAKE_C_STANDARD 99)

add_executable(Bands main.c)
target_link_libraries(Bands m)